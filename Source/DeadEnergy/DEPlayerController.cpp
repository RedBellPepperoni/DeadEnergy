// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
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
  //  HudWidget->UpdateDoomlegCooldownIcon(Percent);
}

void ADEPlayerController::UpdateBoomerangIconHudWidget(const float Percent) noexcept
{
   // HudWidget->UpdateBoomerangCooldownIcon(Percent);
}

void ADEPlayerController::TeleportToSpawn() const noexcept
{
    UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorLocation(PlayerSpawnLocation);
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

void ADEPlayerController::ShowEndWidget() noexcept
{
    ShowWidgetAndPause(EndWidget);
}

void ADEPlayerController::HideEndWidget() noexcept
{
    HideWidgetAndUnpause(EndWidget);
}

void ADEPlayerController::ShowDeathWidget() noexcept
{
    ShowWidgetAndPause(DeathWidget);
}

void ADEPlayerController::HideDeathWidget() noexcept
{
    HideWidgetAndUnpause(DeathWidget);
        
}

void ADEPlayerController::BeginPlay()
{
    Super::BeginPlay();

    DecideSpawnLocation();
   
    CreateWidgets();

    HudWidget->AddToPlayerScreen();


}

void ADEPlayerController::CreateWidgets() noexcept
{

    HudWidget = CreateWidget<UPlayerHUD>(this, HudWidgetClass);

    if (DeathWidgetClass)
    {
        DeathWidget = CreateWidget<UUserWidget>(this, DeathWidgetClass);
    }

    if (EndWidgetClass)
    {
        EndWidget = CreateWidget<UUserWidget>(this, EndWidgetClass);
    }


}

void ADEPlayerController::ShowWidgetToFocus(UUserWidget* WidgetToShow) noexcept
{
    WidgetToShow->AddToPlayerScreen();
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}

void ADEPlayerController::HideFocusedWidget(UUserWidget* WidgetToHide) noexcept
{

    WidgetToHide->RemoveFromParent();
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
}

void ADEPlayerController::ShowWidgetAndPause(UUserWidget* WidgetToShow) noexcept
{
    ShowWidgetToFocus(WidgetToShow);
    UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void ADEPlayerController::HideWidgetAndUnpause(UUserWidget* WidgetToHide) noexcept
{
    HideFocusedWidget(WidgetToHide);
    UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void ADEPlayerController::DecideSpawnLocation() noexcept
{
    PlayerSpawnLocation = FVector(62.0, 307, 1065);
}

void ADEPlayerController::TeleportToSpawn()
{
}
