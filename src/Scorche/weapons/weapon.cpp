//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "../shop.h"
#include "weapon.h"
#include "../playerinfo.h"
#include "../tanks/tankbase.h"

Weapon::Weapon(TankBase *owner)
{
    this->Owner = owner;
}

Weapon::~Weapon()
{

}

QString Weapon::GetName()
{
    return Shop::DefaultShop->ItemString(this->GetWeaponType());
}

bool Weapon::reduceAmmo()
{
    if (this->Ammo <= 0)
        return false;

    this->Owner->GetPlayer()->ItemList[this->GetWeaponType()]--;
    this->Ammo = this->Owner->GetPlayer()->ItemList[this->GetWeaponType()];
    return true;
}
