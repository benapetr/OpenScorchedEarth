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
#include <PixelEngine/Graphics/renderer.h>
#include "newgame.h"
#include "../staticassets.h"
#include "../game.h"
#include "../playerinfo.h"

NewGame::NewGame()
{

}

void NewGame::Render(PE::Renderer *r, PE::Camera *c)
{
    (void)c;
    r->DrawText(r->GetWidth() / 2 - 120, r->GetHeight() - 60, "New game", QColor("black"), 40);
    r->DrawText(60, r->GetHeight() - 120, "How many bots you want to play with: " + QString::number(this->Bots), QColor("black"), 20);

    r->DrawBitmap(420, r->GetHeight() - 120, 40, 20, StaticAssets::Instance->RightArrow);
    r->DrawBitmap(462, r->GetHeight() - 120, 40, 20, StaticAssets::Instance->LeftArrow);
    r->DrawText(r->GetWidth() / 2 - 138, r->GetHeight() / 2 - 200, "Press space to open inventory", QColor("black"), 20);
    //r->DrawText(60, r->GetHeight() / 2 + 160, "How many bots you want to play with", QColor("black"), 20);

    // Draw arrow
    //r->DrawBitmap(20, r->GetHeight() / 2 + 100 - (this->SelectedItem * 60), 40, 20, StaticAssets::Instance->LeftArrow);
}

void NewGame::Event_KeyPress(int key)
{
    this->RedrawNeeded = true;
    switch (key)
    {
        case Qt::Key::Key_Space:
        {
            // color hack
            QList<QColor> colors;
            colors << QColor("red")
                   << QColor("green")
                   << QColor(244, 107, 66)
                   << QColor(217, 244, 65)
                   << QColor(157, 65, 244)
                   << QColor(57, 235, 144)
                   << QColor(157, 240, 244)
                   << QColor(255, 0, 244)
                   << QColor(0, 0, 0)
                   << QColor(0, 153, 60);

            // Create a new game
            // Set up player info
            PlayerInfo::Players.append(new PlayerInfo("Player", QColor(0, 0, 160), false));
            int bot_id = 1;
            int botc = this->Bots;
            while(botc-- > 0)
            {
                PlayerInfo::Players.append(new PlayerInfo("Bot " + QString::number(bot_id++), colors[bot_id-1], true));
                // Doesn't work
                //PlayerInfo::Players.append(new PlayerInfo("Bot " + QString::number(bot_id++), PlayerInfo::GetRandomUnusedColor(), true));
            }
            Game::CurrentGame->RequestScene(Scene_Inventory);
        }
            return;
        case Qt::Key::Key_Left:
            if (this->Bots > 1)
                this->Bots--;
            return;

        case Qt::Key::Key_Right:
            if (this->Bots < 64)
                this->Bots++;
            return;
    }
}

void NewGame::SelectUp()
{
    if (this->SelectedItem <= 0)
        return;
    this->SelectedItem--;
}

void NewGame::SelectDown()
{
    if (this->SelectedItem >= 1)
        return;
    this->SelectedItem++;
}
