// Fill out your copyright notice in the Description page of Project Settings.

// Spawnable LevelExit Marker to Decide what happens When the player
// reaches the end

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LevelExit.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADENERGY_API ULevelExit : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULevelExit();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
