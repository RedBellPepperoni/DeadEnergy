// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSection.h"

// Sets default values
ALevelSection::ALevelSection() noexcept
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	DefaultRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));

	SetRootComponent(DefaultRootComp);
	LevelExit = CreateDefaultSubobject<ULevelExit>(TEXT("Exit"));
	LevelExit->SetupAttachment(RootComponent);
}

const int ALevelSection::GetLeftTurnCount() const noexcept
{
	return LeftTurns;
}

const int ALevelSection::GetRightTurnCount() const noexcept
{
	return RightTurns;
}

FTransform ALevelSection::GetExitTransform() const noexcept
{
	return LevelExit->GetComponentTransform();
}



