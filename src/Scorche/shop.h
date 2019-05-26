//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef SHOP_H
#define SHOP_H

#include "weaponlist.h"
#include <QHash>

class PlayerInfo;
class Shop
{
    public:
        static Shop *DefaultShop;
        Shop();
        bool BuyItem(PlayerInfo *player, int item_id);
        bool SellItem(PlayerInfo *player, int item_id);
        QString ItemString(int i);
        QHash<int, int> PriceList;
};

#endif // SHOP_H
