//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef ROCKS_H
#define ROCKS_H

#include "genericeffect.h"

class Rocks : public GenericEffect
{
    public:
        Rocks();
        void Update(qint64 time) override;
        void Render(PE::Renderer *r, PE::Camera *c) override;
        void Event_OnImpact(const PE::Vector &v) override;
        void RandomForce();
};

#endif // ROCKS_H
