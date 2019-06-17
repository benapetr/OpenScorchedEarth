//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "tracermini.h"
#include <PixelEngine/camera.h>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/world.h>
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/terrain.h>
#include <PixelEngine/Physics/pixelcollider.h>
#include <QPen>
#include <QPainter>
#include "../effects/explosion.h"
#include "../tanks/tankbase.h"
#include "../game.h"

TracerMini::TracerMini(const PE::Vector &position) : Projectile (position)
{
    this->AddChildren(new PE::PixelCollider(0, 0));
    this->lastPosition = position;
}

void TracerMini::Update(qint64 time)
{
    (void)time;
    // Draw a tracer path to world
    if (this->lastPosition == this->Position)
    {
        Game::CurrentGame->Terrain->DrawPixel(this->Position.X2int(), this->Position.Y2int(), Qt::green);
    } else
    {
        Game::CurrentGame->Terrain->DrawLine(this->lastPosition.X2int(), this->lastPosition.Y2int(), this->Position.X2int(), this->Position.Y2int(), Qt::green);
    }
    Game::CurrentGame->Terrain->RefreshPixmap();
    this->lastPosition = this->Position;
}

void TracerMini::Render(PE::Renderer *r, PE::Camera *c)
{
    PE::Vector position = c->ProjectedPosition(this->Position);
    r->DrawEllipse(position.X2int() - 1, position.Y2int() - 1, 2, 2, QColor("black"), 2);
}

void TracerMini::on_Terrain(PE::Collider *collider)
{
    this->Destroy(2000);
}

void TracerMini::on_Vehicle(TankBase *tank)
{
    this->Destroy(2000);
}
