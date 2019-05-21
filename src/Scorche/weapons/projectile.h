//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <PixelEngine/actor.h>
#include <PixelEngine/vector.h>
#include <PixelEngine/GC/collectable_smartptr.h>

class TankBase;

class Projectile : public PE::Actor
{
    public:
        Projectile(const PE::Vector &position);
        void Event_OnCollision(PE::Collider *collider) override;
        PE::Collectable_SmartPtr<TankBase> Owner;
};

#endif // PROJECTILE_H
