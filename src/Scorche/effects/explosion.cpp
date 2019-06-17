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

int Explosion::ExplosionCounter = 0;

Explosion::Explosion(TankBase *p, double size)
{
    this->owner = p;
    this->maxSize = size;
    this->untouchedTanks = TankBase::Players;
    Explosion::ExplosionCounter++;
}

Explosion::~Explosion()
{
    Explosion::ExplosionCounter--;
    if (Explosion::ExplosionCounter == 0)
        TankBase::RotateIfPeace();
}

void Explosion::Update(qint64 time)
{
    (void)time;
    if (this->currentSize < this->maxSize)
    {
        // Prevent any dynamic terrain updates during the explosion - this leads to issues and CPU overload
        Game::CurrentGame->TerrainNeedsUpdate = false;
        this->currentSize += 1;
        this->destroyTerrain();
        Game::CurrentGame->Terrain->LastMovementUpdate = time;
        this->RedrawNeeded = true;
        if (Game::ExplosionEffects && this->FallingRocks)
        {
            // Fire a random rock out some direction, just for fun
            Rocks *r = new Rocks();
            r->Position = this->Position;
            r->RandomForce();
            Game::CurrentGame->GetWorld()->RegisterActor(r, 1);
            r->Destroy(1000);
        }
    } else
    {
        this->Destroy();
        Game::CurrentGame->TerrainNeedsUpdate = true;
    }
}

static int normalize(int i, int min)
{
    if (i > 255)
        return 255;
    if (i < min)
        return min;
    if (i < 0)
        return 0;

    return i;
}

void Explosion::Render(PE::Renderer *r, PE::Camera *c)
{
    // Get position to render on
    PE::Vector position = c->ProjectedPosition(this->Position);
    int shift;
    QColor color;
    if (this->Sonic)
    {
        color = QColor(0, 255, 0);
        int size = 1;
        while ((size / 2) <= static_cast<int>(this->currentSize))
        {
            shift = size / 2;
            r->DrawEllipse(position.X2int() - shift, position.Y2int() - shift, size, size, color, 1);
            size += 3;
        }
    } else
    {
        shift = static_cast<int>(this->currentSize / 2);
        if (this->TerrainCreating)
        {
            color = Qt::yellow;
        }
        else
        {
            int color_shift = static_cast<int>(this->currentSize * 2);
            int red = 255 - color_shift;
            int gr = 100 - color_shift * 4;
            int bl = 100 - color_shift * 2;
            color = QColor(normalize(red, 80), normalize(gr, 50), normalize(bl, 50));
        }
        r->DrawEllipse(position.X2int() - shift, position.Y2int() - shift, static_cast<int>(this->currentSize), static_cast<int>(this->currentSize), color, this->currentSize);
    }
}

void Explosion::destroyTerrain()
{
    int x = this->Position.X2int();
    int y = this->Position.Y2int();

    // Get all points of circle

    double angle = 0;
    int old_x = 0;
    int old_y = 0;

    int pixel_id = 1;
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

        if (this->TerrainCreating)
        {
            Game::CurrentGame->Terrain->CreatePixel(circle_x, circle_y);
        } else
        {
            Game::CurrentGame->Terrain->DestroyPixel(circle_x, circle_y);
            if (this->Damage > 0)
            {
                // These functions are expensive - only check them for every 4th pixel
                if (pixel_id > 4)
                {
                    pixel_id = 0;
                    PE::Vector p(circle_x, circle_y);
                    foreach (TankBase *v, this->untouchedTanks)
                    {
                        if (v->CheckCollision(p))
                        {
                            v->TakeDamage(this->owner, (this->Damage * this->DistanceDamageRatio) / this->currentSize);
                            this->untouchedTanks.removeOne(v);
                            // There seems to be some kind of bug in Qt? There are random crashes happening from this point
                            // I suspect that this change of container makes them
                            break;
                        }
                    }
                }
            }
        }
        pixel_id++;
    }
    Game::CurrentGame->Terrain->RefreshPixmap();
}
