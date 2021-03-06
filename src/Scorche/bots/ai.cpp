//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "ai.h"
#include "../console.h"
#include "../game.h"
#include "../playerinfo.h"
#include "../shop.h"
#include "../tanks/tankbase.h"
#include "../weapons/weapon.h"
#include "../weaponlist.h"
#include <cmath>
#include <PixelEngine/world.h>
#include <PixelEngine/pemath.h>
#include <PixelEngine/vector.h>

QString AI::AIStateToString(AI::AI_State State)
{
    switch(State)
    {
        case AI_State_Fired:
            return "fired";
        case AI_State_Undecided:
            return "undecided";
        case AI_State_Obstructed:
            return "obstructed";
        case AI_State_Trace_Wait:
            return "trace-wait";
        case AI_State_Nothing_To_Do:
            return "nothing-to-do";
        case AI_State_Waiting_Angle:
            return "waiting-angle";
        case AI_State_PreFire_Trace_Wait:
            return "pre-fire-trace-wait";
        case AI_State_PreFire_Trace_Analyze:
            return "pre-fire-trace-analyze";
    }
    return "unknown";
}

AI::AI(TankBase *t)
{
    this->tank = t;
}

AI::~AI()
{
    qDeleteAll(this->metrics);
}

void AI::Process()
{
    if (this->selectedEnemy != nullptr && !this->selectedEnemy->IsAlive())
    {
        debug_log("enemy was killed, looking for a new one");
        this->selectedEnemy = nullptr;
        this->changeState(AI_State_Undecided);
    }
    if (this->tank->LastDamagedBy != nullptr && this->tank->LastDamagedBy != this->tank && this->tank->LastDamagedBy != this->selectedEnemy && this->tank->LastDamagedBy->IsAlive())
    {
        // Someone damaged us and it's not our current enemy, let's make them that!
        this->selectedEnemy = this->tank->LastDamagedBy;
        debug_log("changing enemy to " + this->selectedEnemy->PlayerName + " because they attacked us");
        this->resetEnemy();
        this->evaluateWeapon();
        this->changeState(AI_State_Undecided);
        this->tank->LastDamagedBy = nullptr;
    }
    this->evaluateShield();
    this->evaluateSonic();
    this->evaluateRepair();
    switch(this->state)
    {
        case AI_State_Trace_Wait:
            this->traceEval();
            return;
        case AI_State_Obstructed:
            this->Fire();
            return;
        case AI_State_Undecided:
            debug_log("is undecided - thinking what to do best next");
            if (this->selectedEnemy == nullptr)
            {
                debug_log("looking for a new enemy");
                this->selectedEnemy = FindClosestEnemy();
            }

            if (this->selectedEnemy == nullptr)
            {
                debug_log("impossible to find enemy");
                this->changeState(AI_State_Nothing_To_Do);
                return;
            }
            else
            {
                debug_log("enemy found: " + this->selectedEnemy->PlayerName);
                this->resetEnemy();
                this->evaluateWeapon();
                this->getTargetAngle();
                debug_log("new target angle calculated, sending tracers: " + QString::number(this->targetAngle));
                this->trace();
                return;
            }
            break;
        case AI_State_Waiting_Angle:
            if (Game::AIQuickAim)
            {
                this->tank->SetAngle(this->targetAngle);
                this->tank->SetPower(this->targetPower);
                this->tank->ResetCanonAdjust();
                this->Fire();
                return;
            }
            if (std::abs(this->tank->GetCanonAngle() - this->targetAngle) < this->targetTolerance)
            {
                this->tank->ResetCanonAdjust();
                this->tank->SetPower(this->targetPower);
                this->Fire();
                return;
            }
            if (this->tank->GetCanonAngle() > this->targetAngle)
            {
                this->tank->SetCanonAdjustRight();
            } else if (this->tank->GetCanonAngle() < this->targetAngle)
            {
                this->tank->SetCanonAdjustLeft();
            }
            break;
        case AI_State_Nothing_To_Do:
            this->tank->Pass();
            break;
        case AI_State_Fired:
            this->prevEvaluation = this->lastEvaluation;
            this->evaluateWeapon();
            this->evaluateFire();
            this->postEvaluateFire();
            break;
    }
}

TankBase *AI::FindClosestEnemy()
{
    double best = AI_WORST_DISTANCE;
    TankBase *tank = nullptr;
    // Just find the most close one
    foreach (TankBase *p, TankBase::Players)
    {
        if (p == this->tank)
            continue;
        if (!p->IsAlive())
            continue;

        double distance = this->tank->Position.DistanceTo(p->Position);
        if (distance > best)
            continue;

        best = distance;
        tank = p;
    }
    return tank;
}

void AI::ProcessInventory()
{
    // Spend most of money for everything
    PlayerInfo *info = this->tank->GetPlayer();
    bool needs_more = true;
    while(needs_more)
    {
        needs_more = false;
        // We always need shields
        if (info->Cash > 600 && info->ItemList[INVENTORY_SHIELD] < 2)
        {
            Shop::DefaultShop->BuyItem(info, INVENTORY_SHIELD);
            needs_more = true;
        }
        if (info->Cash > 3500 && info->ItemList[INVENTORY_HEAVY_SHIELD] < 2)
        {
            Shop::DefaultShop->BuyItem(info, INVENTORY_HEAVY_SHIELD);
            needs_more = true;
        }
        if (info->Cash > 100 && info->ItemList[WEAPON_SONIC_BOMB] < 2)
        {
            Shop::DefaultShop->BuyItem(info, WEAPON_SONIC_BOMB);
            needs_more = true;
        }
        if (info->Cash > 10000 && info->ItemList[WEAPON_NUKE] < 2)
        {
            Shop::DefaultShop->BuyItem(info, WEAPON_NUKE);
            needs_more = true;
        }
        {
            //Shop::DefaultShop->BuyItem(info, WEAPON_NUKE);
        }
        if (info->Cash > 2000 && info->ItemList[WEAPON_MINI_NUKE] < 2)
        {
            Shop::DefaultShop->BuyItem(info, WEAPON_MINI_NUKE);
            needs_more = true;
        }

        if (info->Cash > 10 && info->ItemList[WEAPON_BIG_CANON] < 60)
        {
            Shop::DefaultShop->BuyItem(info, WEAPON_BIG_CANON);
            needs_more = true;
        }
    }
}

QString AI::GetAIModelName()
{
    return "Default";
}

void AI::Fire()
{
    this->print_debug_stats();
    this->firstShot = false;
    this->changeState(AI_State_Fired);
    this->tank->Fire();
}

void AI::WarmUp()
{
    if (this->tank->GetPlayer()->ItemList[INVENTORY_HEAVY_SHIELD] > 0)
    {
        this->tank->DeployShield(ShieldHeavy);
    } else if (this->tank->GetPlayer()->ItemList[INVENTORY_SHIELD] > 0)
    {
        this->tank->DeployShield(ShieldMini);
    }
}

void AI::getTargetAngle()
{
    //this->targetAngle = this->tank->Position.AngleTo(this->selectedEnemy->Position);
    if (this->enemySide == AI_Left)
        this->targetAngle = 1.6;
    else
        this->targetAngle = 0.8;
    this->improveAngle(0.2);
}

void AI::improveAngle(double max)
{
    qint32 x = static_cast<qint32>(max * 100);
    double rnd = static_cast<double>(PE::PEMath::GetRandom(-1 * x, x)) / 100;
    double old_angle = this->targetAngle;
    this->changeAngle(this->targetAngle + rnd);
    this->normalizeAngle();
    while (old_angle == this->targetAngle)
        this->improveAngle(max);
}

void AI::improvePower(double max)
{
    this->increasePower(PE::PEMath::GetRandom(static_cast<qint32>(-1 * max), static_cast<qint32>(2 * max)));
}

void AI::resetEnemy()
{
    debug_log("resetting metrics and evaluation history data");
    qDeleteAll(this->metrics);
    // Last hit is irrelevant
    this->tank->LastHit = PE::Vector::Zero;
    this->lastDistanceFromSelf = AI_WORST_DISTANCE;
    this->metrics.clear();
    this->firstShot = true;
    this->bestPower = this->tank->GetMaxPower();
    this->bestDistance = AI_WORST_DISTANCE;
    this->previousEnemyHP = this->selectedEnemy->Health + this->selectedEnemy->ShieldPower;
    this->unknownDataCounter = 0;
    double distance = this->tank->Position.DistanceTo(this->selectedEnemy->Position);
    if (this->selectedEnemy->Position.X > this->tank->Position.X)
    {
        this->enemySide = AI_Right;
    } else
    {
        this->enemySide = AI_Left;
    }
    if (distance > 800)
        this->tank->SetPower(PE::PEMath::GetRandom(80, 100));
    else if (distance > 500)
        this->tank->SetPower(PE::PEMath::GetRandom(60, 100));
    else if (distance > 300)
        this->tank->SetPower(PE::PEMath::GetRandom(30, 80));
    else if (distance > 100)
        this->tank->SetPower(PE::PEMath::GetRandom(30, 60));
    this->lastEvaluation = AI_PreviousHitEvaluation_Unknown;
}

void AI::normalizeAngle()
{
    if (this->targetAngle < 0)
        this->targetAngle = 0;
    if (this->targetAngle > PE_PI_RAD_CNV)
        this->targetAngle = PE_PI_RAD_CNV;

    if (this->enemySide == AI_Left && this->targetAngle < PE_PI_RAD_CNV / 2)
        this->targetAngle = (PE_PI_RAD_CNV / 2) + 0.1;
    else if (this->enemySide == AI_Right && this->targetAngle > PE_PI_RAD_CNV / 2)
        this->targetAngle = (PE_PI_RAD_CNV / 2) - 0.1;
}

bool AI::fallenBefore(double X)
{
    if (this->enemySide == AI_Right)
    {
        return (this->selectedEnemy->Position.X > X);
    } else{
        return (this->selectedEnemy->Position.X < X);
    }
}

bool AI::flewOver(double X)
{
    if (this->enemySide == AI_Right)
    {
        return (this->selectedEnemy->Position.X < X);
    } else{
        return (this->selectedEnemy->Position.X > X);
    }
}

bool AI::flewAway(double X)
{
    if (X < 0)
        return true;
    if (X > Game::CurrentGame->MapWidth)
        return true;
    return false;
}

void AI::evaluateFire()
{
    PE::Vector lastHit = this->tank->LastHit;
    double distance_last = lastHit.DistanceTo(this->selectedEnemy->Position);
    double distance_prev = this->previousHit.DistanceTo(this->selectedEnemy->Position);
    double distance_self = lastHit.DistanceTo(this->tank->Position);
    if (lastHit != PE::Vector::Zero)
        this->lastDistanceFromSelf = distance_self;
    this->untracedCounter++;
    if (this->untracedCounter > 4)
    {
        debug_log("more than 5 times didn't suceed, sending tracers");
        this->trace();
        return;
    }
    if (this->tank->LastHit == PE::Vector::Zero)
    {
        if (this->unknownDataCounter > 1)
        {
            debug_log("too many times got unknown hit results, trying random angle");
            this->improveAngle(0.6);
            this->changeState(AI_State_Waiting_Angle);
            return;
        }
        debug_log("unable to evaluate last hit success - not enough data");
        this->unknownDataCounter++;
        this->Fire();
        return;
    }
    if (this->selectedEnemy->Health + this->selectedEnemy->ShieldPower < this->previousEnemyHP)
    {
        debug_log("enemy has lower HP since last hit, keeping same trajectory");
        this->lastEvaluation = AI_PreviousHitEvaluation_Pefect;
        this->previousEnemyHP = this->selectedEnemy->Health + this->selectedEnemy->ShieldPower;
        this->Fire();
        return;
    }

    if (this->previousHit == PE::Vector(0, 0))
    {
        if (this->unknownDataCounter > 1)
        {
            debug_log("too many times got unknown hit results, trying random angle");
            this->improveAngle(0.6);
            this->changeState(AI_State_Waiting_Angle);
            return;
        }
        this->previousHit = this->tank->LastHit;
        this->unknownDataCounter++;
        debug_log("not enough hits to evaluate which one was better, keeping trajectory");
        this->Fire();
        return;
    }

    this->unknownDataCounter = 0;

    if (distance_last < this->bestDistance)
    {
        debug_log("this is best distance hit we have so far, saving this angle for later");
        this->lastEvaluation = AI_PreviousHitEvaluation_Pefect;
        this->bestAngle = this->targetAngle;
        this->bestPower = this->tank->Power;
        this->bestDistance = distance_last;
    }

    if (this->flewOver(this->tank->LastHit.X) && this->tank->LastHit.Y < this->selectedEnemy->Position.Y && this->tank->LastHit_Velocity.Y < 0)
    {
        debug_log("last hit was when rocket was falling down and enemy was above it, rocket must go higher, adjusting power and angle");
        this->tank->IncreasePower(PE::PEMath::GetRandom(2, 20));
        this->targetPower = this->tank->Power;
        this->improveAngle(0.1);
        this->changeState(AI_State_Waiting_Angle);
        return;
    }

    if (this->flewOver(this->tank->LastHit.X) && this->tank->LastHit.Y > this->selectedEnemy->Position.Y && this->tank->LastHit_Velocity.Y > 0)
    {
        debug_log("last hit was when rocket was above enemy and it was flying up, adjusting power down");
        this->lastEvaluation = AI_PreviousHitEvaluation_Bad;
        this->tank->IncreasePower(PE::PEMath::GetRandom(2, 20) * -1);
        this->targetPower = this->tank->Power;
        this->improveAngle(0.1);
        this->changeState(AI_State_Waiting_Angle);
        return;
    }

    if (this->flewAway(this->tank->LastHit.X) && this->tank->LastHit.Y > this->selectedEnemy->Position.Y)
    {
        debug_log("last hit flew out of map when rocket was above enemy, adjusting power down");
        this->lastEvaluation = AI_PreviousHitEvaluation_Bad;
        this->tank->IncreasePower(PE::PEMath::GetRandom(2, 20) * -1);
        this->Fire();
        return;
    }

    if (distance_last < distance_prev)
    {
        this->previousHit = this->tank->LastHit;
        this->lastEvaluation = AI_PreviousHitEvaluation_Good;
        debug_log("last hit had better distance than one before, this is a good trajectory");
        this->tank->Fire();
        return;
    }

    if (this->fallenBefore(this->tank->LastHit.X) && this->tank->Power < 100 && this->tank->LastHit.Y < this->selectedEnemy->Position.Y && this->tank->LastHit_Velocity.Y < 0)
    {
        debug_log("last hit was when rocket was falling down and enemy was above far away, rocket must go higher, adjusting power up");
        this->lastEvaluation = AI_PreviousHitEvaluation_Bad;
        this->tank->IncreasePower(PE::PEMath::GetRandom(5, 40));
        this->targetPower = this->tank->Power;
        this->changeState(AI_State_Waiting_Angle);
        return;
    }

    if (distance_last == distance_prev)
    {
        this->previousHit = this->tank->LastHit;
        this->targetAngle = this->bestAngle;
        this->improveAngle(0.3);
        debug_log("last hit had same distance than one before, trying better angle - " + QString::number(this->targetAngle));
        this->lastEvaluation = AI_PreviousHitEvaluation_Average;
        return;
    }

    this->previousHit = this->tank->LastHit;
    this->targetAngle = this->bestAngle;
    /*this->improveAngle(0.2);
    this->improvePower(10);
    debug_log("last hit had worse distance than one before, trying better angle - " + QString::number(this->targetAngle)); */
    debug_log("last hit had worse distance than one before, sending tracers");
    this->lastEvaluation = AI_PreviousHitEvaluation_Horrible;
    this->trace();
}

void AI::postEvaluateFire()
{
    switch (this->prevEvaluation)
    {
        case AI_PreviousHitEvaluation_Bad:
        case AI_PreviousHitEvaluation_Horrible:
            if (this->lastEvaluation == AI_PreviousHitEvaluation_Good || this->lastEvaluation == AI_PreviousHitEvaluation_Pefect)
            {
                // Previous evaluation was bad, but now it's much better, which means we improved,
                // let's try to add half of the change we did last time to check if it improves further
                debug_log("last change is evaluated as improvement, reapplying same change");
                this->changeAngle(this->targetAngle + (this->lastAngleChange / 2));
                this->changePower(this->targetPower + (this->lastPowerChange / 2));
                return;
            }
            break;
        case AI_PreviousHitEvaluation_Pefect:
        case AI_PreviousHitEvaluation_Good:
            if (this->lastEvaluation == AI_PreviousHitEvaluation_Bad || this->lastEvaluation == AI_PreviousHitEvaluation_Horrible)
            {
                debug_log("last change is evaluated as worse, reverting change");
                this->changeAngle(this->bestAngle);
                this->changePower(this->bestPower);
                return;
            }
            break;
        default:
            return;
    }
}

bool AI::hasWeapon(int id)
{
    return this->tank->GetPlayer()->ItemList[id] > 0;
}

void AI::evaluateShield()
{
    if (this->tank->ShieldPower > 0)
        return;
    if (this->tank->GetPlayer()->ItemList[INVENTORY_HEAVY_SHIELD] > 0)
        this->tank->DeployShield(ShieldHeavy);
    else if (this->tank->GetPlayer()->ItemList[INVENTORY_SHIELD] > 0)
        this->tank->DeployShield(ShieldMini);
}

void AI::evaluateRepair()
{
    if (this->tank->Health < 80 && this->tank->GetPlayer()->ItemList[INVENTORY_REPAIR_KIT] > 0)
        this->tank->Repair();
}

void AI::evaluateWeapon()
{
    int current_weapon = this->tank->SelectedWeapon->GetWeaponType();
    if (current_weapon != 0 && this->tank->SelectedWeapon->Ammo <= 0)
    {
        // Current weapon is out of ammo, we have to change
        debug_log("current weapon is out of ammo, finding alternative weapon");
        this->changeWeapon();
        debug_log("alternative weapon chosen: " + Shop::DefaultShop->ItemString(this->tank->SelectedWeapon->GetWeaponType()));
        return;
    }

    // This leads to problems as there is lots of post processing later which we need to skip in case we decided to use sonic bomb
    // if (this->evaluateSonic())
    //    return;

    if (!this->firstShot && this->bestDistance < 400 && this->hasWeapon(WEAPON_NUKE))
    {
        this->tank->SwitchWeapon(WEAPON_NUKE);
        return;
    } else if (!this->firstShot && this->bestDistance < 120 && this->hasWeapon(WEAPON_MINI_NUKE))
    {
        this->tank->SwitchWeapon(WEAPON_MINI_NUKE);
        return;
    } else if (current_weapon == 0 && this->hasWeapon(WEAPON_BIG_CANON))
    {
        this->tank->SwitchWeapon(WEAPON_BIG_CANON);
        return;
    } else if (current_weapon == 0 && this->hasWeapon(WEAPON_TRIPLE_CANON))
    {
        this->tank->SwitchWeapon(WEAPON_TRIPLE_CANON);
        return;
    }
}

void AI::changeWeapon()
{
    if (this->hasWeapon(WEAPON_TRIPLE_CANON))
    {
        this->tank->SwitchWeapon(WEAPON_TRIPLE_CANON);
        return;
    }
    if (this->hasWeapon(WEAPON_BIG_CANON))
    {
        this->tank->SwitchWeapon(WEAPON_BIG_CANON);
        return;
    }
    this->tank->SwitchWeapon(WEAPON_CANON);
}

void AI::changeAngle(double new_angle)
{
    this->lastAngleChange = new_angle - this->targetAngle;
    this->targetAngle = new_angle;
    this->changeState(AI_State_Waiting_Angle);
}

void AI::changePower(double new_power)
{
    if (new_power < 10)
        new_power = 10;
    if (new_power > 100)
        new_power = 100;
    this->lastPowerChange = new_power - this->targetPower;
    this->targetPower = new_power;
    this->tank->SetPower(new_power);
}

void AI::increasePower(double p)
{
    this->changePower(this->tank->Power += p);
}

void AI::trace()
{
    this->untracedCounter = 0;
    this->changeState(AI_State_Trace_Wait);
    // Fire couple of tracers
    int trace_count = 4;
    double original_angle = this->targetAngle;
    double current_angle = original_angle - 0.2;
    double current_power = this->tank->Power;
    while (trace_count-- > 0)
    {
        // Get spawn point
        PE::Vector source = tank->GetCanonRoot(this->tank->Position);
        PE::Vector spawn_point = source;
        PE::Vector position = source;
        // angle math
        double radians = current_angle * PE_PI_RAD_CNV;
        position.X += (2 * std::cos(radians));
        position.Y += (2 * std::sin(radians));
        spawn_point.X += (1 * std::cos(radians));
        spawn_point.Y += (1 * std::sin(radians));
        PE::Vector force = (position - source) * (current_power / 15);
        AITracer *t = new AITracer(spawn_point);
        t->SetForce(force);
        t->Owner = this->tank;
        t->Angle = current_angle;
        t->Power = current_power;
        t->Destroy(8000);
        this->tracers.append(t);
        Game::CurrentGame->GetWorld()->RegisterActor(t);
        current_angle += 0.1;
    }
    Game::Tracing = true;
}

void AI::traceEval()
{
    foreach (AITracer *t, this->tracers)
    {
        if (!t->Finished)
            return;
    }

    Game::Tracing = false;

    double original_best_dist = this->bestDistance;
    double original_best_angle = this->bestAngle;
    double original_best_power = this->bestPower;
    double original_best_dist_self = this->bestDistance_DistanceToSelf;

    // Evaluate all tracers
    int tracer_id = 0;
    foreach (AITracer *t, this->tracers)
    {
        tracer_id++;
        if (t->target != nullptr)
        {
            // We hit something!
            if (this->selectedEnemy != t->target)
            {
                // it's not our enemy though
            } else
            {
                this->bestAngle = t->Angle;
                this->bestPower = t->Power;
                this->bestDistance = t->PositionFinal.DistanceTo(this->selectedEnemy->Position);
                this->bestDistance_DistanceToSelf = t->PositionFinal.DistanceTo(this->tank->Position);
                this->targetAngle = this->bestAngle;
                this->targetPower = this->bestPower;
                debug_log("Tracer found enemy: " + t->target->PlayerName);
                this->changeState(AI_State_Waiting_Angle);
                this->tracers.clear();
                return;
            }
        }
        double distance = t->PositionFinal.DistanceTo(this->selectedEnemy->Position);
        double distance_to_self = t->PositionFinal.DistanceTo(this->tank->Position);
        if (distance_to_self < 20)
        {
            debug_log("Tracer " + QString::number(tracer_id) + " is dangeously close to use, skipping it: " + QString::number(distance_to_self));
            continue;
        }
        debug_log("Tracer " + QString::number(tracer_id) + " found distance to target enemy: " + QString::number(distance));
        if (distance < this->bestDistance)
        {
            debug_log("Tracer " + QString::number(tracer_id) + " found better distance to target enemy: " + QString::number(distance));
            this->bestDistance = distance;
            this->bestAngle = t->Angle;
            this->bestPower = t->Power;
        }
    }

    debug_log("Tracer results: best distance " + QString::number(this->bestDistance) + " best angle " + QString::number(this->bestAngle) + " best power " + QString::number(this->bestPower));

    if (this->bestDistance == original_best_dist)
    {
        debug_log("Tracers didn't find any better state than we already have");
        // Reset back to previous values to avoid doing something stupid
        this->bestAngle = original_best_angle;
        this->bestPower = original_best_power;
        this->bestDistance_DistanceToSelf = original_best_dist_self;
        if (this->bestDistance >= AI_WORST_DISTANCE)
        {
            this->getTargetAngle();
            this->targetPower = this->tank->GetMaxPower();
            this->improvePower(20);
        } else
        {
            this->targetAngle = this->bestAngle;
            this->targetPower = this->bestPower;
        }
        this->changeState(AI_State_Waiting_Angle);
    } else
    {
        this->targetAngle = this->bestAngle;
        this->targetPower = this->bestPower;
        this->changeState(AI_State_Waiting_Angle);
    }
    this->tracers.clear();
}

bool AI::evaluateSonic()
{
    // There is too much terrain close to us
    if (this->lastDistanceFromSelf < 80 && this->hasWeapon(WEAPON_HEAVY_SONIC_BOMB))
    {
        debug_log("last hit distance from myself is less than 80 and I have heavy sonic bomb, let's use it");
        this->tank->SwitchWeapon(WEAPON_HEAVY_SONIC_BOMB);
        this->changeState(AI_State_Obstructed);
        return true;
    }
    if (this->lastDistanceFromSelf < 60 && this->hasWeapon(WEAPON_SONIC_BOMB))
    {
        debug_log("last hit distance from myself is less than 60 and I have sonic bomb, let's use it");
        this->tank->SwitchWeapon(WEAPON_SONIC_BOMB);
        this->changeState(AI_State_Obstructed);
        return true;
    }
    return false;
}

void AI::changeState(AI::AI_State s)
{
    // If there is state change print it to logs
    if (this->state != s)
        debug_log("AI state: " + AI::AIStateToString(this->state) + " >> " + AI::AIStateToString(s));

    // Change current state
    this->state = s;

    if (s == AI_State_Obstructed)
    {
        // Obstructed - let's send tracers next time
        this->untracedCounter = 999999;
    }
}

void AI::print_debug_stats()
{
    this->debug_log("Best angle: " + QString::number(this->bestAngle) +
                    " best power: " + QString::number(this->bestPower) +
                    " target angle: " + QString::number(this->targetAngle) +
                    " target power: " + QString::number(this->targetPower));
}

void AI::debug_log(const QString &text)
{
    if (AITracer::Debug)
        Console::Append("DEBUG " + this->tank->PlayerName + ": " + text);
}

AI_HitMetrics::AI_HitMetrics(double a, double p, const PE::Vector& pos)
{
    this->Power = p;
    this->Source = pos;
    this->Angle = a;
}
