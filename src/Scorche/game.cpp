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
#include "console.h"
#include "playerinfo.h"
#include "scenes/introscene.h"
#include "scenes/inventoryscene.h"
#include "scenes/newgame.h"
#include "tanks/demotank.h"
#include "hud.h"

Game *Game::CurrentGame = nullptr;
bool  Game::AIQuickAim = true;
bool  Game::ExplosionEffects = true;
bool  Game::SuperFast = false;

Game::Game(double w_width, double w_height, PE::Renderer *r)
{
    Game::CurrentGame = this;
    this->renderer = r;
    this->MapHeight = w_height;
    this->MapWidth = w_width;
    this->renderer->ManualUpdate = true;

    //this->NewGame();
    this->showIntroScreen();

    this->timer = new QTimer(this);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(OnUpdate()));
    this->timer->start(10);
}

Game::~Game()
{
    this->timer->stop();
    delete this->world;
}

void Game::RequestScene(Scene s)
{
    this->requestedScene = s;
}

void Game::showIntroScreen()
{
    this->CurrentScene = Scene_Intro;
    this->resetWorld();
    this->world->RegisterActor(new IntroScene(), 10);
}

void Game::showInventoryScreen()
{
        this->CurrentScene = Scene_NewGame;
        TankBase::ResetPlayers();
        this->resetWorld();
        this->world->RegisterActor(new InventoryScene(), 10);
}

void Game::startGame()
{
    this->CurrentScene = Scene_Game;
    TankBase::ResetPlayers();
    this->resetWorld();
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

    int step = 0;
    if (PlayerInfo::Players.count() > 0)
        step = (this->MapHeight / (PlayerInfo::Players.count() - 1)) + 80;
    int player_id = 0;

    // Register players
    foreach (PlayerInfo *x, PlayerInfo::Players)
    {
        DemoTank *player = new DemoTank(10 + (player_id++ * step), 700, x->Color, x->PlayerName, x->IsBot);
        if (!x->IsBot)
            TankBase::PlayerTank = player;
        player->IsPlayer = !x->IsBot;
        this->world->RegisterActor(player);
    }
}

void Game::startNewGame()
{
    this->CurrentScene = Scene_NewGame;
    PlayerInfo::Clear();
    TankBase::ResetPlayers();
    this->resetWorld();
    this->world->RegisterActor(new NewGame(), 10);
}

void Game::OnUpdate()
{
    if (this->requestedScene != Scene_Nothing)
    {
        switch (this->requestedScene)
        {
            case Scene_NewGame:
                this->startNewGame();
                break;
            case Scene_Inventory:
                this->showInventoryScreen();
                break;
            case Scene_Game:
                this->startGame();
                break;
            case Scene_Intro:
                this->showIntroScreen();
                break;
            default:
                Console::Append(QString("Unknown scene requested in ") + BOOST_CURRENT_FUNCTION);
                break;
        }
        this->requestedScene = Scene_Nothing;
        return;
    }

    this->world->Update();
    if (this->CurrentScene != Scene_Game || !Game::SuperFast)
        return;

    int x = 5;
    while (--x > 0)
        this->world->Update();
}

void Game::resetWorld()
{
    delete this->world;
    this->world = new PE::World(this->MapWidth, this->MapHeight);
    this->world->BackgroundColor = QColor(204, 221, 255);
}
