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
#include "ui_mainwindow.h"
#include "projectiles/aitracer.h"
#include "game.h"
#include "staticassets.h"
#include "console.h"
#include <QKeyEvent>
#include <QDateTime>
#include <QImage>
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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    Main = this;
    PE::Engine::Initialize(false);
    ui->setupUi(this);
    this->showMaximized();
    StaticAssets::Instance = new StaticAssets();
#ifndef SCORCHE_GL
    this->fps = 0;
    this->fps_current = 0;
    this->fps_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    this->se_renderer = new PE::QImageRenderer(this->GetWidth(), this->GetHeight());
    this->se_renderer->ManualUpdate = true;
    this->qimage = this->se_renderer->GetImage();
#else
    this->viewPort = new PE::PEGLWidget(this, nullptr);
    this->layout()->removeWidget(this->ui->viewPort);
    delete this->ui->viewPort;
    this->ui->viewPort = nullptr;
    this->layout()->addWidget(this->viewPort);
#endif
    this->game = new Game(this->GetWidth(), this->GetHeight());
    //this->Render();
    this->renderTimer = new QTimer(this);
    connect(this->renderTimer, SIGNAL(timeout()), this, SLOT(OnRender()));
    // this timer speed defines FPS, smaller means higher FPS, but also more CPU usage
#ifdef __EMSCRIPTEN__
    // WASM is significantly worse than native, so let's decrease FPS to put less strain on CPU
    this->renderTimer->start(40);
#else
    this->renderTimer->start(20);
#endif

    // Init console
    foreach (PE::RingLog_Item item, PE::Engine::GetEngine()->RL->GetItems())
    {
        Console::Append(item.GetText());
    }

    this->ui->actionFast_game->setChecked(Game::SuperFast);

    new Console(this);
#ifdef __EMSCRIPTEN__
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
#endif
}

MainWindow::~MainWindow()
{
    delete this->game;
#ifndef SCORCHE_GL
    delete this->se_renderer;
#else
    delete this->viewPort;
#endif
    delete ui;
    delete StaticAssets::Instance;
    StaticAssets::Instance = nullptr;
}

#define SAMPLE_RATE 2000

void MainWindow::Render()
{
#ifndef SCORCHE_GL
    qint64 current_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (current_time - fps_start > SAMPLE_RATE)
    {
        this->fps = static_cast<double>(this->fps_current) / (SAMPLE_RATE / 1000);
        this->real_fps = static_cast<double>(this->rc_fps) / (SAMPLE_RATE / 1000);
        this->fps_current = 0;
        this->rc_fps = 0;
        this->fps_start = current_time;
    }
    this->fps_current++;
    this->game->GetWorld()->Render(this->se_renderer);
    if (this->se_renderer->HasUpdate)
    {
        this->rc_fps++;
        this->ui->viewPort->setPixmap(this->se_renderer->GetPixmap());
    }
#endif
}

int MainWindow::GetWidth()
{
#ifndef SCORCHE_GL
    return this->ui->viewPort->width();// * QApplication::desktop()->devicePixelRatio();
#else
    return this->viewPort->width();
#endif
}

int MainWindow::GetHeight()
{
#ifndef SCORCHE_GL
    return this->ui->viewPort->height();// * QApplication::desktop()->devicePixelRatio();
#else
    return this->viewPort->height();
#endif
}

double MainWindow::GetFPS()
{
#ifdef SCORCHE_GL
   return this->viewPort->GetFPS();
#else
    return this->fps;
#endif
}

void MainWindow::InstallWorld(PE::World *w)
{
#ifdef SCORCHE_GL
    this->viewPort->SetWorld(w);
#endif
}

void MainWindow::UninstallWorld()
{
#ifdef SCORCHE_GL
    this->viewPort->SetWorld(nullptr);
#endif
}

void MainWindow::OnRender()
{
    if (!this->ui->actionRendering->isChecked())
        return;
#ifndef SCORCHE_GL
    this->Render();
#else
    this->viewPort->update();
#endif
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    this->game->GetWorld()->ProcessKeyPress(e->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    this->game->GetWorld()->ProcessKeyRelease(e->key());
}

void MainWindow::on_actionRendering_triggered()
{
#ifndef SCORCHE_GL
    this->se_renderer->Enabled = !this->se_renderer->Enabled;
#endif
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

void MainWindow::on_actionLow_FPS_triggered(bool checked)
{
    this->renderTimer->stop();
    if (!checked)
        this->renderTimer->start(20);
    else
        this->renderTimer->start(40);
}
