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
#include "../weaponlist.h"
#include "../shop.h"
#include "../staticassets.h"
#include "../playerinfo.h"
#include "../game.h"

InventoryScene::InventoryScene()
{

}

void InventoryScene::Render(PE::Renderer *r, PE::Camera *c)
{
    (void)c;
    r->DrawText(r->GetWidth() / 2 - 120, r->GetHeight() - 60, "Inventory", QColor("black"), 40);
    r->DrawText(20, r->GetHeight() - 80, "Your money: $" + QString::number(PlayerInfo::Players[0]->Cash), QColor("black"), 20);
    this->insertItem(r, WEAPON_BIG_CANON, 0);
    this->insertItem(r, WEAPON_TRIPLE_CANON, 1);
    this->insertItem(r, WEAPON_MINI_NUKE, 2);
    this->insertItem(r, WEAPON_NUKE, 3);
    this->insertItem(r, INVENTORY_SHIELD, 4);
    this->insertItem(r, INVENTORY_HEAVY_SHIELD, 5);
    this->insertItem(r, INVENTORY_REPAIR_KIT, 6);
    this->insertItem(r, WEAPON_HEAVY_RIOT_BOMB, 7);
    this->insertItem(r, WEAPON_RIOT_BOMB, 8);
    this->insertItem(r, WEAPON_SONIC_BOMB, 9);
    this->insertItem(r, WEAPON_HEAVY_SONIC_BOMB, 10);
    this->insertItem(r, WEAPON_TRACER, 11);

    r->DrawBitmap(0, r->GetHeight() - 120 - (this->selectedItem * 20), 20, 20, StaticAssets::Instance->RightArrow);
    r->DrawBitmap(360, r->GetHeight() - 120 - (this->selectedItem * 20), 20, 20, StaticAssets::Instance->LeftArrow);
    r->DrawBitmap(380, r->GetHeight() - 120 - (this->selectedItem * 20), 20, 20, StaticAssets::Instance->RightArrow);


    //r->DrawBitmap(420, r->GetHeight() / 2 + 100, 40, 20, StaticAssets::Instance->RightArrow);
    //r->DrawBitmap(462, r->GetHeight() / 2 + 100, 40, 20, StaticAssets::Instance->LeftArrow);
    r->DrawText(r->GetWidth() / 2 - 138, r->GetHeight() / 2 - 200, "Press space to start game", QColor("black"), 20);
}

void InventoryScene::Event_KeyPress(int key)
{
    this->RedrawNeeded = true;
    switch (key)
    {
        case Qt::Key::Key_Left:
            Shop::DefaultShop->SellItem(PlayerInfo::Players[0], this->GetItemFromID());
            break;
        case Qt::Key::Key_Right:
            Shop::DefaultShop->BuyItem(PlayerInfo::Players[0], this->GetItemFromID());
            break;
        case Qt::Key::Key_Space:
            Game::CurrentGame->RequestScene(Scene_Game);
            return;
        case Qt::Key::Key_Down:
            this->SelectDown();
            break;
        case Qt::Key::Key_Up:
            this->SelectUp();
            break;
    }
}

void InventoryScene::insertItem(PE::Renderer *r, int id, int position)
{
    r->DrawText(20, r->GetHeight() - 120 - (position * 20), Shop::DefaultShop->ItemString(id) +  "($" + QString::number(Shop::DefaultShop->PriceList[id]) + "): ", QColor("black"), 20);
    r->DrawText(310, r->GetHeight() - 120 - (position * 20), QString::number(PlayerInfo::Players[0]->ItemList[id]), QColor("black"), 20);
}

void InventoryScene::SelectUp()
{
    this->selectedItem--;
    if (this->selectedItem < 0)
        this->selectedItem = 0;
}

void InventoryScene::SelectDown()
{
    this->selectedItem++;
    if (this->selectedItem > 11)
        this->selectedItem = 11;
}

int InventoryScene::GetItemFromID()
{
    switch(this->selectedItem)
    {
        case 0:
            return WEAPON_BIG_CANON;
        case 1:
            return WEAPON_TRIPLE_CANON;
        case 2:
            return WEAPON_MINI_NUKE;
        case 3:
            return WEAPON_NUKE;
        case 4:
            return INVENTORY_SHIELD;
        case 5:
            return INVENTORY_HEAVY_SHIELD;
        case 6:
            return INVENTORY_REPAIR_KIT;
        case 7:
            return WEAPON_HEAVY_RIOT_BOMB;
        case 8:
            return WEAPON_RIOT_BOMB;
        case 9:
            return WEAPON_SONIC_BOMB;
        case 10:
            return WEAPON_HEAVY_SONIC_BOMB;
        case 11:
            return WEAPON_TRACER;
    }

    return WEAPON_CANON;
}
