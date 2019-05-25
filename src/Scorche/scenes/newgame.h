//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef NEWGAME_H
#define NEWGAME_H

#include <PixelEngine/actor.h>

class NewGame : public PE::Actor
{
    public:
        NewGame();
        void Render(PE::Renderer *r, PE::Camera *c) override;
        void Event_KeyPress(int key) override;
        void SelectUp();
        void SelectDown();
        int SelectedItem = 0;
        int Bots = 1;
};

#endif // NEWGAME_H
