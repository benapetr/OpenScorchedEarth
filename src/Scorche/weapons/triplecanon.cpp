//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "triplecanon.h"
#include "../weaponlist.h"
#include "../tanks/tankbase.h"
#include "../playerinfo.h"
#include "../projectiles/smallrocket.h"
#include "../game.h"
#include <cmath>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/world.h>

TripleCanon::TripleCanon(TankBase *pawn) : Weapon(pawn)
{
    this->Ammo = this->Owner->GetPlayer()->ItemList[WEAPON_TRIPLE_CANON];
}

int TripleCanon::GetWeaponType()
{
    return WEAPON_TRIPLE_CANON;
}

void TripleCanon::Fire(const PE::Vector &source, double angle, double power)
{
    if (this->Ammo <= 0)
        return;

    this->Owner->GetPlayer()->ItemList[WEAPON_TRIPLE_CANON]--;
    this->Ammo = this->Owner->GetPlayer()->ItemList[WEAPON_TRIPLE_CANON];

    PE::Vector position1 = source;
    PE::Vector position2 = source;
    PE::Vector position3 = source;
    PE::Vector spawn_position1 = source;
    PE::Vector spawn_position2 = source;
    PE::Vector spawn_position3 = source;

    // angle math
    double radians1 = angle * PE_PI_RAD_CNV;
    position1.X += (2 * std::cos(radians1));
    position1.Y += (2 * std::sin(radians1));
    spawn_position1.X += (1 * std::cos(radians1));
    spawn_position1.Y += (1 * std::sin(radians1));
    PE::Vector force1 = (position1 - source) * (power / 15);

    double radians2 = (angle + 0.02) * PE_PI_RAD_CNV;
    position2.X += (2 * std::cos(radians2));
    position2.Y += (2 * std::sin(radians2));
    spawn_position2.X += (1 * std::cos(radians2));
    spawn_position2.Y += (1 * std::sin(radians2));
    PE::Vector force2 = (position2 - source) * (power / 15);

    double radians3 = (angle - 0.02) * PE_PI_RAD_CNV;
    position3.X += (2 * std::cos(radians3));
    position3.Y += (2 * std::sin(radians3));
    spawn_position3.X += (1 * std::cos(radians3));
    spawn_position3.Y += (1 * std::sin(radians3));
    PE::Vector force3 = (position3 - source) * (power / 15);

    // Instantiate a projectile in front of cannon
    PE::Collectable_SmartPtr<SmallRocket> rocket1 = new SmallRocket(spawn_position1);
    PE::Collectable_SmartPtr<SmallRocket> rocket2 = new SmallRocket(spawn_position2);
    PE::Collectable_SmartPtr<SmallRocket> rocket3 = new SmallRocket(spawn_position3);
    rocket1->Owner = this->Owner;
    rocket2->Owner = this->Owner;
    rocket3->Owner = this->Owner;
    rocket1->Destroy(8000);
    rocket2->Destroy(8000);
    rocket3->Destroy(8000);
    rocket1->RigidBody->AddForce(force1);
    rocket2->RigidBody->AddForce(force2);
    rocket3->RigidBody->AddForce(force3);
    Game::CurrentGame->GetWorld()->RegisterActor(rocket1);
    Game::CurrentGame->GetWorld()->RegisterActor(rocket2);
    Game::CurrentGame->GetWorld()->RegisterActor(rocket3);
}
