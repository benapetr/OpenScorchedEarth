//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "terminator.h"
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

Terminator::Terminator(TankBase *t) : AI(t)
{

}

void Terminator::evaluateFire()
{
    this->untracedCounter++;
    AI::evaluateFire();
}

void Terminator::trace()
{
    this->untracedCounter = 0;
    this->state = AI_State_Trace_Wait;
    // Fire couple of tracers
    int trace_count;
    double original_angle = this->targetAngle;
    double current_angle;
    double current_power = 100;
    while (current_power > 40)
    {
        trace_count = 20;
        current_angle = original_angle - 0.4;
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
            current_angle += 0.05;
        }
        current_power -= PE::PEMath::GetRandom(8, 15);
    }
    Game::Tracing = true;
}
