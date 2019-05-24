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
    r->DrawText(r->GetWidth() / 2 - 160, r->GetHeight() / 2 - 200, "Use menu on top to start new game", QColor("black"), 20);

    r->DrawText(r->GetWidth() / 2 - 100, r->GetHeight() / 2 - 300, "Created by Petr Bena", QColor("black"), 20);
}
