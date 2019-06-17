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
        static int ActiveProjectiles;
        Projectile(const PE::Vector &position);
        ~Projectile() override;
        void Event_OnCollision(PE::Collider *collider) override;
        void Event_OnImpact(const PE::Vector &v) override;
        void Event_Destroyed() override;
        void SetForce(const PE::Vector &force);
        PE::Collectable_SmartPtr<TankBase> Owner;
    protected:
        virtual void on_Terrain(PE::Collider *collider);
        virtual void on_Vehicle(TankBase *tank);
};

#endif // PROJECTILE_H
