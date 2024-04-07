// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "DEPlayerController.h"


ADEGameModeBase::ADEGameModeBase() noexcept
{
}

void ADEGameModeBase::LevelSectionCompleted() noexcept
{
	ADEPlayerController* PlayerController = Cast<ADEPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PlayerController->HideTimeWidget();
	GetWorldTimerManager().ClearTimer(FloorTimer);
}

void ADEGameModeBase::LevelSectionStarted() noexcept
{
	ADEPlayerController* PlayerController = Cast<ADEPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PlayerController->ShowTimeWidget();
	CurrentTime = 0;
	GetWorldTimerManager().SetTimer(FloorTimer, [this]() { CurrentTime++; }, 0.01f, true);

}

FText ADEGameModeBase::TimeToText(int32 TimeInHundredthsOfSeconds) const noexcept
{
	uint32 HundredthsOfSeconds;
	uint32 TensOfHundredthsOfSeconds;
	uint32 Seconds;
	uint32 TensOfSeconds;
	uint32 Minutes;
	uint32 TensOfMinutes;
	HundredthsOfSeconds = TimeInHundredthsOfSeconds % 10;
	TensOfHundredthsOfSeconds = TimeInHundredthsOfSeconds % 100 / 10;
	Seconds = TimeInHundredthsOfSeconds % 1000 / 100;
	TensOfSeconds = TimeInHundredthsOfSeconds % 6000 / 1000;
	Minutes = TimeInHundredthsOfSeconds % 360000 / 6000;
	TensOfMinutes = TimeInHundredthsOfSeconds / 60000;
	const FString Time = FString::Printf(TEXT("%d%d:%d%d:%d%d"), TensOfMinutes, Minutes, TensOfSeconds, Seconds, TensOfHundredthsOfSeconds, HundredthsOfSeconds);
	return FText::FromString(Time);


}

FText ADEGameModeBase::GetCurrentTimeInText() const noexcept
{
	return TimeToText(CurrentTime);
}

void ADEGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADEGameModeBase::SaveScore() const noexcept
{
	

}

