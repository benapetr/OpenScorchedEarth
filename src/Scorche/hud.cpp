//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "hud.h"
#include "tankbase.h"
#include <PixelEngine/renderer.h>

HUD::HUD()
{

}

void HUD::Render(PE::Renderer *r, PE::Camera *c)
{
    // HUD is using absolute position
    (void)c;

    r->DrawRect(0, 0, r->GetWidth(), 20, 2, QColor("white"), true);
    r->DrawRect(0, 0, r->GetWidth(), 20, 2, QColor("black"));
    if (!TankBase::PlayerTank)
        return;
    r->DrawText(10, 6, "HP: " + QString::number(TankBase::PlayerTank->Health), QColor("black"));
    r->DrawText(60, 6, "Angle: " + QString::number(TankBase::PlayerTank->GetCanonAngle()), QColor("black"));
}
