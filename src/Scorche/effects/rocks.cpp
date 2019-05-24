//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "rocks.h"
#include <PixelEngine/camera.h>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/Physics/pixelcollider.h>
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/pemath.h>

Rocks::Rocks()
{
    this->RigidBody = new PE::Rigidbody();
    this->RigidBody->Weight = 0.2;
    this->AddChildren(new PE::PixelCollider(0, 0));
}

void Rocks::Update(qint64 time)
{

}

void Rocks::Render(PE::Renderer *r, PE::Camera *c)
{
    PE::Vector position = c->ProjectedPosition(this->Position);
    r->DrawBitmap(position.X2int(), position.Y2int(), 6, 6, QPixmap(":/textures/terrain/rock.png"));
}

void Rocks::Event_OnImpact(const PE::Vector &v)
{
    this->Destroy();
}

void Rocks::RandomForce()
{
    double force_x = static_cast<double>(PE::PEMath::GetRandom(-300, 300)) / 100;
    double force_y = static_cast<double>(PE::PEMath::GetRandom(-200, 500)) / 100;
    this->RigidBody->AddForce(PE::Vector(force_x, force_y));
}
