//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "riotcannon.h"
#include "../weaponlist.h"
#include "../tanks/tankbase.h"
#include "../playerinfo.h"
#include "../projectiles/riotbomb.h"
#include "../game.h"
#include <cmath>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/world.h>

RiotCannon::RiotCannon(bool heavy, TankBase *pawn) : Weapon(pawn)
{
    this->IsHeavy = heavy;
    this->Ammo = pawn->GetPlayer()->ItemList[this->GetWeaponType()];
}

int RiotCannon::GetWeaponType()
{
    if (this->IsHeavy)
        return WEAPON_HEAVY_RIOT_BOMB;
    else
        return WEAPON_RIOT_BOMB;
}

void RiotCannon::Fire(const PE::Vector &source, double angle, double power)
{
    if (!this->reduceAmmo())
        return;

    PE::Vector position = source;
    PE::Vector spawn_position = source;

    // angle math
    double radians = angle * PE_PI_RAD_CNV;
    position.X += (2 * std::cos(radians));
    position.Y += (2 * std::sin(radians));
    spawn_position.X += (1 * std::cos(radians));
    spawn_position.Y += (1 * std::sin(radians));

    PE::Vector force = (position - source) * (power / 15);
    PE::Collectable_SmartPtr<RiotBomb> rocket = new RiotBomb(this->IsHeavy, spawn_position);
    rocket->Owner = this->Owner;
    rocket->Destroy(8000);
    rocket->RigidBody->AddForce(force);
    Game::CurrentGame->GetWorld()->RegisterActor(rocket);
}
