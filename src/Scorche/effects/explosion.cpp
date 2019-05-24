//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "explosion.h"
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/camera.h>
#include <PixelEngine/terrain.h>
#include <PixelEngine/world.h>
#include <cmath>
#include "rocks.h"
#include "../tanks/tankbase.h"
#include "../game.h"

Explosion::Explosion(TankBase *p, double size)
{
    this->owner = p;
    this->maxSize = size;
    this->untouchedTanks = TankBase::Players;
}

void Explosion::Update(qint64 time)
{
    (void)time;
    if (this->currentSize < this->maxSize)
    {
        this->currentSize += 1;
        this->destroyTerrain();
        Game::CurrentGame->Terrain->LastMovementUpdate = time;
        this->RedrawNeeded = true;
        // Fire a random rock out some direction, just for fun
        Rocks *r = new Rocks();
        r->Position = this->Position;
        r->RandomForce();
        Game::CurrentGame->GetWorld()->RegisterActor(r, 1);
        r->Destroy(2000);
    } else
    {
        this->Destroy();
    }
}

void Explosion::Render(PE::Renderer *r, PE::Camera *c)
{
    // Get position to render on
    PE::Vector position = c->ProjectedPosition(this->Position);
    int shift = static_cast<int>(this->currentSize / 2);
    r->DrawEllipse(position.X2int() - shift, position.Y2int() - shift, static_cast<int>(this->currentSize), static_cast<int>(this->currentSize), QColor("red"), this->currentSize);
}

void Explosion::destroyTerrain()
{
    int x = this->Position.X2int();
    int y = this->Position.Y2int();

    // Get all points of circle

    double angle = 0;
    int old_x = 0;
    int old_y = 0;

    while (angle < (PE_PI_RAD_CNV * 4))
    {
        int circle_x = x + static_cast<int>(this->currentSize * std::cos(angle));
        int circle_y = y + static_cast<int>(this->currentSize * std::sin(angle));
        angle = angle + (0.3 / this->currentSize);
        // Optimization
        if (circle_x == old_x && circle_y == old_y)
            continue;

        old_x = circle_x;
        old_y = circle_y;

        // These functions are expensive
        Game::CurrentGame->Terrain->DestroyPixel(circle_x, circle_y);
        PE::Vector p(static_cast<double>(circle_x), static_cast<double>(circle_y));
        foreach (TankBase *v, this->untouchedTanks)
        {
            if (v->CheckCollision(p))
            {
                v->TakeDamage(this->owner, this->Damage / this->currentSize);
                this->untouchedTanks.removeOne(v);
            }
        }
    }
    Game::CurrentGame->Terrain->RefreshPixmap();
}