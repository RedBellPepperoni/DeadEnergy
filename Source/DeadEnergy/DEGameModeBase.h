// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "LevelSection.h"
#include "DEGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DEADENERGY_API ADEGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	ADEGameModeBase() noexcept;

	UFUNCTION(BlueprintCallable, Category = "LeveLSection")
	void LevelSectionCompleted() noexcept;

	UFUNCTION(BlueprintCallable, Category = "LeveLSection")
	void LevelSectionStarted() noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rooms")
	FText TimeToText(int32 TimeInHundredthsOfSeconds) const noexcept; // Converting time in hundredths of seconds to text in format mm:ss:hh

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rooms")
	FText GetCurrentTimeInText() const noexcept;




protected:
	virtual void BeginPlay() override;

private:
	void SaveScore() const noexcept;


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Rooms");
	uint32 NumberOfRoomsToSpawn = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Rooms");
	TArray<TSubclassOf<ALevelSection>>RoomsClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Rooms");
	TSubclassOf<ALevelSection> FinalRoomClass;

	uint32 CurrentTime = 0; // Current run time in hundredths of second
	uint32 CurrentFloorBeat = 0; // Current run time in hundredths of second
	

	TArray<ALevelSection*> SpawnedRooms;

	FTransform LastRoomExitTransform;
	FTimerHandle FloorTimer;
};
