//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef INVENTORYSCENE_H
#define INVENTORYSCENE_H

#include <PixelEngine/actor.h>
#include <QHash>

class InventoryScene : public PE::Actor
{
    public:
        InventoryScene();
        void Render(PE::Renderer *r, PE::Camera *c) override;
        void Event_KeyPress(int key) override;
    private:
        void insertItem(PE::Renderer *r, int id, int position);
        void SelectUp();
        void SelectDown();
        int GetItemFromID();
        int selectedItem = 0;
};

#endif // INVENTORYSCENE_H
