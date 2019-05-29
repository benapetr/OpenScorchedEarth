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
#include <QList>
#include <QColor>
#include "../weaponlist.h"

class AI;
class Weapon;
class PlayerInfo;

namespace PE
{
    class CircleCollider;
}

enum ShieldType
{
    ShieldNone,
    ShieldMini,
    ShieldHeavy,
    ShieldForce,
    ShieldHeavyForce,
    ShieldMag
};

class TankBase : public PE::Pawn
{
    public:
        static void ResetPlayers();
        static int LivingPlayers();
        static TankBase *GetActivePlayer();
        static TankBase *RotatePlayers();
        static TankBase *GetUnspawned();
        static void FinishRound();
        static QList<TankBase*> Players;
        static TankBase *ActivePlayer;
        static TankBase *PlayerTank;
        static bool ControlsFrozen;

        TankBase(double x, double y, PlayerInfo *player);
        ~TankBase() override;
        PlayerInfo *GetPlayer();
        virtual void Fire();
        virtual void Pass();
        bool CanPlay();
        void Update(qint64 time = 0) override;
        void Event_KeyPress(int key) override;
        void Event_KeyRelease(int key) override;
        double GetCanonAngleDegree();
        double GetCanonAngle();
        void Render(PE::Renderer *r, PE::Camera *c) override;
        virtual void TakeDamage(TankBase *source, double damage);
        virtual void Kill(TankBase *source);
        virtual bool IsAlive();
        bool CheckCollision(const PE::Vector &point);
        void InitializeBot();
        void SetCanonAdjustLeft();
        void SetCanonAdjustRight();
        void ResetCanonAdjust();
        void SetPower(double p);
        void SetAngle(double a);
        void IncreasePower(double p);
        void SwitchWeapon(int id);
        virtual void DeployShield(ShieldType shield);
        void DisableShield();
        void RestoreShield();
        virtual PE::Vector GetCanonRoot(const PE::Vector &source)=0;
        QColor GetShieldColor();
        void Warm();
        // Temporary hack to disable shield while firing so that we don't hit our own shield
        bool ShieldDisabled = false;
        bool BotInitialized = false;
        bool IsSpawned = false;
        bool WarmingUp = true;
        ShieldType Shield = ShieldNone;
        double ShieldPower = 0;
        QString PlayerName;
        Weapon *SelectedWeapon;
        bool IsPlayer = false;
        double Health = 100;
        double Power = 100;
        double Fuel = 100;
        PE::Vector LastHit;
        PE::Vector LastHit_Velocity;

    protected:
        virtual PE::Vector getCanonB(const PE::Vector &source);
        PE::Collectable_SmartPtr<PE::CircleCollider> shieldCollider;
        PlayerInfo *playerInfo;
        AI *ai = nullptr;
        double powerAdjust = 0;
        double canonAdjust = 0;
        double canonAngle = 0;
        QColor tankColor;
};

#endif // TANKBASE_H
