//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "../effects/explosion.h"
#include "../game.h"
#include "demotank.h"
#include <PixelEngine/world.h>
#include <PixelEngine/camera.h>
#include <PixelEngine/Physics/boxcollider.h>
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/Physics/rigidbody.h>

DemoTank::DemoTank(double x, double y, PlayerInfo *player) : TankBase (x, y, player)
{
    this->AddChildren(new PE::BoxCollider(0, 0, this->width, this->height, this));
    this->AddChildren(new PE::BoxCollider(5, 10, 10, 6, this));

    this->SetPosition(this->Position);
}

void DemoTank::Render(PE::Renderer *r, PE::Camera *c)
{
    if (!this->IsSpawned)
        return;
    if (!this->IsAlive())
        return;
    // Get position to render on
    PE::Vector position = c->ProjectedPosition(this->Position);
    // Draw a demo tank
    r->DrawRect(static_cast<int>(position.X), static_cast<int>(position.Y), this->width, this->height, 2, this->tankColor, true);
    r->DrawRect(static_cast<int>(position.X + 5), static_cast<int>(position.Y + 10), 10, 6, 1, this->tankColor, true);
    r->DrawLine(this->GetCanonRoot(position), this->getCanonB(position), 1, this->tankColor);

    TankBase::Render(r, c);
}

void DemoTank::Kill(TankBase *tank)
{
    // Explode
    PE::Collectable_SmartPtr<Explosion> explosion = new Explosion(this, 80);
    explosion->Position = this->Position;
    Game::CurrentGame->GetWorld()->RegisterActor(explosion, 2);

    TankBase::Kill(tank);
}

PE::Vector DemoTank::GetCanonRoot(const PE::Vector &source)
{
    return PE::Vector(source) + PE::Vector(10, 14);
}
