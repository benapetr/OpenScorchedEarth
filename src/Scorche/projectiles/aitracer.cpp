//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "aitracer.h"
#include "../game.h"
#include "../tanks/tankbase.h"
#include <PixelEngine/camera.h>
#include <PixelEngine/world.h>
#include <PixelEngine/pemath.h>
#include <PixelEngine/Physics/pixelcollider.h>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/Graphics/renderer.h>

bool AITracer::Debug = false;

AITracer::AITracer(const PE::Vector &position) : Projectile (position)
{
    this->AddChildren(new PE::PixelCollider(0, 0));
    Projectile::ActiveProjectiles--;
}

AITracer::~AITracer()
{
    Projectile::ActiveProjectiles++;
}

void AITracer::Render(PE::Renderer *r, PE::Camera *c)
{
    if (!Debug)
        return;
    PE::Vector position = c->ProjectedPosition(this->Position);
    r->DrawEllipse(position.X2int(), position.Y2int(), 3, 3, QColor("red"), 3);
}

void AITracer::Event_Destroyed()
{
    this->Finished = true;
    this->PositionFinal = this->Position;
}

void AITracer::on_Terrain(PE::Collider *collider)
{
    this->PositionFinal = this->Position;
    this->Finished = true;
}

void AITracer::on_Vehicle(TankBase *tank)
{
    this->PositionFinal = this->Position;
    this->target = tank;
    this->Finished = true;
}
