// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPlayerController.h"

void ADEPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADEPlayerController::TogglePause() noexcept
{
}

void ADEPlayerController::UpdateDoomLegIconHudWidget(const float Percent) noexcept
{
    HudWidget->UpdateDoomlegCooldownIcon(Percent);
}

void ADEPlayerController::UpdateBoomerangIconHudWidget(const float Percent) noexcept
{
    HudWidget->UpdateBoomerangCooldownIcon(Percent);
}

void ADEPlayerController::TeleportToSpawn() const noexcept
{
}

FVector ADEPlayerController::GetPlayerSpawnLocation() const noexcept
{
    return PlayerSpawnLocation;
}

void ADEPlayerController::TeleportPlayer(const FVector Position)
{
}

void ADEPlayerController::ShowTimeWidget() noexcept
{
}

void ADEPlayerController::HideTimeWidget() noexcept
{
}

void ADEPlayerController::ShowDeathWidget() noexcept
{
}

void ADEPlayerController::HideDeathWidget() noexcept
{
}

void ADEPlayerController::BeginPlay()
{
    Super::BeginPlay();

    DecideSpawnLocation();
   

    HudWidget->AddToPlayerScreen();


}

void ADEPlayerController::CreateWidgets() noexcept
{

    HudWidget = CreateWidget<UPlayerHUD>(this, HudWidgetClass);

    if (PauseWidgetClass)
    {
        PauseWidget = CreateWidget<UUserWidget>(this, PauseWidgetClass);
    }

}

void ADEPlayerController::ShowWidgetAndPause(UUserWidget* WidgetToShow) noexcept
{
}

void ADEPlayerController::HideWidgetAndUnpause(UUserWidget* WidgetToHide) noexcept
{
}

void ADEPlayerController::DecideSpawnLocation() noexcept
{
}

void ADEPlayerController::TeleportToSpawn()
{
}
