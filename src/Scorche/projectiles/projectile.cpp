//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include <PixelEngine/Physics/boxcollider.h>
#include <PixelEngine/Physics/rigidbody.h>
#include "../tanks/tankbase.h"
#include "projectile.h"

Projectile::Projectile(const PE::Vector &position)
{
    this->Position = position;
    this->RigidBody = new PE::Rigidbody();
    this->RigidBody->Weight = 0.1;
}

void Projectile::Event_OnCollision(PE::Collider *collider)
{
    if (collider->GetParent() != nullptr)
    {
        // Check if this is a tank
        TankBase *t = dynamic_cast<TankBase*>(collider->GetParent());
        if (t != nullptr)
            this->on_Vehicle(t);
        else
            this->on_Terrain(collider);
    } else {
        this->on_Terrain(collider);
    }
    this->Destroy();
}

void Projectile::on_Terrain(PE::Collider *collider)
{

}

void Projectile::on_Vehicle(TankBase *tank)
{

}
