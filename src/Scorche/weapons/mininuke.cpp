//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "mininuke.h"
#include "../weaponlist.h"
#include "../tanks/tankbase.h"
#include "../playerinfo.h"
#include "../projectiles/mnrocket.h"
#include "../game.h"
#include <cmath>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/world.h>

MiniNuke::MiniNuke(TankBase *pawn) : Weapon (pawn)
{
    this->Ammo = pawn->GetPlayer()->ItemList[WEAPON_MINI_NUKE];
}

int MiniNuke::GetWeaponType()
{
    return WEAPON_MINI_NUKE;
}

void MiniNuke::Fire(const PE::Vector &source, double angle, double power)
{
    if (this->Ammo <= 0)
        return;

    this->Owner->GetPlayer()->ItemList[WEAPON_MINI_NUKE]--;
    this->Ammo = this->Owner->GetPlayer()->ItemList[WEAPON_MINI_NUKE];

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
    PE::Collectable_SmartPtr<MNRocket> rocket = new MNRocket(spawn_position);
    rocket->Owner = this->Owner;
    rocket->Destroy(8000);
    rocket->RigidBody->AddForce(force);
    Game::CurrentGame->GetWorld()->RegisterActor(rocket);
}

