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
#include <PixelEngine/renderer.h>
#include <PixelEngine/boxcollider.h>
#include <PixelEngine/world.h>
#include <PixelEngine/worldgenerator.h>
#include "demotank.h"

Game::Game(double w_width, double w_height, PE::Renderer *r)
{
    this->renderer = r;
    this->renderer->ManualUpdate = true;
    this->world = new PE::World(w_width, w_height);
    this->world->BackgroundColor = QColor(204, 221, 255);
    this->world->RegisterTerrain(PE::WorldGenerator::GenerateRandom(static_cast<int>(w_width), static_cast<int>(w_height)));
    this->world->Render(r);
    this->timer = new QTimer(this);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(OnUpdate()));
    this->timer->start(10);

    // Create floor
    PE::BoxCollider *floor = new PE::BoxCollider(-100, -15, 2000, 20);
    this->world->RegisterCollider(floor);

    this->world->RegisterActor(new DemoTank(10, 30, Qt::darkRed));
    this->world->RegisterActor(new DemoTank(40, 50, Qt::darkBlue));

    /*int Y = 80;
    while (Y < 700)
    {
        this->world->RegisterActor(new DemoTank(10, Y));
        this->world->RegisterActor(new DemoTank(40, Y));
        this->world->RegisterActor(new DemoTank(80, Y));
        this->world->RegisterActor(new DemoTank(120, Y));
        this->world->RegisterActor(new DemoTank(150, Y));
        this->world->RegisterActor(new DemoTank(180, Y));
        this->world->RegisterActor(new DemoTank(230, Y));
        this->world->RegisterActor(new DemoTank(250, Y));
        this->world->RegisterActor(new DemoTank(280, Y));
        this->world->RegisterActor(new DemoTank(330, Y));
        this->world->RegisterActor(new DemoTank(360, Y));
        this->world->RegisterActor(new DemoTank(400, Y));
        this->world->RegisterActor(new DemoTank(450, Y));
        this->world->RegisterActor(new DemoTank(500, Y));
        this->world->RegisterActor(new DemoTank(550, Y));
        this->world->RegisterActor(new DemoTank(600, Y));
        this->world->RegisterActor(new DemoTank(630, Y));
        this->world->RegisterActor(new DemoTank(660, Y));
        this->world->RegisterActor(new DemoTank(700, Y));
        this->world->RegisterActor(new DemoTank(730, Y));
        this->world->RegisterActor(new DemoTank(760, Y));
        this->world->RegisterActor(new DemoTank(800, Y));
        this->world->RegisterActor(new DemoTank(830, Y));
        this->world->RegisterActor(new DemoTank(860, Y));
        Y = Y + 50;
    } */

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
