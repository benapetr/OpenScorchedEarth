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
    PE::Vector spawn_position = source;

    // angle math
    double radians = angle * PE_PI_RAD_CNV;
    position.X += (2 * std::cos(radians));
    position.Y += (2 * std::sin(radians));
    spawn_position.X += (1 * std::cos(radians));
    spawn_position.Y += (1 * std::sin(radians));

    PE::Vector force = (position - source) * (power / 15);

    // Instantiate a projectile in front of cannon
    PE::Collectable_SmartPtr<SmallRocket> rocket = new SmallRocket(spawn_position);
    rocket->Owner = this->Owner;
    rocket->Destroy(20000);
    rocket->RigidBody->AddForce(force);
    Game::CurrentGame->GetWorld()->RegisterActor(rocket);
}
