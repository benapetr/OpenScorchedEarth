//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "cannon.h"
#include "../projectiles/smallrocket.h"
#include "../game.h"
#include "../tanks/tankbase.h"
#include <cmath>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/world.h>

Cannon::Cannon(TankBase *pawn) : Weapon(pawn)
{
    this->Ammo = -1;
}

Weapon::WeaponType Cannon::GetWeaponType()
{
    return WeaponType_SmallRocket;
}

QString Cannon::GetName()
{
    return "Cannon";
}

void Cannon::Fire(const PE::Vector &source, double angle, double power)
{
    PE::Vector position = source;

    // angle math
    double radians = angle * PE_PI_RAD_CNV;
    position.X += (2 * std::cos(radians));
    position.Y += (2 * std::sin(radians));

    PE::Vector force = (position - source) * (power / 10);

    // Boost the Y
    force.Y *= 1.2;

    // Instantiate a projectile in front of cannon
    PE::Collectable_SmartPtr<SmallRocket> rocket = new SmallRocket(position);
    rocket->Owner = this->Owner;
    rocket->DestroyAfter = Game::CurrentGame->GetWorld()->GetTime() + 20000;
    rocket->RigidBody->AddForce(force);
    Game::CurrentGame->GetWorld()->RegisterActor(rocket);
}
