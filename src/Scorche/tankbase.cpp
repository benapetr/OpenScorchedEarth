//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "tankbase.h"
#include <cmath>
#include <PixelEngine/definitions.h>

TankBase::TankBase()
{

}

PE::Vector TankBase::getCanonB()
{
    if (this->canonAngle < 0)
        this->canonAngle = 0;

    if (this->canonAngle > 180)
        this->canonAngle = 180;

    // angle math
    double radians = this->canonAngle * PE_PI_RAD_CNV;
    PE::Vector result = this->getCanonRoot();
    result.X += (12 * cos(radians));
    result.Y += (12 * sin(radians));
    return result;
}
