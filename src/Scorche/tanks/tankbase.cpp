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
#include "../weapons/cannon.h"
#include <cmath>
#include <PixelEngine/definitions.h>
#include <PixelEngine/rigidbody.h>

TankBase *TankBase::PlayerTank = nullptr;

TankBase::TankBase(double x, double y, const QColor &color)
{
    this->tankColor = color;

    this->Position.X = x;
    this->Position.Y = y;

    this->RigidBody = new PE::Rigidbody();
    this->RigidBody->Weight = 0.1;

    this->SelectedWeapon = new Cannon(this);
}

void TankBase::Fire()
{
    if (this->SelectedWeapon == nullptr)
        return;

    this->SelectedWeapon->Fire(this->getCanonB(this->Position), this->GetCanonAngle(), this->Power);
}

void TankBase::Update(qint64 time)
{
    (void)time;
    if (this->canonAdjust == 0 && this->powerAdjust == 0)
        return;
    this->canonAngle += this->canonAdjust;
    this->Power += this->powerAdjust;
    if (this->Power < 1)
        this->Power = 1;
    if (this->Power > 100)
        this->Power = 100;
    this->RedrawNeeded = true;
}

void TankBase::Event_KeyPress(int key)
{
    if (!this->IsPlayer)
        return;

    switch (key)
    {
        case Qt::Key_A:
        case Qt::Key_Left:
            this->canonAdjust = 0.006;
            return;
        case Qt::Key_D:
        case Qt::Key_Right:
            this->canonAdjust = -0.006;
            return;
        case Qt::Key_Up:
        case Qt::Key_W:
            this->powerAdjust = 1;
            return;
        case Qt::Key_S:
        case Qt::Key_Down:
            this->powerAdjust = -1;
            return;
        case Qt::Key_Space:
            this->Fire();
            return;
    }
}

void TankBase::Event_KeyRelease(int key)
{
    if (!this->IsPlayer)
        return;

    switch (key)
    {
        case Qt::Key_A:
        case Qt::Key_D:
        case Qt::Key_Left:
        case Qt::Key_Right:
            this->canonAdjust = 0;
            return;
        case Qt::Key_Up:
        case Qt::Key_W:
        case Qt::Key_S:
        case Qt::Key_Down:
            this->powerAdjust = 0;
            return;
    }
}

double TankBase::GetCanonAngleDegree()
{
    // Simple radian to degree conversion
    return (this->canonAngle * PE_PI_RAD_CNV) * PE_RAD_DEG_CNV;
}

double TankBase::GetCanonAngle()
{
    if (this->canonAngle < 0)
        this->canonAngle = 0;

    if (this->canonAngle > PE_PI_RAD_CNV)
        this->canonAngle = PE_PI_RAD_CNV;

    return this->canonAngle;
}

PE::Vector TankBase::getCanonB(const PE::Vector &source)
{
    // angle math
    double radians = this->GetCanonAngle() * PE_PI_RAD_CNV;
    PE::Vector result = this->getCanonRoot(source);
    result.X += (12 * cos(radians));
    result.Y += (12 * sin(radians));
    return result;
}
