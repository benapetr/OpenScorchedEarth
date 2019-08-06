//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "mainwindow.h"
#include "projectiles/aitracer.h"
#include "game.h"
#include "staticassets.h"
#include "console.h"
#include <QKeyEvent>
#include <QDateTime>
#include <QImage>
#include <QApplication>
#include <QDesktopWidget>
#include <PixelEngine/engine.h>
#include <PixelEngine/world.h>
#include <PixelEngine/ringlog.h>
#include <PixelEngine/ringlog_item.h>
#ifdef SCORCHE_GL
#include <PixelEngine/Graphics/peglwidget.h>
#include <PixelEngine/Graphics/qglrenderer.h>
#else
#include <PixelEngine/Graphics/qimagerenderer.h>
#endif

MainWindow *MainWindow::Main = nullptr;

MainWindow::MainWindow()
{
    MainWindow::Main = this;
    this->setWidth(QApplication::desktop()->screenGeometry().width());
    this->setHeight(QApplication::desktop()->screenGeometry().height() - 120);
    PE::Engine::Initialize(false);
    this->setTitle("Open Scorched Earth");
    //this->showMaximized();
    StaticAssets::Instance = new StaticAssets();
    this->initializeRenderer();
    this->game = new Game(this->GetWidth(), this->GetHeight());
    this->SetWorld(this->game->GetWorld());

    // Init console
    foreach (PE::RingLog_Item item, PE::Engine::GetEngine()->RL->GetItems())
    {
        Console::Append(item.GetText());
    }

    //this->ui->actionFast_game->setChecked(Game::SuperFast);

    new Console(nullptr);
#ifdef __EMSCRIPTEN__
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
#endif
}

MainWindow::~MainWindow()
{
    delete this->game;
    delete StaticAssets::Instance;
    StaticAssets::Instance = nullptr;
}

int MainWindow::GetWidth()
{
    return this->width();
}

int MainWindow::GetHeight()
{
    return this->height();
}

void MainWindow::InstallWorld(PE::World *w)
{
    this->SetWorld(w);
}

void MainWindow::UninstallWorld()
{
    this->SetWorld(nullptr);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_D && e->modifiers() & Qt::ShiftModifier)
    {
        this->on_actionDebug_AI_triggered();
    } else if (e->key() == Qt::Key_N && e->modifiers() & Qt::ShiftModifier)
    {
        this->on_actionNew_game_triggered();
        return;
    } else if (e->key() == Qt::Key_C && e->modifiers() & Qt::ShiftModifier)
    {
        this->on_actionShow_console_triggered();
        return;
    }

    this->game->GetWorld()->ProcessKeyPress(e->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    this->game->GetWorld()->ProcessKeyRelease(e->key());
}

void MainWindow::on_actionBots_enable_quick_aim_triggered()
{
    Game::AIQuickAim = !Game::AIQuickAim;
}

void MainWindow::on_actionNew_game_triggered()
{
    Game::CurrentGame->RequestScene(Scene_NewGame);
}

void MainWindow::on_actionExplosion_rocks_triggered()
{
    Game::ExplosionEffects = !Game::ExplosionEffects;
}

void MainWindow::on_actionFast_game_triggered()
{
    Game::SuperFast = !Game::SuperFast;
}

void MainWindow::on_actionShow_console_triggered()
{
    Console::ActiveConsole->show();
}

void MainWindow::on_actionDebug_AI_triggered()
{
    AITracer::Debug = !AITracer::Debug;
}

void MainWindow::on_actionFluid_terrain_triggered()
{
    Game::FastTerrainUpdates = !Game::FastTerrainUpdates;
}
