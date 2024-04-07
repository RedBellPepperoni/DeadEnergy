// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Exit Scene Component
#include "LevelExit.h"

#include "LevelSection.generated.h"


UCLASS()
class DEADENERGY_API ALevelSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelSection() noexcept;

	// Get the Number of LeftTurns in the Section
	UFUNCTION(BlueprintCallable)
	const int GetLeftTurnCount() const noexcept;

	// Get the Number of RightTurns in the Section
	UFUNCTION(BlueprintCallable)
	const int GetRightTurnCount() const noexcept;

	FTransform GetExitTransform() const noexcept;

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// ============== Varibales =================
protected:

	UPROPERTY(EditDefaultsOnly)
	int LeftTurns;

	UPROPERTY(EditDefaultsOnly)
	int RightTurns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULevelExit* LevelExit;


private:
	UPROPERTY()
	USceneComponent* DefaultRootComp;


};
