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
#include "../projectiles/aitracer.h"

class TankBase;

class AI_HitMetrics
{
    public:
        AI_HitMetrics(double a, double p, PE::Vector pos);
        PE::Vector Source;
        double Angle;
        double Power;
        double ExpectedDistance = 999999999;
        int SuccessfulHits = 0;
        int Failures = 0;
};

class AI
{
    public:
        enum AI_State
        {
            AI_State_Undecided,
            AI_State_Waiting_Angle,
            AI_State_Fired,
            AI_State_Trace_Wait,
            AI_State_Nothing_To_Do
        };

        enum AI_Side
        {
            AI_Left,
            AI_Right
        };

        enum AI_PreviousHitEvaluation
        {
            AI_PreviousHitEvaluation_Pefect,
            AI_PreviousHitEvaluation_Good,
            AI_PreviousHitEvaluation_Average,
            AI_PreviousHitEvaluation_Bad,
            AI_PreviousHitEvaluation_Horrible,
            AI_PreviousHitEvaluation_Unknown
        };

    public:
        AI(TankBase *t);
        virtual ~AI();
        virtual void Process();
        virtual TankBase *FindClosestEnemy();
        virtual void ProcessInventory();
        virtual QString GetAIModelName();
        void Fire();
        virtual void WarmUp();

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
        virtual void postEvaluateFire();
        bool hasWeapon(int id);
        virtual void evaluateShield();
        virtual void evaluateWeapon();
        virtual void changeWeapon();
        void changeAngle(double new_angle);
        void changePower(double new_power);
        void increasePower(double p);
        virtual void trace();
        virtual void traceEval();
        void debug_log(const QString &text);
        QList<AI_HitMetrics*> metrics;
        QList<PE::Collectable_SmartPtr<AITracer>> tracers;
        int untracedCounter = 0;
        AI_State state = AI_State_Undecided;
        AI_PreviousHitEvaluation lastEvaluation = AI_PreviousHitEvaluation_Unknown;
        AI_PreviousHitEvaluation prevEvaluation = AI_PreviousHitEvaluation_Unknown;
        double targetTolerance = 0.01;
        int unknownDataCounter = 0;
        bool firstShot = true;
        PE::Vector previousHit;
        double targetAngle = 0;
        double targetPower = 0;
        double bestAngle = 0;
        double bestPower = 0;
        double bestDistance = 9999;
        double lastAngleChange = 0;
        double lastPowerChange = 0;
        AI_Side enemySide = AI_Right;
        TankBase *selectedEnemy = nullptr;
        double previousEnemyHP = 0;
        TankBase *tank;
};

#endif // AI_H
