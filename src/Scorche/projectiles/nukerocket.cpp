//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "nukerocket.h"
#include "../effects/explosion.h"
#include "../game.h"
#include "../tanks/tankbase.h"
#include <PixelEngine/camera.h>
#include <PixelEngine/world.h>
#include <PixelEngine/pemath.h>
#include <PixelEngine/Physics/pixelcollider.h>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/Graphics/renderer.h>

NukeRocket::NukeRocket(const PE::Vector &position) : Projectile (position)
{
    this->AddChildren(new PE::PixelCollider(0, 0));
}

void NukeRocket::Render(PE::Renderer *r, PE::Camera *c)
{
    PE::Vector position = c->ProjectedPosition(this->Position);
    r->DrawEllipse(position.X2int(), position.Y2int(), 6, 6, QColor("black"), 3);
    r->DrawEllipse(position.X2int() + 2, position.Y2int() + 2, 4, 4, QColor("red"), 3);
}

void NukeRocket::on_Terrain(PE::Collider *collider)
{
    (void)collider;
    // Explode
    PE::Collectable_SmartPtr<Explosion> explosion = new Explosion(this->Owner, 260);
    explosion->Damage = 10000;
    explosion->DistanceDamageRatio = 3;
    explosion->FallingRocks = false;
    explosion->Position = this->Position;
    Game::CurrentGame->GetWorld()->RegisterActor(explosion, 2);
}

void NukeRocket::on_Vehicle(TankBase *tank)
{
    tank->TakeDamage(this->Owner, 1000);
}
