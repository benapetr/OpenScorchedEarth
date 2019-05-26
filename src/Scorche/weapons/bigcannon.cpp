//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "bigcannon.h"
#include "../weaponlist.h"
#include "../tanks/tankbase.h"
#include "../playerinfo.h"

BigCannon::BigCannon(TankBase *pawn) : Weapon (pawn)
{
    this->Ammo = pawn->GetPlayer()->ItemList[WEAPON_BIG_CANON];
}

Weapon::WeaponType BigCannon::GetWeaponType()
{
    return WeaponType_Rocket;
}

QString BigCannon::GetName()
{
    return "Big cannon";
}

void BigCannon::Fire(const PE::Vector &source, double angle, double power)
{

}
