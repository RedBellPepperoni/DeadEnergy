// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSection.h"

// Sets default values
ALevelSection::ALevelSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	DefaultRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));

	SetRootComponent(DefaultRootComp);
	LevelExit = CreateDefaultSubobject<ULevelExit>(TEXT("Exit"));
	LevelExit->SetupAttachment(RootComponent);
}

const uint32 ALevelSection::GetLeftTurnCount() const noexcept
{
	return LeftTurns;
}

const uint32 ALevelSection::GetRightTurnCount() const noexcept
{
	return RightTurns;
}

FTransform ALevelSection::GetExitTransform() const noexcept
{
	return LevelExit->GetComponentTransform();
}



