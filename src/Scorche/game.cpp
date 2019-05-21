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
#include "tanks/demotank.h"
#include "hud.h"

Game *Game::CurrentGame = nullptr;

Game::Game(double w_width, double w_height, PE::Renderer *r)
{
    Game::CurrentGame = this;
    this->renderer = r;
    this->renderer->ManualUpdate = true;
    this->world = new PE::World(w_width, w_height);
    this->world->BackgroundColor = QColor(204, 221, 255);
    this->world->RegisterObject(new HUD(), 10);
    PE::Collectable_SmartPtr<PE::Terrain> terrain = PE::WorldGenerator::GenerateRandom(static_cast<int>(w_width), static_cast<int>(w_height) - 20);
    // Move the terrain little bit higher, so there is space for HUD
    PE::Vector terrain_position = terrain->Position;
    terrain_position.Y += 20;
    terrain->SetPosition(terrain_position);
    this->world->RegisterTerrain(terrain);
    this->world->Render(r);
    this->timer = new QTimer(this);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(OnUpdate()));
    this->timer->start(10);

    // Create floor
    PE::Collectable_SmartPtr<PE::BoxCollider> floor = new PE::BoxCollider(-100, 0, 2000, 20);
    this->world->RegisterCollider(floor);

    DemoTank *player = new DemoTank(10, 700, Qt::darkRed);
    TankBase::PlayerTank = player;
    player->IsPlayer = true;

    this->world->RegisterActor(player);
    this->world->RegisterActor(new DemoTank(800, 700, Qt::darkBlue));
}

Game::~Game()
{
    this->timer->stop();
    delete this->world;
}

void Game::GenerateRandomWorld()
{

}

void Game::OnUpdate()
{
    this->world->Update();
    this->world->Render(this->renderer);
}
