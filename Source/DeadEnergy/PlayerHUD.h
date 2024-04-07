// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "PlayerHUD.generated.h"


/**
 * 
 */
UCLASS()
class DEADENERGY_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable) 
	void UpdateDoomlegCooldownIcon(const float Percent) noexcept; 
	void UpdateBoomerangCooldownIcon(const float Percent) noexcept;

protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* DoomLegIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* BoomerangIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* IconMaterialParent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* DoomLegMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* BoomerangMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ScalarParameterName = FName("Percent");

	
};
