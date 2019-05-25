//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "introscene.h"
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/Graphics/pixmaptools.h>
#include <PixelEngine/resources.h>
#include "../game.h"

IntroScene::IntroScene()
{

}

void IntroScene::Render(PE::Renderer *r, PE::Camera *c)
{
    // HUD is using absolute position
    (void)c;

    r->DrawText(r->GetWidth() / 2 - 200, r->GetHeight() / 2 + 200, "Open Scorched Earth", QColor("black"), 40);
    r->DrawText(r->GetWidth() / 2 - 100, r->GetHeight() / 2 + 160, "Mother of all games", QColor("black"), 20);
    r->DrawText(r->GetWidth() / 2 - 138, r->GetHeight() / 2 - 200, "Press space to start new game", QColor("black"), 20);

    r->DrawText(r->GetWidth() / 2 - 100, r->GetHeight() / 2 - 300, "Created by Petr Bena", QColor("black"), 20);
    //r->DrawBitmap(10, 50, 20, 30, PE::PixmapTools::Rotate(PE::Resources::GetPixmap(":/textures/terrain/arrow.png"), 90));
    //r->DrawBitmap(10, 50, 20, 30, PE::Resources::GetPixmap(":/textures/terrain/arrow.png"));
}

void IntroScene::Event_KeyPress(int key)
{
    if (key == Qt::Key::Key_Space)
        Game::CurrentGame->RequestScene(Scene_NewGame);
}
