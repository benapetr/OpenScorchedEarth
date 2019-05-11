//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef DEMOTANK_H
#define DEMOTANK_H

#include "tankbase.h"

class DemoTank : public TankBase
{
    public:
        DemoTank();
        DemoTank(double x, double y);
        void Render(PE::Renderer *r) override;
    private:
        int width = 20;
        int height = 10;
};

#endif // DEMOTANK_H
