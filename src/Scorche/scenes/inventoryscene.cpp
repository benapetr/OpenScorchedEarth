//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include <PixelEngine/Graphics/renderer.h>
#include "inventoryscene.h"
#include "../game.h"

InventoryScene::InventoryScene()
{

}

void InventoryScene::Render(PE::Renderer *r, PE::Camera *c)
{
    (void)c;
    r->DrawText(r->GetWidth() / 2 - 120, r->GetHeight() - 60, "Inventory", QColor("black"), 40);

    //r->DrawBitmap(420, r->GetHeight() / 2 + 100, 40, 20, StaticAssets::Instance->RightArrow);
    //r->DrawBitmap(462, r->GetHeight() / 2 + 100, 40, 20, StaticAssets::Instance->LeftArrow);
    r->DrawText(r->GetWidth() / 2 - 138, r->GetHeight() / 2 - 200, "Press space to open inventory", QColor("black"), 20);
}

void InventoryScene::Event_KeyPress(int key)
{
    this->RedrawNeeded = true;
    switch (key)
    {
        case Qt::Key::Key_Space:
            Game::CurrentGame->RequestScene(Scene_Game);
            return;

    }
}
