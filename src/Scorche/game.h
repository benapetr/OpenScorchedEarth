//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <PixelEngine/terrain.h>

namespace PE
{
    class World;
    class WorldGenerator;
    class Renderer;
}

class TankBase;

class Game : public QObject
{
        Q_OBJECT
    public:
        static Game *CurrentGame;
        static bool AIQuickAim;
        static bool ExplosionEffects;
        static bool SuperFast;
        static bool PlayerRequestNewGame;

        Game(double w_width, double w_height, PE::Renderer *r);
        ~Game() override;
        void GenerateRandomWorld();
        void ShowIntroScreen();
        void NewGame();
        PE::World *GetWorld() { return this->world; }
        PE::Collectable_SmartPtr<PE::Terrain> Terrain;
        double MapWidth;
        double MapHeight;
        bool IsPaused = false;

    protected slots:
        void OnUpdate();

    private:
        QTimer *timer;
        PE::World *world = nullptr;
        PE::Renderer *renderer = nullptr;
};

#endif // GAME_H
