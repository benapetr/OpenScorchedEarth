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
#include "renderer.h"
#include "world.h"
#include "worldgenerator.h"
#include "demotank.h"

Game::Game(double w_width, double w_height, PE::Renderer *r)
{
    this->renderer = r;
    this->renderer->ManualUpdate = true;
    this->world = PE::WorldGenerator::GenerateRandom(w_width, w_height);
    this->world->Render(r);
    this->timer = new QTimer(this);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(OnUpdate()));
    this->timer->start(10);
    DemoTank *tank = new DemoTank();
    tank->Position = PE::Vector(40, 600);
    this->world->RegisterActor(tank);
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
