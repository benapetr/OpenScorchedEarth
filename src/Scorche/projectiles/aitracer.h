//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef AITRACER_H
#define AITRACER_H

#include "projectile.h"

class AITracer : public Projectile
{
    public:
        static bool Debug;
        AITracer(const PE::Vector &position);
        void Render(PE::Renderer *r, PE::Camera *c) override;
        void Event_Destroyed() override;
        PE::Vector PositionFinal;
        double Angle = 0;
        double Power = 0;
        bool Finished = false;
        TankBase *target = nullptr;;
    protected:
        void on_Terrain(PE::Collider *collider) override;
        void on_Vehicle(TankBase *tank) override;
};

#endif // AITRACER_H
