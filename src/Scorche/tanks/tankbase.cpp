//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "tankbase.h"
#include "../bots/ai.h"
#include "../console.h"
#include "../bots/terminator.h"
#include "../weapons/cannon.h"
#include "../weapons/bigcannon.h"
#include "../weapons/mininuke.h"
#include "../weapons/nuke.h"
#include "../weapons/triplecanon.h"
#include "../playerinfo.h"
#include "../weaponlist.h"
#include "../game.h"
#include <cmath>
#include <PixelEngine/camera.h>
#include <PixelEngine/engine.h>
#include <PixelEngine/Physics/circlecollider.h>
#include <PixelEngine/definitions.h>
#include <PixelEngine/resources.h>
#include <PixelEngine/Graphics/renderer.h>
#include <PixelEngine/Physics/rigidbody.h>

#define SHIELD_RADIUS 22

QList<TankBase*> TankBase::Players;
bool TankBase::ControlsFrozen = false;
TankBase *TankBase::ActivePlayer = nullptr;
TankBase *TankBase::PlayerTank = nullptr;

void TankBase::ResetPlayers()
{
    Players.clear();
    ActivePlayer = nullptr;
    PlayerTank = nullptr;
    ControlsFrozen = false;
}

int TankBase::LivingPlayers()
{
    int result = 0;
    foreach (TankBase *player, Players)
    {
        if (player->IsAlive())
            result++;
    }
    return result;
}

TankBase *TankBase::GetActivePlayer()
{
    if (ActivePlayer == nullptr)
    {
        if (PlayerTank->IsAlive())
        {
            ActivePlayer = PlayerTank;
            return PlayerTank;
        } else
        {
            foreach (TankBase *player, Players)
            {
                if (player->IsAlive())
                {
                    ActivePlayer = player;
                    return player;
                }
            }
        }
        return nullptr;
    }

    return ActivePlayer;
}

TankBase *TankBase::RotatePlayers()
{
    TankBase *activePlayer = GetActivePlayer();
    // Check if there are any players alive
    if (activePlayer == nullptr)
        return nullptr;

    int index = Players.indexOf(activePlayer);
    int tries = 0;
    while (tries++ < Players.size())
    {
        if (++index >= Players.size())
        {
            index = 0;
            TankBase::FinishRound();
        }
        activePlayer = Players.at(index);
        if (activePlayer->IsAlive())
        {
            ActivePlayer = activePlayer;
            return activePlayer;
        }
    }
    return nullptr;
}

TankBase *TankBase::GetUnspawned()
{
    foreach (TankBase *player, Players)
    {
        if (!player->IsSpawned)
        {
            return player;
        }
    }
    return nullptr;
}

void TankBase::FinishRound()
{
    Console::Append("Round finished - giving $20 to all survivors");
    foreach (TankBase *player, Players)
    {
        if (player->IsAlive())
            player->GetPlayer()->IncreaseMoney(20);
    }
}

TankBase::TankBase(double x, double y, PlayerInfo *player)
{
    Players.append(this);
    this->playerInfo = player;
    this->tankColor = player->Color;
    this->PlayerName = player->PlayerName;

    this->Position.X = x;
    this->Position.Y = y;

    this->RigidBody = new PE::Rigidbody();
    this->RigidBody->Weight = 0.1;

    this->SelectedWeapon = new Cannon(this);
}

TankBase::~TankBase()
{
    Players.removeOne(this);
    delete this->SelectedWeapon;
    delete this->ai;
}

PlayerInfo *TankBase::GetPlayer()
{
    return this->playerInfo;
}

void TankBase::Fire()
{
    if (this->SelectedWeapon->GetWeaponType() != 0 && this->SelectedWeapon->Ammo <= 0)
    {
        Console::Append("Not enough ammo to fire this weapon :(");
        return;
    }

    if (TankBase::ControlsFrozen)
        return;

    if (this->Power > 100)
        this->Power = 100;

    if (this->SelectedWeapon == nullptr)
        return;

    TankBase::ControlsFrozen = true;

    this->DisableShield();
    this->SelectedWeapon->Fire(this->getCanonB(this->Position), this->GetCanonAngle(), this->Power);
    this->ResetCanonAdjust();
}

void TankBase::Pass()
{
    this->ResetCanonAdjust();
    RotatePlayers();
}

bool TankBase::CanPlay()
{
    if (Game::CurrentGame->IsPaused)
        return false;

    if (Game::CurrentGame->DynamicTerrain && Game::CurrentGame->TerrainNeedsUpdate)
        return false;

    return true;
}

void TankBase::Update(qint64 time)
{
    (void)time;

    // Workaround for a bug in collision optimizer
    this->RigidBody->GroundCollider = nullptr;

    if (!this->IsSpawned)
        return;

    if (!this->BotInitialized)
        this->InitializeBot();

    if (!this->CanPlay())
        return;

    if (Game::CurrentGame->WarmingTanks > 0)
        return;

    if (!this->IsAlive())
    {
        if (GetActivePlayer() == this)
        {
            RotatePlayers();
        }
        return;
    }

    //! \todo This needs to be removed once fixed
    if (this->shieldCollider != nullptr)
        this->shieldCollider->Position = this->GetCanonRoot(this->Position);

    if (!TankBase::ControlsFrozen && this->IsAlive() && this->ai != nullptr && GetActivePlayer() == this)
        this->ai->Process();

    if (this->canonAdjust == 0 && this->powerAdjust == 0)
        return;
    this->canonAngle += this->canonAdjust;
    this->Power += this->powerAdjust;
    if (this->Power < 1)
        this->Power = 1;
    if (this->Power > 100)
        this->Power = 100;
    this->RedrawNeeded = true;
}

void TankBase::Event_KeyPress(int key)
{
    if (Game::CurrentGame->IsFinished)
    {
        switch (key)
        {
            case Qt::Key_N:
                Game::CurrentGame->RequestScene(Scene_Game);
                return;
            case Qt::Key_Space:
                Game::CurrentGame->RequestScene(Scene_Inventory);
                return;
        }
        return;
    }
    if (this->WarmingUp)
    {
        switch (key)
        {
            case Qt::Key_L:
                this->DeployShield(ShieldMini);
                this->Warm();
                break;
            case Qt::Key_H:
                this->DeployShield(ShieldHeavy);
                this->Warm();
                break;
            case Qt::Key_Space:
                this->Warm();
                break;
        }
        return;
    }
    if (TankBase::ControlsFrozen)
        return;

    if (!this->IsPlayer)
        return;

    if (PlayerTank != GetActivePlayer())
        return;

    switch (key)
    {
        case Qt::Key_L:
            this->DeployShield(ShieldMini);
            break;
        case Qt::Key_H:
            this->DeployShield(ShieldHeavy);
            break;
        case Qt::Key_A:
        case Qt::Key_Left:
            this->SetCanonAdjustLeft();
            return;
        case Qt::Key_D:
        case Qt::Key_Right:
            this->SetCanonAdjustRight();
            return;
        case Qt::Key_Up:
        case Qt::Key_W:
            this->powerAdjust = 1;
            return;
        case Qt::Key_S:
        case Qt::Key_Down:
            this->powerAdjust = -1;
            return;
        case Qt::Key_Space:
            this->Fire();
            return;
        case Qt::Key_1:
            this->SwitchWeapon(WEAPON_CANON);
            break;
        case Qt::Key_2:
            this->SwitchWeapon(WEAPON_BIG_CANON);
            break;
        case Qt::Key_3:
            this->SwitchWeapon(WEAPON_TRIPLE_CANON);
            break;
        case Qt::Key_4:
            this->SwitchWeapon(WEAPON_MINI_NUKE);
            break;
        case Qt::Key_5:
            this->SwitchWeapon(WEAPON_NUKE);
            break;
    }
}

void TankBase::Event_KeyRelease(int key)
{
    if (!this->IsPlayer)
        return;

    switch (key)
    {
        case Qt::Key_A:
        case Qt::Key_D:
        case Qt::Key_Left:
        case Qt::Key_Right:
            this->canonAdjust = 0;
            return;
        case Qt::Key_Up:
        case Qt::Key_W:
        case Qt::Key_S:
        case Qt::Key_Down:
            this->powerAdjust = 0;
            return;
    }
}

double TankBase::GetCanonAngleDegree()
{
    // Simple radian to degree conversion
    return ((this->canonAngle * PE_PI_RAD_CNV) * PE_RAD_DEG_CNV) - 90;
}

double TankBase::GetCanonAngle()
{
    if (this->canonAngle < 0)
        this->canonAngle = 0;

    if (this->canonAngle > PE_PI_RAD_CNV)
        this->canonAngle = PE_PI_RAD_CNV;

    return this->canonAngle;
}

void TankBase::Render(PE::Renderer *r, PE::Camera *c)
{
    if (!this->IsSpawned)
        return;

    // Get position to render on
    PE::Vector position = c->ProjectedPosition(this->Position);
    position.Y += 50;

    if (!this->PlayerName.isEmpty())
    {
        // Render name information
        r->DrawText(position.X2int(), position.Y2int(), this->PlayerName, QColor("black"));
    }

    // Render HP information
    r->DrawText(position.X2int(), position.Y2int() - 20, QString::number(this->Health), QColor("black"));

    if (GetActivePlayer() == this)
    {
        r->DrawBitmap(position.X2int(), position.Y2int() + 30, 20, 30, PE::Resources::GetPixmap(":/textures/terrain/arrow.png"));
        if (!ControlsFrozen && this->playerInfo->IsBot)
        {
            // Bot is thinking
            r->DrawBitmap(position.X2int() + 20, position.Y2int(), 40, 30, PE::Resources::GetPixmap(":/textures/ui/thinking.png"));
        }
    }

    if (this->WarmingUp)
    {
        int x = r->GetWidth() / 2 - 150;
        int y = r->GetHeight() / 2 - 100;
        int width = 300;
        int height = 200;
        r->DrawRect(x, y, 300, 200, 2, QColor("white"), true);
        r->DrawRect(x, y, 300, 200, 2, QColor("black"), false);
        r->DrawText(x + width / 2 - 50, y + height - 60, "Warm up", QColor("black"), 20);
        if (this->playerInfo->ItemList[INVENTORY_SHIELD] > 0)
            r->DrawText(x + width / 2 - 80, y + height - 100, "Press L to set up shield", QColor("black"), 12);
        if (this->playerInfo->ItemList[INVENTORY_HEAVY_SHIELD] > 0)
            r->DrawText(x + width / 2 - 100, y + height - 120, "Press H to set up heavy shield", QColor("black"), 12);
        r->DrawText(x + width / 2 - 80, y + height - 180, "Press space to continue", QColor("black"), 12);
    }

    //PE::Collider::Debug = true;
    //return;

    double shield_radius = SHIELD_RADIUS;
    position.Y -= 50;
    PE::Vector sp = this->GetCanonRoot(position) - shield_radius;

    // Shields
    if (this->ShieldPower > 0)
    {
        int shield_t = 1;
        switch (this->Shield)
        {
            case ShieldHeavyForce:
            case ShieldHeavy:
                shield_t = 4;
                break;
        }
        r->DrawEllipse(sp.X2int(), sp.Y2int(), shield_radius * 2, shield_radius * 2, this->GetShieldColor(), shield_t);
    }
}

void TankBase::TakeDamage(TankBase *source, double damage)
{
    if (!this->IsAlive())
        return;

    if (this->ShieldPower > 0)
    {
        this->ShieldPower -= damage;
        if (this->ShieldPower < 0)
        {
            this->ShieldPower = 0;
            this->RemoveChildren(this->shieldCollider);
            this->shieldCollider = nullptr;
            this->Shield = ShieldNone;
        }
        return;
    }

    this->Health -= damage;
    if (this->Health < 0)
        this->Health = 0;

    if (source != this)
        source->playerInfo->IncreaseMoney(static_cast<int>(damage));

    if (!this->IsAlive())
        this->Kill(source);
}

void TankBase::Kill(TankBase *source)
{
    //this->playerInfo->IncreaseMoney(-200);
    this->playerInfo->Deaths_Total++;
    if (source == this)
    {
        Console::Append(source->PlayerName + " killed himself - LOL");
    } else
    {
        Console::Append(source->PlayerName + " just nuked " + this->PlayerName + " :O");
        source->playerInfo->IncreaseMoney(1000);
        source->playerInfo->Score += 2000;
        source->playerInfo->Kills_Total++;
    }

    // Remove all colliders
    foreach (PE::Collider *c, this->colliders)
        c->DestroyNow();

    this->colliders.clear();
}

bool TankBase::IsAlive()
{
    return (this->Health > 0);
}

bool TankBase::CheckCollision(const PE::Vector &point)
{
    foreach (PE::Collider *c, this->colliders)
    {
        if (c->PositionMatch(point))
            return true;
    }
    return false;
}

void TankBase::InitializeBot()
{
    this->BotInitialized = true;
    if (this->playerInfo->IsBot)
    {
        if (this->playerInfo->AI == "terminator")
            this->ai = new Terminator(this);
        else
            this->ai = new AI(this);
        this->ai->ProcessInventory();
        this->ai->WarmUp();
        this->WarmingUp = false;
        Game::CurrentGame->WarmingTanks--;
    } else
    {
        // Check if this player has anything to use in warm up
        if (this->playerInfo->ItemList[INVENTORY_SHIELD] <= 0 &&
            this->playerInfo->ItemList[INVENTORY_HEAVY_SHIELD] <= 0)
        {
            this->WarmingUp = false;
            Game::CurrentGame->WarmingTanks--;
        }
    }
}

void TankBase::SetCanonAdjustLeft()
{
    this->canonAdjust = 0.006;
}

void TankBase::SetCanonAdjustRight()
{
    this->canonAdjust = -0.006;
}

void TankBase::ResetCanonAdjust()
{
    this->canonAdjust = 0;
}

void TankBase::SetPower(double p)
{
    this->Power = p;
    if (this->Power > 100)
        this->Power = 100;
    if (this->Power < 0)
        this->Power = 0;
}

void TankBase::SetAngle(double a)
{
    this->canonAngle = a;
    if (this->canonAngle < 0)
        this->canonAngle = 0;

    if (this->canonAngle > PE_PI_RAD_CNV)
        this->canonAngle = PE_PI_RAD_CNV;
}

void TankBase::IncreasePower(double p)
{
    this->SetPower(this->Power + p);
}

void TankBase::SwitchWeapon(int id)
{
    if (id != 0 && this->playerInfo->ItemList[id] <= 0)
        return;

    switch(id)
    {
        case WEAPON_NUKE:
            delete this->SelectedWeapon;
            this->SelectedWeapon = new Nuke(this);
            break;
        case WEAPON_CANON:
            delete this->SelectedWeapon;
            this->SelectedWeapon = new Cannon(this);
            break;
        case WEAPON_BIG_CANON:
            delete this->SelectedWeapon;
            this->SelectedWeapon = new BigCannon(this);
            break;
        case WEAPON_MINI_NUKE:
            delete this->SelectedWeapon;
            this->SelectedWeapon = new MiniNuke(this);
            break;
        case WEAPON_TRIPLE_CANON:
            delete this->SelectedWeapon;
            this->SelectedWeapon = new TripleCanon(this);
            break;
    }

    this->RedrawNeeded = true;
}

void TankBase::DeployShield(ShieldType shield)
{
    int shield_type;
    double max_power;
    switch (shield)
    {
        case ShieldMini:
            shield_type = INVENTORY_SHIELD;
            max_power = SHIELD_LIGHT_MAX_HP;
            break;
        case ShieldHeavy:
            shield_type = INVENTORY_HEAVY_SHIELD;
            max_power = SHIELD_HEAVY_MAX_HP;
            break;
        default:
            return;
    }
    if (this->playerInfo->ItemList[shield_type] <= 0)
        return;
    if (this->shieldCollider != nullptr)
        this->RemoveChildren(this->shieldCollider);
    PE::Vector position = this->GetCanonRoot(this->Position);
    this->shieldCollider = new PE::CircleCollider(position.X, position.Y, SHIELD_RADIUS);
    this->AddChildren(this->shieldCollider);
    this->RedrawNeeded = true;
    this->playerInfo->ItemList[shield_type]--;
    this->Shield = shield;
    this->ShieldPower = max_power;
}

void TankBase::DisableShield()
{
    if (this->ShieldDisabled)
        return;
    if (this->shieldCollider != nullptr)
        this->RemoveChildren(this->shieldCollider);
    this->ShieldDisabled = true;
}

void TankBase::RestoreShield()
{
    if (!this->ShieldDisabled)
        return;
    if (this->shieldCollider != nullptr)
    {
        this->shieldCollider->Position = this->GetCanonRoot(this->Position);
        this->AddChildren(this->shieldCollider);
    }
    this->ShieldDisabled = false;
}

QColor TankBase::GetShieldColor()
{
    double max_hp;
    QColor base;
    switch(this->Shield)
    {
        case ShieldMini:
            base = QColor(255, 255, 255);
            max_hp = SHIELD_LIGHT_MAX_HP;
            break;
        case ShieldHeavy:
            base = QColor(255, 255, 255);
            max_hp = SHIELD_HEAVY_MAX_HP;
            break;
    }
    double result_color_r = (static_cast<double>(base.red()) / max_hp) * this->ShieldPower;
    double result_color_g = (static_cast<double>(base.green()) / max_hp) * this->ShieldPower;
    double result_color_b = (static_cast<double>(base.blue()) / max_hp) * this->ShieldPower;
    return QColor(static_cast<int>(result_color_r), static_cast<int>(result_color_g), static_cast<int>(result_color_b));
}

void TankBase::Warm()
{
    this->WarmingUp = false;
    Game::CurrentGame->WarmingTanks--;
    this->RedrawNeeded = true;
}

PE::Vector TankBase::getCanonB(const PE::Vector &source)
{
    // angle math
    double radians = this->GetCanonAngle() * PE_PI_RAD_CNV;
    PE::Vector result = this->GetCanonRoot(source);
    result.X += (12 * cos(radians));
    result.Y += (12 * sin(radians));
    return result;
}
