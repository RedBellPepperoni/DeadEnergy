// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerHUD.h"
#include "DEGameModeBase.h"
#include "DEPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class DEADENERGY_API ADEPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void TogglePause() noexcept;

	void UpdateDoomLegIconHudWidget(const float Percent) noexcept;
	void UpdateBoomerangIconHudWidget(const float Percent) noexcept;

	void TeleportToSpawn() const noexcept;
	FVector GetPlayerSpawnLocation() const noexcept;

	UFUNCTION(BlueprintCallable)
	void TeleportPlayer(const FVector Position);



	void ShowTimeWidget() noexcept;
	void HideTimeWidget() noexcept;

	void ShowDeathWidget() noexcept;
	void HideDeathWidget() noexcept;

protected:
	virtual void BeginPlay() override;

private:
	void CreateWidgets() noexcept;
	void ShowWidgetAndPause(UUserWidget* WidgetToShow) noexcept;
	void HideWidgetAndUnpause(UUserWidget* WidgetToHide) noexcept;
	void DecideSpawnLocation() noexcept;

	
	void TeleportToSpawn();
	



protected:
	// Widgets, blueprint children will set the widgets classes
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UPlayerHUD> HudWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> DeathWidgetClass;


private:

	FVector PlayerSpawnLocation;
	FVector TutorialStartLocation;
	FVector RegularStartLocation;

	UPROPERTY()
	UPlayerHUD* HudWidget;
	UPROPERTY()
	UUserWidget* PauseWidget;

};
