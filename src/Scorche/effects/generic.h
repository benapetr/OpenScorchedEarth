//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef GENERIC_H
#define GENERIC_H

#include <PixelEngine/actor.h>
#include <PixelEngine/terrain.h>

namespace PE
{
    class Terrain;
}

class TankBase;

class Generic : public PE::Actor
{
    public:
        Generic();
};

#endif // GENERIC_H