//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include <PixelEngine/world.h>
#include <PixelEngine/Physics/boxcollider.h>
#include <PixelEngine/Physics/rigidbody.h>
#include "../tanks/tankbase.h"
#include "../game.h"
#include "projectile.h"

Projectile::Projectile(const PE::Vector &position)
{
    this->Position = position;
    this->RigidBody = new PE::Rigidbody();
    this->RigidBody->Weight = 0.1;
}

void Projectile::Event_OnCollision(PE::Collider *collider)
{
    (void)collider;
    Game::CurrentGame->GetWorld()->DestroyObject(this);
}
