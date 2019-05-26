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
#include <cmath>
#include <PixelEngine/world.h>
#include <PixelEngine/pemath.h>
#include <PixelEngine/vector.h>

AI::AI(TankBase *t)
{
    this->tank = t;
}

AI::~AI()
{

}

void AI::Process()
{
    if (this->selectedEnemy != nullptr && !this->selectedEnemy->IsAlive())
    {
        debug_log("enemy was killed, looking for a new one");
        this->selectedEnemy = nullptr;
        this->state = AI_State_Undecided;
    }
    switch(this->state)
    {
        case AI_State_Trace_Wait:
            this->traceEval();
            return;
        case AI_State_Undecided:
            debug_log("is undecided, looking for a new enemy");
            if (this->selectedEnemy == nullptr)
                this->selectedEnemy = FindClosestEnemy();

            if (this->selectedEnemy == nullptr)
            {
                debug_log("impossible to find enemy");
                this->state = AI_State_Nothing_To_Do;
                return;
            }
            else
            {
                debug_log("new enemy found: " + this->selectedEnemy->PlayerName);
                this->resetEnemy();
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
                this->tank->ResetCanonAdjust();
                this->Fire();
                return;
            }
            if (std::abs(this->tank->GetCanonAngle() - this->targetAngle) < this->targetTolerance)
            {
                this->tank->ResetCanonAdjust();
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
            this->evaluateFire();
            this->postEvaluateFire();
            break;
    }
}

TankBase *AI::FindClosestEnemy()
{
    double best = 99999;
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
    while(info->Cash > 500)
    {
        // We always need shields
        if (info->ItemList[INVENTORY_SHIELD] < 2)
        {
            Shop::DefaultShop->BuyItem(info, INVENTORY_SHIELD);
        }
        if (info->Cash > 1500)
        {
            Shop::DefaultShop->BuyItem(info, INVENTORY_HEAVY_SHIELD);
        }
        if (info->Cash > 5000)
        {
            Shop::DefaultShop->BuyItem(info, WEAPON_NUKE);
        }
        if (info->Cash > 2000)
        {
            Shop::DefaultShop->BuyItem(info, WEAPON_MINI_NUKE);
        }

        Shop::DefaultShop->BuyItem(info, WEAPON_TRIPLE_CANON);
        Shop::DefaultShop->BuyItem(info, WEAPON_BIG_CANON);
    }
}

QString AI::GetAIModelName()
{
    return "Default";
}

void AI::Fire()
{
    this->state = AI_State_Fired;
    this->tank->Fire();
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
    this->bestDistance = 99999;
    this->previousEnemyHP = this->selectedEnemy->Health;
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
    if (this->tank->LastHit == PE::Vector(0, 0))
    {
        if (this->unknownDataCounter > 1)
        {
            debug_log("too many times got unknown hit results, trying random angle");
            this->improveAngle(0.6);
            this->state = AI_State_Waiting_Angle;
            return;
        }
        debug_log("unable to evaluate last hit success - not enough data");
        this->unknownDataCounter++;
        this->Fire();
        return;
    }
    if (this->selectedEnemy->Health < this->previousEnemyHP)
    {
        debug_log("enemy has lower HP since last hit, keeping same trajectory");
        this->lastEvaluation = AI_PreviousHitEvaluation_Pefect;
        this->previousEnemyHP = this->selectedEnemy->Health;
        this->Fire();
        return;
    }
    PE::Vector lastHit = this->tank->LastHit;
    if (this->previousHit == PE::Vector(0, 0))
    {
        if (this->unknownDataCounter > 1)
        {
            debug_log("too many times got unknown hit results, trying random angle");
            this->improveAngle(0.6);
            this->state = AI_State_Waiting_Angle;
            return;
        }
        this->previousHit = this->tank->LastHit;
        this->unknownDataCounter++;
        debug_log("not enough hits to evaluate which one was better, keeping trajectory");
        this->Fire();
        return;
    }

    double distance_last = lastHit.DistanceTo(this->selectedEnemy->Position);
    double distance_prev = this->previousHit.DistanceTo(this->selectedEnemy->Position);
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
        this->state = AI_State_Waiting_Angle;
        return;
    }

    if (this->flewOver(this->tank->LastHit.X) && this->tank->LastHit.Y > this->selectedEnemy->Position.Y && this->tank->LastHit_Velocity.Y > 0)
    {
        debug_log("last hit was when rocket was above enemy and it was flying up, adjusting power down");
        this->lastEvaluation = AI_PreviousHitEvaluation_Bad;
        this->tank->IncreasePower(PE::PEMath::GetRandom(2, 20) * -1);
        this->Fire();
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
        this->state = AI_State_Waiting_Angle;
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
    this->improveAngle(0.2);
    this->improvePower(10);
    debug_log("last hit had worse distance than one before, trying better angle - " + QString::number(this->targetAngle));
    this->lastEvaluation = AI_PreviousHitEvaluation_Horrible;
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
                this->changeAngle(this->lastAngleChange / 2);
                this->changePower(this->lastPowerChange / 2);
                return;
            }
            break;
        case AI_PreviousHitEvaluation_Pefect:
        case AI_PreviousHitEvaluation_Good:
            if (this->lastEvaluation == AI_PreviousHitEvaluation_Bad || this->lastEvaluation == AI_PreviousHitEvaluation_Horrible)
            {
                debug_log("last change is evaluated as worse, reverting change");
                this->changeAngle(-1 * this->lastAngleChange);
                this->changePower(-1 *this->lastPowerChange);
                return;
            }
            break;
        default:
            return;
    }
}

void AI::changeAngle(double new_angle)
{
    this->lastAngleChange = new_angle - this->targetAngle;
    this->targetAngle = new_angle;
    this->state = AI_State_Waiting_Angle;
}

void AI::changePower(double new_power)
{
    if (new_power < 0)
        new_power = 0;
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
    this->state = AI_State_Trace_Wait;
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
}

void AI::traceEval()
{
    foreach (AITracer *t, this->tracers)
    {
        if (!t->Finished)
            return;
    }

    double original_best_dist = this->bestDistance;

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
                this->targetAngle = this->bestAngle;
                this->targetPower = this->bestPower;
                debug_log("Tracer found enemy: " + t->target->PlayerName);
                this->state = AI_State_Waiting_Angle;
                this->tracers.clear();
                return;
            }
        }
        double distance = t->PositionFinal.DistanceTo(this->selectedEnemy->Position);
        debug_log("Tracer " + QString::number(tracer_id) + " found distance to target enemy: " + QString::number(distance));
        if (distance < this->bestDistance)
        {
            debug_log("Tracer " + QString::number(tracer_id) + " found better distance to target enemy: " + QString::number(distance));
            this->bestDistance = distance;
            this->bestAngle = t->Angle;
            this->bestPower = t->Power;
        }
    }

    if (this->bestAngle == original_best_dist)
    {
        debug_log("Tracers didn't find any better state");
    } else
    {
        this->targetAngle = this->bestAngle;
        this->targetPower = this->bestPower;
        this->state = AI_State_Waiting_Angle;
    }
    this->tracers.clear();
}

void AI::debug_log(const QString &text)
{
    Console::Append("DEBUG " + this->tank->PlayerName + ": " + text);
}
