//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include <PixelEngine/camera.h>
#include <PixelEngine/Physics/rigidbody.h>
#include <PixelEngine/world.h>
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/terrain.h>
#include <PixelEngine/Physics/pixelcollider.h>
#include "smallrocket.h"
#include "../effects/explosion.h"
#include "../tanks/tankbase.h"
#include "../game.h"

SmallRocket::SmallRocket(const PE::Vector &position) : Projectile (position)
{
    this->AddChildren(new PE::PixelCollider(0, 0));
}

void SmallRocket::Render(PE::Renderer *r, PE::Camera *c)
{
    PE::Vector position = c->ProjectedPosition(this->Position);
    r->DrawEllipse(position.X2int(), position.Y2int(), 4, 4, QColor("black"), 2);
}

void SmallRocket::SetForce(const PE::Vector &force)
{
    this->RigidBody->AddForce(force);
}

void SmallRocket::on_Terrain(PE::Collider *collider)
{
    (void)collider;
    // Explode
    PE::Collectable_SmartPtr<Explosion> explosion = new Explosion(this->Owner, 10);
    explosion->Position = this->Position;
    Game::CurrentGame->GetWorld()->RegisterActor(explosion, 2);
}

void SmallRocket::on_Vehicle(TankBase *tank)
{
    // Damage
    tank->TakeDamage(this->Owner, 60);
}
