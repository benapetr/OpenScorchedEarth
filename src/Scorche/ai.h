//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef AI_H
#define AI_H

#include <PixelEngine/vector.h>
#include <QString>

class TankBase;

class AI
{
    public:
        enum AI_State
        {
            AI_State_Undecided,
            AI_State_Waiting_Angle,
            AI_State_Fired,
            AI_State_Nothing_To_Do
        };

        enum AI_Side
        {
            AI_Left,
            AI_Right
        };

    public:
        AI(TankBase *t);
        virtual ~AI();
        virtual void Process();
        virtual TankBase *FindClosestEnemy();
        void Fire();

    protected:
        // AI stuff
        virtual void getTargetAngle();
        virtual void improveAngle(double max);
        virtual void improvePower(double max);
        virtual void resetEnemy();
        virtual void normalizeAngle();
        virtual bool fallenBefore(double X);
        virtual bool flewOver(double X);
        virtual bool flewAway(double X);
        virtual void evaluateFire();

        void debug_log(const QString &text);
        AI_State state = AI_State_Undecided;
        double targetTolerance = 0.01;
        int unknownDataCounter = 0;
        PE::Vector previousHit;
        double targetAngle = 0;
        double bestAngle = 0;
        double bestDistance = 9999;
        AI_Side enemySide = AI_Right;
        TankBase *selectedEnemy = nullptr;
        double previousEnemyHP = 0;
        TankBase *tank;
};

#endif // AI_H
