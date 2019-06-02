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
#include <PixelEngine/engine.h>
#include <PixelEngine/GC/gc.h>
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/pemath.h>
#include <PixelEngine/Physics/boxcollider.h>
#include <PixelEngine/world.h>
#include <PixelEngine/worldgenerator.h>
#include "console.h"
#include "playerinfo.h"
#include "shop.h"
#include "scenes/introscene.h"
#include "scenes/inventoryscene.h"
#include "scenes/newgame.h"
#include "playerstart.h"
#include "tanks/demotank.h"
#include "hud.h"

Game *Game::CurrentGame = nullptr;
bool  Game::AIQuickAim = true;
bool  Game::ExplosionEffects = true;
bool  Game::SuperFast = true;
bool  Game::Tracing = false;
bool  Game::FastTerrainUpdates = true;

Game::Game(double w_width, double w_height, PE::Renderer *r)
{
    Game::CurrentGame = this;
    Shop::DefaultShop = new Shop();
    this->renderer = r;
    this->MapHeight = w_height;
    this->MapWidth = w_width;
    this->renderer->ManualUpdate = true;

    //this->NewGame();
    this->showIntroScreen();

    this->timer = new QTimer(this);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(OnUpdate()));
    this->timer->start(10);

#ifdef PE_DEBUG
    PE::Collider::Debug = true;
#endif
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
    this->world->RegisterActor(new HUD(), 10);
    QColor background_color = this->getRandomBackgroundColor();
    QColor terrain = this->getRandomTerrainColor();
    this->Terrain = PE::WorldGenerator::GenerateRandom(static_cast<int>(this->MapWidth), static_cast<int>(this->MapHeight) - 20, background_color, terrain);
    // Move the terrain little bit higher, so there is space for HUD
    PE::Vector terrain_position = this->Terrain->Position;
    terrain_position.Y += 20;
    this->Terrain->SetPosition(terrain_position);
    this->world->RegisterTerrain(this->Terrain);
    this->world->BorderColor = QColor("blue");

    // Create floor
    this->world->RegisterCollider(new PE::BoxCollider(-1000, -100, 4000, 120));
    this->world->RegisterCollider(new PE::BoxCollider(-100, -100, 100, 2000));
    this->world->RegisterCollider(new PE::BoxCollider(this->MapWidth, -100, 100, 2000));
    this->world->RegisterCollider(new PE::BoxCollider(-1000, this->MapHeight + 800, 4000, 100));

    this->WarmingTanks = PlayerInfo::Players.count();

    int step = 0;
    if (PlayerInfo::Players.count() > 1)
        step = (this->MapHeight / (PlayerInfo::Players.count() - 1)) * 1.6;
    int player_id = 0;

    // Register players
    foreach (PlayerInfo *x, PlayerInfo::Players)
    {
        DemoTank *player = new DemoTank(10 + (player_id * step), 0, x);
        if (!x->IsBot)
            TankBase::PlayerTank = player;
        player->IsPlayer = !x->IsBot;
        this->world->RegisterActor(player);

        // Create player start, one for each bot, very high
        PlayerStart *ps = new PlayerStart(PE::Vector(10 + (player_id++ * step), this->MapHeight));
        this->world->RegisterActor(ps);
    }
}

void Game::startNewGame()
{
    this->CurrentScene = Scene_NewGame;
    PlayerInfo::Clear();
    TankBase::ResetPlayers();
    this->resetWorld();
    this->TerrainNeedsUpdate = false;
    this->world->RegisterActor(new NewGame(), 10);
}

void Game::OnUpdate()
{
    // emscripten doesn't support multi-threading so we have to do this using timer
#ifdef __EMSCRIPTEN__
    PE::Engine::GetEngine()->GetGC()->Collect();
#else
    // Temporary hack - let's check if random crashes aren't caused by multithreaded GC
    PE::Engine::GetEngine()->GetGC()->Collect();
#endif

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

    // Check if game is finished
    if (!this->IsFinished && this->CurrentScene == Scene_Game)
    {
        if (TankBase::LivingPlayers() <= 1)
        {
            // Find last player
            foreach (TankBase *t, TankBase::Players)
            {
                if (t->IsAlive())
                {
                    t->GetPlayer()->Score += 10000;
                    t->GetPlayer()->IncreaseMoney(6000);
                }
            }
            this->IsFinished = true;
            Console::Append("Game finished - giving $200 to each player");
            foreach (PlayerInfo *p, PlayerInfo::Players)
                p->IncreaseMoney(200);
        }
    }

    this->updateTerrain();
    this->world->Update();
    if (this->CurrentScene != Scene_Game || (!Game::SuperFast && !Game::Tracing))
        return;

    if (!TankBase::ControlsFrozen && TankBase::Players[0] == TankBase::ActivePlayer)
        return;

    if (Game::SuperFast)
    {
        int x = 5;
        while (--x > 0)
        {
            this->world->Update();
        }
    }
}

QColor Game::getRandomBackgroundColor()
{
    int color = PE::PEMath::GetRandom(0, 5);
    switch (color)
    {
        case 0:
            return QColor(190, 200, 216);
        case 1:
            return QColor(174, 180, 191);
        case 2:
            return QColor(185, 202, 232);
        case 3:
            return QColor(139, 175, 239);
        case 4:
            return QColor(178, 207, 255);
        case 5:
            return QColor(249, 240, 219);
    }
    return QColor(178, 207, 255);
}

QColor Game::getRandomTerrainColor()
{
    int color = PE::PEMath::GetRandom(0, 5);
    switch (color)
    {
        case 0:
            return QColor(30, 33, 31);
        case 1:
            return QColor(46, 48, 48);
        case 2:
            return QColor(35, 63, 43);
        case 3:
            return QColor(0, 0, 0);
        case 4:
            return QColor(7, 112, 37);
        case 5:
            return QColor(6, 66, 23);
    }
    return QColor(10, 10, 10);
}

void Game::updateTerrain()
{
    // Update terrain
    if (this->DynamicTerrain)
    {
        if (this->TerrainNeedsUpdate)
        {
            if (Game::FastTerrainUpdates)
                this->lastTerrainShiftedPoints = this->Terrain->ShiftFloatingBitsDown();
            else
                this->lastTerrainShiftedPoints = this->Terrain->ShiftFloatingBitsDownByOnePixel();
            this->Terrain->RefreshPixmap();
            this->Terrain->RedrawNeeded = true;
            if (this->lastTerrainShiftedPoints == 0)
            {
                // Terrain is all layed
                this->TerrainNeedsUpdate = false;
            }
        }
    }
}

void Game::resetWorld()
{
    Tracing = false;
    this->IsFinished = false;
    delete this->world;
    this->world = new PE::World(this->MapWidth, this->MapHeight);
    this->world->BackgroundColor = QColor(204, 221, 255);
}
