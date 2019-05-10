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
#include "engine.h"
#include "ui_mainwindow.h"
#include "qimagerenderer.h"
#include "game.h"
#include <QImage>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    PE::Engine::Initialize();
    ui->setupUi(this);
    this->se_renderer = new PE::QImageRenderer(this->GetWidth(), this->GetHeight());
    this->qimage = this->se_renderer->GetImage();
    this->game = new Game(this->GetWidth(), this->GetHeight(), this->se_renderer);
    this->Render();
    this->renderTimer = new QTimer(this);
    connect(this->renderTimer, SIGNAL(timeout()), this, SLOT(OnRender()));
    this->renderTimer->start(10);
}

MainWindow::~MainWindow()
{
    delete this->game;
    delete this->se_renderer;
    delete ui;
}

void MainWindow::Render()
{
    this->ui->viewPort->setPixmap(this->se_renderer->GetPixmap());
}

int MainWindow::GetWidth()
{
    //return this->ui->viewPort->width() * QApplication::desktop()->devicePixelRatio() * 2;
    return 1024;
}

int MainWindow::GetHeight()
{
    //return this->ui->viewPort->height() * QApplication::desktop()->devicePixelRatio() * 2;
    return 768;
}

void MainWindow::OnRender()
{
    if (this->se_renderer->HasUpdate)
        this->ui->viewPort->setPixmap(this->se_renderer->GetPixmap());
}
