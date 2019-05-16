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
#include <PixelEngine/boxcollider.h>
#include <PixelEngine/renderer.h>
#include <PixelEngine/rigidbody.h>

DemoTank::DemoTank(double x, double y, const QColor& color)
{
    this->tankColor = color;

    this->Position.X = x;
    this->Position.Y = y;

    this->RigidBody = new PE::Rigidbody();
    this->RigidBody->Weight = 0.1;

    this->AddChildren(new PE::BoxCollider(0, 0, this->width, this->height));
    this->AddChildren(new PE::BoxCollider(5, 10, 10, 6));

    this->SetPosition(this->Position);
}

void DemoTank::Render(PE::Renderer *r)
{
    // Draw a demo tank
    r->DrawRect(static_cast<int>(this->Position.X), static_cast<int>(this->Position.Y), this->width, this->height, 2, this->tankColor, true);
    r->DrawRect(static_cast<int>(this->Position.X + 5), static_cast<int>(this->Position.Y + 10), 10, 6, 1, this->tankColor, true);
    r->DrawLine(this->getCanonRoot(), this->getCanonB(), 1, this->tankColor);
}

void DemoTank::Event_KeyPress(int key)
{

}

void DemoTank::Event_KeyRelease(int key)
{

}

PE::Vector DemoTank::getCanonRoot()
{
    return this->Position + PE::Vector(10, 14);
}
