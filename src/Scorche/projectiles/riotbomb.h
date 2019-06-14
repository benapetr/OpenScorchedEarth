//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef RIOTBOMB_H
#define RIOTBOMB_H

#include "projectile.h"

class RiotBomb : public Projectile
{
    public:
        RiotBomb(bool heavy, const PE::Vector &position);
        void Render(PE::Renderer *r, PE::Camera *c) override;
    protected:
        void on_Terrain(PE::Collider *collider) override;
        void on_Vehicle(TankBase *tank) override;
        void explode();
        bool IsHeavy;
};

#endif // RIOTBOMB_H
