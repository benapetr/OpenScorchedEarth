//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "demotank.h"
#include "renderer.h"
#include "rigidbody.h"

DemoTank::DemoTank()
{
    this->RigidBody = new PE::Rigidbody();
    this->RigidBody->Weight = 0.1;
}

void DemoTank::Render(PE::Renderer *r)
{
    // Draw a demo tank
    r->DrawRect(static_cast<int>(this->Position.X), static_cast<int>(this->Position.Y), 20, 10, 4, QColor("red"));
}
