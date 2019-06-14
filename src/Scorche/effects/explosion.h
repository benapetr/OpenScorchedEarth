//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "genericeffect.h"

class TankBase;

class Explosion : public GenericEffect
{
    public:
        static int ExplosionCounter;

        Explosion(TankBase *p, double size);
        ~Explosion() override;
        void Update(qint64 time) override;
        void Render(PE::Renderer *r, PE::Camera *c) override;
        double Damage = 1000;
        //! Higher = more damage on distance hit
        double DistanceDamageRatio = 1;
        bool TerrainCreating = false;
        bool Sonic = false;
        bool FallingRocks = true;

    protected:
        void destroyTerrain();
        TankBase *owner;
        QList<TankBase*> untouchedTanks;
        double maxSize;
        double currentSize = 0;
        unsigned int stage = 0;
};

#endif // EXPLOSION_H
