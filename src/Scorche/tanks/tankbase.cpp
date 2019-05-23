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
#include "../ai.h"
#include "../weapons/cannon.h"
#include <cmath>
#include <PixelEngine/camera.h>
#include <PixelEngine/Physics/collider.h>
#include <PixelEngine/definitions.h>
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/Physics/rigidbody.h>

QList<TankBase*> TankBase::Players;
TankBase *TankBase::PlayerTank = nullptr;

TankBase::TankBase(double x, double y, const QColor &color, const QString &player_name, bool bot)
{
    Players.append(this);
    this->tankColor = color;
    this->PlayerName = player_name;
    if (bot)
        this->ai = new AI(this);

    this->Position.X = x;
    this->Position.Y = y;

    this->RigidBody = new PE::Rigidbody();
    this->RigidBody->Weight = 0.1;

    this->SelectedWeapon = new Cannon(this);
}

TankBase::~TankBase()
{
    Players.removeOne(this);
    delete this->ai;
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

void TankBase::Render(PE::Renderer *r, PE::Camera *c)
{
    // Get position to render on
    PE::Vector position = c->ProjectedPosition(this->Position);
    position.Y += 50;

    if (!this->PlayerName.isEmpty())
    {
        // Render name information
        r->DrawText(position.X2int(), position.Y2int(), this->PlayerName, QColor("black"));
    }

    // Render HP information
    r->DrawText(position.X2int(), position.Y2int() - 20, QString::number(this->Health), QColor("black"));
}

void TankBase::TakeDamage(TankBase *source, double damage)
{
    this->Health -= damage;
    if (this->Health < 0)
        this->Health = 0;

    if (!this->IsAlive())
        this->Kill(source);
}

void TankBase::Kill(TankBase *source)
{

}

bool TankBase::IsAlive()
{
    return (this->Health > 0);
}

bool TankBase::CheckCollision(const PE::Vector &point)
{
    foreach (PE::Collider *c, this->colliders)
    {
        if (c->PositionMatch(point))
            return true;
    }
    return false;
}

void TankBase::InitializeBot()
{

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
