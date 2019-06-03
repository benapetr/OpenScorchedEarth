//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "shop.h"
#include "console.h"
#include "playerinfo.h"

Shop *Shop::DefaultShop = nullptr;

Shop::Shop()
{
    this->PriceList.insert(WEAPON_CANON, 0);
    this->PriceList.insert(INVENTORY_FUEL, 100);
    this->PriceList.insert(INVENTORY_SHIELD, 400);
    this->PriceList.insert(INVENTORY_PARACHUTE, 100);
    this->PriceList.insert(INVENTORY_HEAVY_SHIELD, 2000);
    this->PriceList.insert(INVENTORY_REPAIR_KIT, 40);
    this->PriceList.insert(WEAPON_NUKE, 2000);
    this->PriceList.insert(WEAPON_MINI_NUKE, 500);
    this->PriceList.insert(WEAPON_TRIPLE_CANON, 20);
    this->PriceList.insert(WEAPON_BIG_CANON, 10);
}

bool Shop::BuyItem(PlayerInfo *player, int item_id)
{
    if (player->Cash < this->PriceList[item_id])
    {
        // Can't afford
        return false;
    }

    player->ItemList[item_id]++;
    player->Cash -= this->PriceList[item_id];
    Console::Append(player->PlayerName + " bought 1 item: " + this->ItemString(item_id), true);
    return true;
}

bool Shop::SellItem(PlayerInfo *player, int item_id)
{
    if (player->ItemList[item_id] <= 0)
        return false;

    player->ItemList[item_id]--;
    player->Cash += this->PriceList[item_id];
    Console::Append(player->PlayerName + " sold 1 item: " + this->ItemString(item_id), true);
    return true;
}

QString Shop::ItemString(int i)
{
    switch (i)
    {
        case WEAPON_CANON:
            return "Cannon";
        case WEAPON_NUKE:
            return "Nuke";
        case WEAPON_BIG_CANON:
            return "Big cannon";
        case WEAPON_MINI_NUKE:
            return "Mini nuke";
        case WEAPON_PIERCER:
            return "Terrain piercer";
        case WEAPON_SONIC_WAVE:
            return "Sonic wave";
        case WEAPON_TRIPLE_CANON:
            return "Triple rocket";
        case INVENTORY_SHIELD:
            return "Shield";
        case INVENTORY_HEAVY_SHIELD:
            return "Heavy shield";
        case INVENTORY_REPAIR_KIT:
            return "Repair kit";
        case INVENTORY_PARACHUTE:
            return "Parachute";


    }
    return QString::number(i);
}
