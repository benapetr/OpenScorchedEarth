//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <QString>

class PlayerInfo
{
    public:
        PlayerInfo(QString name);
        QString PlayerName;
        double Cash = 2000;

        unsigned int BigCanon = 0;
        unsigned int TripleCanon = 0;
        unsigned int Nuke = 0;
        unsigned int MiniNuke = 0;

        unsigned int HeavyShields = 0;
        unsigned int Shields = 0;

        unsigned int RepairKits = 0;
        unsigned int Fuel = 0;
};

#endif // PLAYERINFO_H
