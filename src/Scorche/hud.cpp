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
#include "effects/explosion.h"
#include <PixelEngine/Graphics/renderer.h>

HUD::HUD()
{

}

void HUD::Update(qint64 time)
{
    this->lastRefresh = time;
}

void HUD::Render(PE::Renderer *r, PE::Camera *c)
{
    // HUD is using absolute position
    (void)c;

    r->DrawRect(0, 0, r->GetWidth(), 20, 2, QColor("white"), true);
    r->DrawRect(0, 0, r->GetWidth(), 20, 2, QColor("black"));

    if (Game::CurrentGame->WarmingTanks > 0)
    {
        r->DrawText(500, 6, "Warm up!!", Qt::darkRed);
    } else if (TankBase::ControlsFrozen)
    {
        r->DrawText(600, 6, "Waiting for current round to finish", Qt::darkRed);
    }

    if (!TankBase::PlayerTank)
        return;
    r->DrawText(10, 6, "HP: " + QString::number(TankBase::PlayerTank->Health), QColor("black"));
    r->DrawText(80, 6, "Angle: " + QString::number(TankBase::PlayerTank->GetCanonAngleDegree()), QColor("black"));
    r->DrawText(160, 6, "Power: " + QString::number(TankBase::PlayerTank->Power), QColor("black"));
    r->DrawText(230, 6, "Weapon: " + TankBase::PlayerTank->SelectedWeapon->GetName(), QColor("black"));
    if (TankBase::PlayerTank->SelectedWeapon->GetWeaponType() != 0)
        r->DrawText(340, 6, "Ammo: " + QString::number(TankBase::PlayerTank->SelectedWeapon->Ammo), QColor("black"));
    r->DrawText(400, 6, "Cash: $" + QString::number(TankBase::PlayerTank->GetPlayer()->Cash), QColor("black"));
    TankBase *currentPlayer = TankBase::GetActivePlayer();
    if (currentPlayer != nullptr)
        r->DrawText(500, 6, "Playing: " + currentPlayer->PlayerName, QColor("black"));
    if (this->lastRefresh - Console::LastMessageTime < 8000)
        r->DrawText(800, 6, Console::LastMessage, QColor("black"));

    if (Game::CurrentGame->IsPaused)
    {
        int x = r->GetWidth() / 2 - 150;
        int y = r->GetHeight() / 2;
        int width = 300;
        int height = 80;
        r->DrawRect(x, y, width, height, 2, QColor("white"), true);
        r->DrawRect(x, y, width, height, 2, QColor("black"), false);
        r->DrawText(x + (width / 2) - 40, y + height - 30, "Paused", QColor("black"), 20);
    }

    if (Explosion::ExplosionCounter == 0 && Game::CurrentGame->IsFinished)
    {
        // Game over
        int x = r->GetWidth() / 2 - 300;
        int y = r->GetHeight() / 2 - 400;
        int width = 600;
        int height = 600;
        r->DrawRect(x, y, width, height, 2, QColor("white"), true);
        r->DrawRect(x, y, width, height, 2, QColor("black"), false);
        r->DrawText(x + (width / 2) - 80, y + height - 20, "Round finished", QColor("black"), 20);

        int current_Y = y + height - 40;
        r->DrawText(x + (width / 2) - 40, current_Y, "Scores:", QColor("black"), 12);
        foreach (TankBase *t, TankBase::Players)
        {
            current_Y -= 20;
            r->DrawText(x + 20, current_Y, t->GetPlayer()->PlayerName + " total kills: " + QString::number(t->GetPlayer()->Kills_Total)
                        + " total deaths: " + QString::number(t->GetPlayer()->Deaths_Total) +  " cash: $"
                        + QString::number(t->GetPlayer()->Cash) + " score: " + QString::number(t->GetPlayer()->Score), t->GetPlayer()->Color, 12);
        }

        r->DrawText(x + (width / 2) - 100, y + 40, "Press space to go to inventory screen", QColor("black"), 10);
        r->DrawText(x + (width / 2) - 48, y + 20, "Press N to play next game", QColor("black"), 10);
    }
}

void HUD::Event_KeyPress(int key)
{
    if (key == Qt::Key_P)
        Game::CurrentGame->IsPaused = !Game::CurrentGame->IsPaused;
    this->RedrawNeeded = true;
}
