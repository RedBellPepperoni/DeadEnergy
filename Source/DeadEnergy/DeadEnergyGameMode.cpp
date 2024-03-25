// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeadEnergyGameMode.h"
#include "DeadEnergyCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADeadEnergyGameMode::ADeadEnergyGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
