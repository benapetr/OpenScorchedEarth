//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef ROCKET_H
#define ROCKET_H

#include <PixelEngine/vector.h>
#include "projectile.h"

class Rocket : public Projectile
{
    public:
        Rocket(const PE::Vector &position);
        void Render(PE::Renderer *r, PE::Camera *c) override;
    protected:
        void on_Terrain(PE::Collider *collider) override;
        void on_Vehicle(TankBase *tank) override;
};

#endif // ROCKET_H
