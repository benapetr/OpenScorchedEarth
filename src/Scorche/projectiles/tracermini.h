//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef TRACERMINI_H
#define TRACERMINI_H

#include "projectile.h"
#include <QImage>

class TracerMini : public Projectile
{
    public:
        TracerMini(const PE::Vector &position);
        void Update(qint64 time) override;
        void Render(PE::Renderer *r, PE::Camera *c) override;
    protected:
        void on_Terrain(PE::Collider *collider) override;
        void on_Vehicle(TankBase *tank) override;
        PE::Vector lastPosition;
};

#endif // TRACERMINI_H
