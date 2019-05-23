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

class Weapon;

class TankBase : public PE::Pawn
{
    public:
        static TankBase *PlayerTank;

        TankBase(double x, double y, const QColor &color);
        virtual void Fire();
        void Update(qint64 time = 0) override;
        void Event_KeyPress(int key) override;
        void Event_KeyRelease(int key) override;
        double GetCanonAngleDegree();
        double GetCanonAngle();
        void Render(PE::Renderer *r, PE::Camera *c) override;
        virtual void TakeDamage(TankBase *source, double damage);
        virtual void Kill(TankBase *source);
        virtual bool IsAlive();
        QString PlayerName;
        Weapon *SelectedWeapon;
        bool IsPlayer = false;
        double Health = 100;
        double Power = 100;
        double Fuel = 100;

    protected:
        virtual PE::Vector getCanonRoot(const PE::Vector &source)=0;
        virtual PE::Vector getCanonB(const PE::Vector &source);
        double powerAdjust = 0;
        double canonAdjust = 0;
        double canonAngle = 0;
        QColor tankColor;
};

#endif // TANKBASE_H
