//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef TANKBASE_H
#define TANKBASE_H

#include <PixelEngine/pawn.h>
#include <PixelEngine/vector.h>
#include <QColor>

class TankBase : public PE::Pawn
{
    public:
        TankBase();
        QColor tankColor;

    protected:
        virtual PE::Vector getCanonRoot()=0;
        virtual PE::Vector getCanonB();
        double canonAngle = 0;
};

#endif // TANKBASE_H
