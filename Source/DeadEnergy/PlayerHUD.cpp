// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UPlayerHUD::UpdateDoomlegCooldownIcon(const float Percent) noexcept
{
	DoomLegIcon->SetPercent(Percent);
}

void UPlayerHUD::UpdateBoomerangCooldownIcon(const float Percent) noexcept
{
	BoomerangIcon->SetPercent(Percent);
}
