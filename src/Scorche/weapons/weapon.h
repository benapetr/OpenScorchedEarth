//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef WEAPON_H
#define WEAPON_H

#include <QString>
#include <PixelEngine/GC/collectable_smartptr.h>
#include <PixelEngine/vector.h>

class TankBase;

class Weapon
{
    public:
        enum WeaponType
        {
            WeaponType_SmallRocket,
            WeaponType_Rocket
        };

    public:
        Weapon(TankBase *owner);
        virtual ~Weapon();
        virtual QString GetName()=0;
        virtual WeaponType GetWeaponType()=0;
        virtual void Fire(const PE::Vector &source, double angle, double power)=0;
        PE::Collectable_SmartPtr<TankBase> Owner;
        int Ammo = 0;
};

#endif // WEAPON_H