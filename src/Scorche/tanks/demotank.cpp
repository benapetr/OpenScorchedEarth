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
#include <PixelEngine/camera.h>
#include <PixelEngine/boxcollider.h>
#include <PixelEngine/renderer.h>
#include <PixelEngine/rigidbody.h>

DemoTank::DemoTank(double x, double y, const QColor& color) : TankBase (x, y, color)
{
    this->AddChildren(new PE::BoxCollider(0, 0, this->width, this->height));
    this->AddChildren(new PE::BoxCollider(5, 10, 10, 6));

    this->SetPosition(this->Position);
}

void DemoTank::Render(PE::Renderer *r, PE::Camera *c)
{
    // Get position to render on
    PE::Vector position = c->ProjectedPosition(this->Position);
    // Draw a demo tank
    r->DrawRect(static_cast<int>(position.X), static_cast<int>(position.Y), this->width, this->height, 2, this->tankColor, true);
    r->DrawRect(static_cast<int>(position.X + 5), static_cast<int>(position.Y + 10), 10, 6, 1, this->tankColor, true);
    r->DrawLine(this->getCanonRoot(position), this->getCanonB(position), 1, this->tankColor);
}

PE::Vector DemoTank::getCanonRoot(const PE::Vector &source)
{
    return PE::Vector(source) + PE::Vector(10, 14);
}