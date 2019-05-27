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
#include "tanks/tankbase.h"
#include "console.h"
#include "game.h"
#include "playerinfo.h"
#include "weapons/weapon.h"
#include <PixelEngine/Graphics/renderer.h>

HUD::HUD()
{

}

void HUD::Render(PE::Renderer *r, PE::Camera *c)
{
    // HUD is using absolute position
    (void)c;

    r->DrawRect(0, 0, r->GetWidth(), 20, 2, QColor("white"), true);
    r->DrawRect(0, 0, r->GetWidth(), 20, 2, QColor("black"));

    if (TankBase::ControlsFrozen)
    {
        r->DrawText(500, 6, "Waiting for current round to finish", Qt::darkRed);
    }

    if (!TankBase::PlayerTank)
        return;
    r->DrawText(10, 6, "HP: " + QString::number(TankBase::PlayerTank->Health), QColor("black"));
    r->DrawText(80, 6, "Angle: " + QString::number(TankBase::PlayerTank->GetCanonAngleDegree()), QColor("black"));
    r->DrawText(160, 6, "Power: " + QString::number(TankBase::PlayerTank->Power), QColor("black"));
    r->DrawText(230, 6, "Weapon: " + TankBase::PlayerTank->SelectedWeapon->GetName(), QColor("black"));
    if (TankBase::PlayerTank->SelectedWeapon->GetWeaponType() != 0)
        r->DrawText(340, 6, "Ammo: " + QString::number(TankBase::PlayerTank->SelectedWeapon->Ammo), QColor("black"));
    TankBase *currentPlayer = TankBase::GetActivePlayer();
    if (currentPlayer != nullptr)
        r->DrawText(400, 6, "Playing: " + currentPlayer->PlayerName, QColor("black"));
    r->DrawText(300, 880, Console::LastMessage, QColor("black"));

    if (Game::CurrentGame->IsFinished)
    {
        // Game over
        r->DrawRect(r->GetWidth() / 2 - 600, r->GetHeight() / 2 - 400, r->GetWidth() - 400, r->GetHeight() - 300, 2, QColor("white"), true);
        r->DrawRect(r->GetWidth() / 2 - 600, r->GetHeight() / 2 - 400, r->GetWidth() - 400, r->GetHeight() - 300, 2, QColor("black"), false);
        r->DrawText(r->GetWidth() / 2 - 200, r->GetHeight() / 2 + 200, "Round finished", QColor("black"), 20);

        int current_Y = r->GetHeight() / 2 + 180;
        r->DrawText(r->GetWidth() / 2 - 300, current_Y, "Scores:", QColor("black"), 12);
        foreach (TankBase *t, TankBase::Players)
        {
            current_Y -= 20;
            r->DrawText(r->GetWidth() / 2 - 300, current_Y, t->GetPlayer()->PlayerName + " cash: $" + QString::number(t->GetPlayer()->Cash) + " score: " + QString::number(t->GetPlayer()->Score), t->GetPlayer()->Color, 12);
        }

        r->DrawText(r->GetWidth() / 2, r->GetHeight() / 2 - 300, "Press space to go to inventory screen", QColor("black"), 10);
        r->DrawText(r->GetWidth() / 2 + 20, r->GetHeight() / 2 - 320, "Press N to play next game", QColor("black"), 10);
    }
}
