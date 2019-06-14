//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "sonicbomb.h"
#include "../effects/explosion.h"
#include "../game.h"
#include "../tanks/tankbase.h"
#include <PixelEngine/camera.h>
#include <PixelEngine/world.h>
#include <PixelEngine/pemath.h>
#include <PixelEngine/Physics/pixelcollider.h>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/Graphics/renderer.h>

SonicBomb::SonicBomb(bool heavy, const PE::Vector &position) : Projectile (position)
{
    this->IsHeavy = heavy;
    this->AddChildren(new PE::PixelCollider(0, 0));
}

void SonicBomb::Render(PE::Renderer *r, PE::Camera *c)
{
    PE::Vector position = c->ProjectedPosition(this->Position);
    if (this->IsHeavy)
        r->DrawEllipse(position.X2int() - 4, position.Y2int() - 4, 4, 4, QColor(255, 255, 0), 4);
    else
        r->DrawEllipse(position.X2int() - 2, position.Y2int() - 2, 2, 2, QColor(255, 255, 0), 2);
}

void SonicBomb::on_Terrain(PE::Collider *collider)
{
    (void)collider;
    this->explode();
}

void SonicBomb::on_Vehicle(TankBase *tank)
{
    (void)tank;
    this->explode();
}

void SonicBomb::explode()
{
    int size = 60;
    if (this->IsHeavy)
        size = 160;
    PE::Collectable_SmartPtr<Explosion> explosion = new Explosion(this->Owner, size);
    explosion->Damage = 0;
    explosion->DistanceDamageRatio = 0;
    explosion->Sonic = true;
    explosion->FallingRocks = false;
    explosion->Position = this->Position;
    Game::CurrentGame->GetWorld()->RegisterActor(explosion, 2);
}
