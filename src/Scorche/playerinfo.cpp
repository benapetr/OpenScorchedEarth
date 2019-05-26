//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include <PixelEngine/pemath.h>
#include "weaponlist.h"
#include "playerinfo.h"

QList<PlayerInfo*> PlayerInfo::Players;

void PlayerInfo::Clear()
{
    qDeleteAll(PlayerInfo::Players);
    PlayerInfo::Players.clear();
}

QColor PlayerInfo::RandomColor()
{
    QList<QColor> colors;
    colors << QColor("red")
           << QColor("green")
           << QColor(244, 107, 66)
           << QColor(217, 244, 65)
           << QColor(157, 65, 244)
           << QColor(0, 153, 60);

    return colors[PE::PEMath::GetRandom(0, colors.size() - 1)];
}

QColor PlayerInfo::GetRandomUnusedColor()
{
    QColor result;
    while (true)
    {
        result = PlayerInfo::RandomColor();
        foreach (PlayerInfo *p, PlayerInfo::Players)
        {
            if (result == p->Color)
                continue;
        }
        return result;
    }
}

PlayerInfo::PlayerInfo(const QString &name, const QColor &color, bool bot)
{
    this->PlayerName = name;
    this->IsBot = bot;
    this->Color = color;
    this->ItemList.insert(WEAPON_NUKE, 0);
    this->ItemList.insert(WEAPON_BIG_CANON, 0);
    this->ItemList.insert(WEAPON_MINI_NUKE, 0);
    this->ItemList.insert(WEAPON_TRIPLE_CANON, 0);
    this->ItemList.insert(INVENTORY_FUEL, 0);
    this->ItemList.insert(INVENTORY_SHIELD, 0);
    this->ItemList.insert(INVENTORY_PARACHUTE, 0);
    this->ItemList.insert(INVENTORY_REPAIR_KIT, 0);
    this->ItemList.insert(INVENTORY_HEAVY_SHIELD, 0);
}

void PlayerInfo::IncreaseMoney(int value)
{
    this->Cash += value;
    if (this->Cash < 0)
        this->Cash = 0;
}
