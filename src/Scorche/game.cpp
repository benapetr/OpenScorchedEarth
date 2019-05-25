//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "game.h"
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/Physics/boxcollider.h>
#include <PixelEngine/world.h>
#include <PixelEngine/worldgenerator.h>
#include "scenes/introscene.h"
#include "tanks/demotank.h"
#include "hud.h"

Game *Game::CurrentGame = nullptr;
bool  Game::AIQuickAim = true;
bool  Game::ExplosionEffects = true;
bool  Game::SuperFast = false;
bool  Game::PlayerRequestNewGame = false;

Game::Game(double w_width, double w_height, PE::Renderer *r)
{
    Game::CurrentGame = this;
    this->renderer = r;
    this->MapHeight = w_height;
    this->MapWidth = w_width;
    this->renderer->ManualUpdate = true;

    //this->NewGame();
    this->ShowIntroScreen();

    this->timer = new QTimer(this);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(OnUpdate()));
    this->timer->start(10);
}

Game::~Game()
{
    this->timer->stop();
    delete this->world;
}

void Game::GenerateRandomWorld()
{

}

void Game::ShowIntroScreen()
{
    TankBase::ResetPlayers();
    delete this->world;

    this->world = new PE::World(this->MapWidth, this->MapHeight);
    this->world->BackgroundColor = QColor(204, 221, 255);
    this->world->RegisterActor(new IntroScene(), 10);
}

void Game::NewGame()
{
    TankBase::ResetPlayers();
    delete this->world;

    this->world = new PE::World(this->MapWidth, this->MapHeight);
    this->world->BackgroundColor = QColor(204, 221, 255);
    this->world->RegisterObject(new HUD(), 10);
    this->Terrain = PE::WorldGenerator::GenerateRandom(static_cast<int>(this->MapWidth), static_cast<int>(this->MapHeight) - 20);
    // Move the terrain little bit higher, so there is space for HUD
    PE::Vector terrain_position = this->Terrain->Position;
    terrain_position.Y += 20;
    this->Terrain->SetPosition(terrain_position);
    this->world->RegisterTerrain(this->Terrain);

    // Create floor
    this->world->RegisterCollider(new PE::BoxCollider(-1000, -100, 4000, 120));
    this->world->RegisterCollider(new PE::BoxCollider(-200, -100, 100, 2000));
    this->world->RegisterCollider(new PE::BoxCollider(this->MapWidth + 100, -100, 100, 2000));
    this->world->RegisterCollider(new PE::BoxCollider(-1000, this->MapHeight + 400, 4000, 100));

    DemoTank *player = new DemoTank(10, 700, Qt::darkRed, "Player", false);
    TankBase::PlayerTank = player;
    player->IsPlayer = true;

    this->world->RegisterActor(player);
    this->world->RegisterActor(new DemoTank(400, 700, Qt::darkBlue, "Bot", true));
    this->world->RegisterActor(new DemoTank(200, 700, Qt::darkCyan, "Bot2", true));
    this->world->RegisterActor(new DemoTank(600, 700, Qt::darkYellow, "Bot3", true));
    this->world->RegisterActor(new DemoTank(800, 700, Qt::darkMagenta, "Bot4", true));
    this->world->RegisterActor(new DemoTank(1000, 700, Qt::darkMagenta, "SuperBot", true));
}

void Game::OnUpdate()
{
    if (PlayerRequestNewGame)
    {
        PlayerRequestNewGame = false;
        this->NewGame();
        return;
    }

    this->world->Update();
    if (!Game::SuperFast)
        return;

    int x = 5;
    while (--x > 0)
        this->world->Update();
}
