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
#include "console.h"
#include <cmath>
#include "game.h"
#include "tanks/tankbase.h"
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
                this->state = AI_State_Waiting_Angle;
                this->getTargetAngle();
                debug_log("new target angle calculated: " + QString::number(this->targetAngle));
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
            this->evaluateFire();
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
    this->targetAngle += rnd;
    this->normalizeAngle();
    while (old_angle == this->targetAngle)
        this->improveAngle(max);
}

void AI::improvePower(double max)
{
    this->tank->IncreasePower(PE::PEMath::GetRandom(-1 * max, 2 * max));
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
        this->bestAngle = this->targetAngle;
        this->bestDistance = distance_last;
    }

    if (this->flewOver(this->tank->LastHit.X) && this->tank->LastHit.Y < this->selectedEnemy->Position.Y && this->tank->LastHit_Velocity.Y < 0)
    {
        debug_log("last hit was when rocket was falling down and enemy was above it, rocket must go higher, adjusting power and angle");
        this->tank->IncreasePower(PE::PEMath::GetRandom(2, 20));
        this->improveAngle(0.2);
        this->state = AI_State_Waiting_Angle;
        return;
    }

    if (this->flewOver(this->tank->LastHit.X) && this->tank->LastHit.Y > this->selectedEnemy->Position.Y && this->tank->LastHit_Velocity.Y > 0)
    {
        debug_log("last hit was when rocket was above enemy and it was flying up, adjusting power down");
        this->tank->IncreasePower(PE::PEMath::GetRandom(2, 20) * -1);
        this->Fire();
        return;
    }

    if (this->flewAway(this->tank->LastHit.X) && this->tank->LastHit.Y > this->selectedEnemy->Position.Y)
    {
        debug_log("last hit flew out of map when rocket was above enemy, adjusting power down");
        this->tank->IncreasePower(PE::PEMath::GetRandom(2, 20) * -1);
        this->Fire();
        return;
    }

    if (distance_last < distance_prev)
    {
        this->previousHit = this->tank->LastHit;
        debug_log("last hit had better distance than one before, this is a good trajectory");
        this->tank->Fire();
        return;
    }

    if (this->fallenBefore(this->tank->LastHit.X) && this->tank->Power < 100 && this->tank->LastHit.Y < this->selectedEnemy->Position.Y && this->tank->LastHit_Velocity.Y < 0)
    {
        debug_log("last hit was when rocket was falling down and enemy was above far away, rocket must go higher, adjusting power up");
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
        this->state = AI_State_Waiting_Angle;
        return;
    }

    this->previousHit = this->tank->LastHit;
    this->targetAngle = this->bestAngle;
    this->improveAngle(0.1);
    this->improvePower(10);
    this->state = AI_State_Waiting_Angle;
    debug_log("last hit had much worse distance than one before, trying better angle - " + QString::number(this->targetAngle));
}

void AI::debug_log(const QString &text)
{
    Console::Append("DEBUG " + this->tank->PlayerName + ": " + text);
}