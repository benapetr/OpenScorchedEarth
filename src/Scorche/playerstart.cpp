//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "playerstart.h"
#include "tanks/tankbase.h"
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/camera.h>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/Physics/pixelcollider.h>

PlayerStart::PlayerStart(const PE::Vector &position)
{
    this->Position = position;
    this->AddChildren(new PE::PixelCollider(0, 0));
    this->RigidBody = new PE::Rigidbody();
    this->RigidBody->Weight = 1;
}

void PlayerStart::Render(PE::Renderer *r, PE::Camera *c)
{
    //PE::Vector position = c->ProjectedPosition(this->Position);
    //r->DrawEllipse(position.X2int(), position.Y2int(), 3, 3, QColor("blue"), 3);
}

void PlayerStart::Event_OnCollision(PE::Collider *collider)
{
    // Spawn tank
    TankBase *unspawned_tank = TankBase::GetUnspawned();
    if (unspawned_tank != nullptr)
    {
        unspawned_tank->Position = this->Position;
        //unspawned_tank->Position.Y += 10;
        unspawned_tank->RigidBody->GroundCollider = nullptr;
        unspawned_tank->IsSpawned = true;
    }
    this->Destroy();
}
