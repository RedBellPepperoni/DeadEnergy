// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (DoomLegIcon)
	{
		DoomLegMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), IconMaterialParent);
		DoomLegIcon->SetBrushFromMaterial(DoomLegMaterial);
	}

}

void UPlayerHUD::UpdateDoomlegCooldownIcon(const float Percent) noexcept
{
	if (DoomLegMaterial)
	{
		DoomLegMaterial->SetScalarParameterValue(ScalarParameterName, Percent);
	}
}

void UPlayerHUD::UpdateBoomerangCooldownIcon(const float Percent) noexcept
{
	if (BoomerangMaterial)
	{
		BoomerangMaterial->SetScalarParameterValue(ScalarParameterName, Percent);
	}
}
