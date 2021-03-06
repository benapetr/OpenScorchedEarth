//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <PixelEngine/Graphics/peglwindow.h>

/*namespace Ui
{
    class MainWindow;
}*/

class QImage;

namespace PE
{
    class PEGLWidget;
    class QImageRenderer;
    class World;
}

class Game;

class MainWindow : public PE::PEGLWindow
{
        Q_OBJECT

    public:
        static MainWindow *Main;

        MainWindow();
        ~MainWindow() override;
        int GetWidth();
        int GetHeight();
        void InstallWorld(PE::World *w);
        void UninstallWorld();

    public slots:
        void keyPressEvent(QKeyEvent* e) override;
        void keyReleaseEvent(QKeyEvent* e) override;

    private slots:
        void on_actionBots_enable_quick_aim_triggered();
        void on_actionNew_game_triggered();
        void on_actionExplosion_rocks_triggered();
        void on_actionFast_game_triggered();
        void on_actionShow_console_triggered();
        void on_actionDebug_AI_triggered();
        void on_actionFluid_terrain_triggered();

    private:
        Game *game;
};

#endif // MAINWINDOW_H
