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
#include "../game.h"

Explosion::Explosion(TankBase *p, double size) : Generic(p)
{
    this->maxSize = size;
}

void Explosion::Update(qint64 time)
{
    (void)time;
    if (this->currentSize < this->maxSize)
    {
        this->currentSize += 1;
        this->RedrawNeeded = true;
    } else
    {
        PE::Vector p = this->Position;
        Game::CurrentGame->Terrain->DestroyPixel(p);
        p.X += 1;
        Game::CurrentGame->Terrain->DestroyPixel(p);
        p.Y += 1;
        Game::CurrentGame->Terrain->DestroyPixel(p);
        p.X -= 2;
        Game::CurrentGame->Terrain->DestroyPixel(p);
        p.Y -= 2;
        Game::CurrentGame->Terrain->RefreshPixmap();
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
