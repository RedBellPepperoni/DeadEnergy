// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADEPlayer::ADEPlayer() noexcept
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	CameraTiltTimeline = CreateDefaultSubobject<UTimelineComponent>(FName("CameraTiltTimeline"));
	CameraTiltInterp.BindUFunction(this, FName("CameraTilt"));

	SlideSpeedTimeline = CreateDefaultSubobject<UTimelineComponent>(FName("SlideSpeedTimeline"));
	SlideSpeedInterp.BindUFunction(this, FName("Sliding"));


	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ADEPlayer::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ADEPlayer::OnBeginOverlap);
}

// Called when the game starts or when spawned
void ADEPlayer::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent = GetCharacterMovement();

	// Setting the default movement variables
	DefaultCrouchSpeed = MovementComponent->MaxWalkSpeedCrouched;
	DefaultAirControl = MovementComponent->AirControl;
	DefaultGravityScale = MovementComponent->GravityScale;
	DefaultAcceleration = MovementComponent->MaxAcceleration;

	// Adding interps to timelines
	if (CameraTiltCurve)
	{
		CameraTiltTimeline->AddInterpFloat(CameraTiltCurve, CameraTiltInterp, FName("Degrees"), FName("Tilt"));
	}

	if (SlideSpeedCurve)
	{
		SlideSpeedTimeline->AddInterpFloat(SlideSpeedCurve, SlideSpeedInterp, FName("Speed"), FName("SpeedTrack"));
	}

	
}

void ADEPlayer::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuls, const FHitResult& Hit) noexcept
{
	if (OtherComp->ComponentHasTag(WallRunTag) && CanWallRun(Hit.ImpactNormal))
	{
		// init wall run
		CurrentSide = FindRunSide(Hit.ImpactNormal);
		WallRunDirection = FindRunDirection(Hit.ImpactNormal, CurrentSide);
		BeginWallRun();
	}
	if (OtherComp->ComponentHasTag(KillBoxTag))
	{
		ADEPlayerController* PlayerController = Cast<ADEPlayerController>(GetController());
		PlayerController->ShowDeathWidget();
	}
}

void ADEPlayer::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) noexcept
{
	if (OtherComp->ComponentHasTag(JumpPadTag))
	{
		const float ForceMultiplier = 0.6f;
		const float LaunchForce = FMath::Abs(MovementComponent->Velocity.Z) * ForceMultiplier + JumpPadAdditionalForce;
		LaunchCharacter(FVector(0, 0, LaunchForce), false, true);
	}
}

void ADEPlayer::CrouchSlide() noexcept
{
	const double PlayerSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
	Crouch();
	if (PlayerSpeed >= SpeedNeededToSlide)
	{
		MovementComponent->MaxWalkSpeedCrouched = PlayerSpeed + SlideAdditionalSpeed;
		SlideSpeedDifference = MovementComponent->MaxWalkSpeedCrouched - DefaultCrouchSpeed;
		MovementComponent->AddImpulse(MovementComponent->Velocity.GetSafeNormal() * PlayerSpeed);
		SlideSpeedTimeline->PlayFromStart();
	}
	else
	{
		MovementComponent->MaxWalkSpeedCrouched = DefaultCrouchSpeed;
	}
}

void ADEPlayer::StopCrouching() noexcept
{
	UnCrouch();
	MovementComponent->MaxWalkSpeedCrouched = DefaultCrouchSpeed;
}

void ADEPlayer::Walk(const FInputActionValue& IAValue) noexcept
{
	const FVector2D MoveVector = IAValue.Get<FVector2D>();
	float XAxis = MoveVector.X;
	YWalkAxis = MoveVector.Y;
	if (bIsOnLadder)
	{
		if (XAxis != 0)
		{
			if (FVector::DotProduct(LadderForwardVector, GetActorRightVector() * XAxis) < 0)
			{
				AddMovementInput(GetActorUpVector(), 1);
			}
			else
			{
				AddMovementInput(GetActorUpVector(), -1);
			}
		}
		if (YWalkAxis != 0)
		{
			if (FVector::DotProduct(LadderForwardVector, GetActorForwardVector() * YWalkAxis) < 0)
			{
				AddMovementInput(GetActorUpVector(), 1);
			}
			else
			{
				AddMovementInput(GetActorUpVector(), -1);
			}
		}
	}
	else
	{
		if (bIsSlidingOff) // player has restricted move while sliding off
		{
			XAxis /= 10;
			YWalkAxis = 0;
		}
		AddMovementInput(GetActorRightVector(), XAxis); // moving in X-axis
		AddMovementInput(GetActorForwardVector(), YWalkAxis); // moving in Y-axis
	}
}

void ADEPlayer::Look(const FInputActionValue& IAValue) noexcept
{
	const FVector2D LookVector = IAValue.Get<FVector2D>();
	AddControllerYawInput(LookVector.X * MouseXSensitivity);
	AddControllerPitchInput(LookVector.Y * MouseYSensitivity * -1);
}

void ADEPlayer::InputJump() noexcept
{
	StopCrouching();
	if (bIsOnLadder)
	{
		uint32 LaunchFromLadderForce = 1000;
		LaunchCharacter(LadderForwardVector * LaunchFromLadderForce, false, false);
		ExittedLadder();
	}
	else if (bIsWallRunning)
	{
		LaunchCharacter(FindLaunchFromWallVelocity(), false, false);
		EndWallRun();
	}
	else if (bIsSlidingOff)
	{
		LaunchCharacter(FVector(0, 0, MovementComponent->JumpZVelocity), false, true);
	}
	else
	{
		Jump();
	}
}

void ADEPlayer::DoomLegJump() noexcept
{
	if (!bDoomLegOnCooldown)
	{
		bDoomLegOnCooldown = true;
		uint32 DoomLegForce;

		if (MovementComponent->IsFalling())
		{
			DoomLegForce = 2550; // Dash is stronger if player is not in air
		}
		else
		{
			DoomLegForce = 2700;
		}
		LaunchCharacter(GetControlRotation().Vector() * DoomLegForce, true, true);
		ADEPlayerController* PlayerController = Cast<ADEPlayerController>(GetController());
		GetWorldTimerManager().SetTimer(DoomLegRecharge, [this, PlayerController]() { PlayerController->UpdateDoomLegIconHudWidget((DoomLegCooldown - GetWorldTimerManager().GetTimerRemaining(ResetDoomLegCharge)) / DoomLegCooldown); }, 0.01f, true);
		GetWorldTimerManager().SetTimer(ResetDoomLegCharge, [this, PlayerController]() { GetWorldTimerManager().ClearTimer(DoomLegRecharge); PlayerController->UpdateDoomLegIconHudWidget(0.f); }, DoomLegCooldown, false);
		GetWorldTimerManager().SetTimer(DoomLegTimerHandle, [this]() {bDoomLegOnCooldown = false; }, DoomLegCooldown, false);
	}
}

void ADEPlayer::CrouchSlideStarted() noexcept
{
	if (MovementComponent->IsMovingOnGround())
	{
		CrouchSlide();
	}
	else
	{
		bHoldingCrouch = true;
	}
}

void ADEPlayer::CrouchSlideTriggered() noexcept
{
	if (bHoldingCrouch)
	{
		bHoldingCrouch = false;
		if (!MovementComponent->IsCrouching())
		{
			CrouchSlideStarted();
		}
	}
}

void ADEPlayer::CrouchSlideCompleted() noexcept
{
	if (!bIsSlidingOff)
	{
		UnCrouch();
		MovementComponent->MaxWalkSpeedCrouched = DefaultCrouchSpeed;
	}
}

void ADEPlayer::PauseCalled() noexcept
{
	ADEPlayerController* PlayerController = Cast<ADEPlayerController>(GetController());
	PlayerController->TogglePause();

}

void ADEPlayer::StopSlidingOff() noexcept
{
	MovementComponent->MaxWalkSpeedCrouched = DefaultCrouchSpeed;
	MovementComponent->MaxAcceleration = DefaultAcceleration;
	bIsSlidingOff = false;
	StopCrouching();
}

bool ADEPlayer::CanWallBeRunOn(const FVector& WallNormal) const noexcept
{
	const float WallAcceptedAngle = -0.05f;
	const double WallAngle = UKismetMathLibrary::DegAcos(FVector::DotProduct(WallNormal.GetSafeNormal2D(), WallNormal));
	const bool WallAngleRunnable = WallAngle < MovementComponent->GetWalkableFloorAngle();
	return WallNormal.Z >= WallAcceptedAngle && WallAngleRunnable;
}

bool ADEPlayer::CanWallRun(const FVector& SurfaceNormal) noexcept
{
	const uint32 MinVelocity = 200;
	const float PlayerVelocity = UKismetMathLibrary::Quat_UnrotateVector(FQuat(GetActorRotation()), GetVelocity()).X;
	return !bIsWallRunning && MovementComponent->IsFalling() && PlayerVelocity > MinVelocity && YWalkAxis >= 0 && CanWallBeRunOn(SurfaceNormal);
}

void ADEPlayer::BeginWallRun() noexcept
{
	MovementComponent->AirControl = 1;
	MovementComponent->GravityScale = 0;
	MovementComponent->SetPlaneConstraintNormal(FVector(0, 0, 1));
	bIsWallRunning = true;
	CameraTiltTimeline->Play();
	GetWorldTimerManager().SetTimer(WallRunTimer, this, &ADEPlayer::UpdateWallRun, 0.1f, true);
}

void ADEPlayer::EndWallRun() noexcept
{
	MovementComponent->AirControl = DefaultAirControl;
	MovementComponent->GravityScale = DefaultGravityScale;
	MovementComponent->SetPlaneConstraintNormal(FVector(0, 0, 0));
	bIsWallRunning = false;
	CameraTiltTimeline->Reverse();
	GetWorldTimerManager().ClearTimer(WallRunTimer);
}

void ADEPlayer::UpdateWallRun() noexcept
{
	FHitResult HitResult;
	FVector SideToTrace;
	switch (CurrentSide)
	{
	case Left:
		SideToTrace = FVector(0, 0, -1);
		break;

	case Right:
		SideToTrace = FVector(0, 0, 1);
		break;
	}
	const uint32 TraceLength = 200;
	const FVector TraceEnd = GetActorLocation() + FVector::CrossProduct(WallRunDirection, SideToTrace) * TraceLength;
	const bool bHitWall = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), TraceEnd, ECC_Visibility, FCollisionQueryParams(FName(NAME_None), false, this));
	if (bHitWall)
	{
		if (HitResult.Component->ComponentHasTag(WallRunTag) && FindRunSide(HitResult.ImpactNormal) == CurrentSide)
		{
			WallRunDirection = FindRunDirection(HitResult.ImpactNormal, CurrentSide);
			const FVector WallRunForce = WallRunDirection * MovementComponent->GetMaxSpeed();
			MovementComponent->Velocity = FVector(WallRunForce.X, WallRunForce.Y, 0);
		}
		else
		{
			EndWallRun();
		}
	}
	else
	{
		EndWallRun();
	}
}

void ADEPlayer::CameraTilt(float TimelineVal) const noexcept
{
	int32 CameraTiltSide;
	switch (CurrentSide)
	{
	case Left:
		CameraTiltSide = -1;
		break;

	case Right:
		CameraTiltSide = 1;
		break;
	}
	ADEPlayerController* PlayerController = Cast<ADEPlayerController>(GetController());
	const FRotator CurrentRoation = PlayerController->GetControlRotation();
	PlayerController->SetControlRotation(FRotator(CurrentRoation.Pitch, CurrentRoation.Yaw, TimelineVal * CameraTiltSide));
}

WallRunSide ADEPlayer::FindRunSide(const FVector& WallNormal) const noexcept
{
	const double DotResult = FVector2D::DotProduct(FVector2D(WallNormal), FVector2D(GetActorRightVector()));
	WallRunSide SideFound;
	if (DotResult < 0)
	{
		SideFound = Left;
	}
	else
	{
		SideFound = Right;
	}
	return SideFound;
}

FVector ADEPlayer::FindRunDirection(const FVector& WallNormal, WallRunSide Side) const noexcept
{
	FVector RunSide;
	switch (Side)
	{
	case Left:
		RunSide = FVector(0, 0, -1);
		break;

	case Right:
		RunSide = FVector(0, 0, 1);
		break;

	default:
		break;
	}
	return FVector::CrossProduct(WallNormal, RunSide);
}

FVector ADEPlayer::FindLaunchFromWallVelocity() const noexcept
{
	FVector SideToJumpFrom;
	switch (CurrentSide)
	{
	case Left:
		SideToJumpFrom = FVector(0, 0, 1);
		break;

	case Right:
		SideToJumpFrom = FVector(0, 0, -1);
		break;
	}
	const float LaunchForceMultiplier = 0.55f;
	const FVector AdditionalUpForce = FVector(0, 0, 200);
	return FVector((FVector::CrossProduct(WallRunDirection, SideToJumpFrom) + FVector(0, 0, 1)) * MovementComponent->JumpZVelocity * LaunchForceMultiplier + AdditionalUpForce);
}

void ADEPlayer::Sliding(float Speed) noexcept
{
	MovementComponent->MaxWalkSpeedCrouched -= Speed * SlideSpeedDifference;
	SlideSpeedDifference = MovementComponent->MaxWalkSpeedCrouched - DefaultCrouchSpeed;
	if (MovementComponent->IsCrouching())
	{
		if (MovementComponent->MaxWalkSpeedCrouched <= DefaultCrouchSpeed + 50)
		{
			MovementComponent->MaxWalkSpeedCrouched = DefaultCrouchSpeed;
			Crouch();
			SlideSpeedTimeline->Stop();
		}
	}
	else
	{
		SlideSpeedTimeline->Stop();
	}
}

// Called every frame
void ADEPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Slide off check
	const AActor* Floor = MovementComponent->CurrentFloor.HitResult.GetActor();
	const FVector ImpactNormal = MovementComponent->CurrentFloor.HitResult.ImpactNormal;
	if (Floor) // Checking if player should slide off the floor he is standing on or if he should stop sliding
	{
		float RelativePitchDegrees;
		float RelativeRollDegrees;
		UKismetMathLibrary::GetSlopeDegreeAngles(Floor->GetActorRightVector(), ImpactNormal, FVector(0, 0, 1), RelativePitchDegrees, RelativeRollDegrees);
		if (UKismetMathLibrary::Abs(RelativePitchDegrees) > SlidingOffAngle || UKismetMathLibrary::Abs(RelativeRollDegrees) > SlidingOffAngle)
		{
			if (!MovementComponent->IsCrouching())
			{
				CrouchSlide();
			}
			float WorldPitchDegrees;
			float WorldRollDegrees;
			const float GeneralAngle = UKismetMathLibrary::FMax(UKismetMathLibrary::Abs(RelativePitchDegrees), UKismetMathLibrary::Abs(RelativeRollDegrees));
			const float SlideOffSpeedMultiplier = 0.5f;
			const uint32 SlideOffAcceleration = 4000;
			UKismetMathLibrary::GetSlopeDegreeAngles(FVector(0, 1, 0), ImpactNormal, FVector(0, 0, 1), WorldPitchDegrees, WorldRollDegrees);
			bIsSlidingOff = true;
			MovementComponent->MaxWalkSpeedCrouched = DefaultCrouchSpeed * GeneralAngle * SlideOffSpeedMultiplier;
			MovementComponent->MaxAcceleration = SlideOffAcceleration;
			MovementComponent->AddInputVector(FVector(WorldPitchDegrees, WorldRollDegrees, GeneralAngle).GetSafeNormal() * -1, true);
		}
		else
		{
			if (bIsSlidingOff)
			{
				StopSlidingOff();
			}
		}
	}
	else
	{
		if (bIsSlidingOff)
		{
			StopSlidingOff();
		}
	}

}

// Called to bind functionality to input
void ADEPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (GetController())
	{
		BindController(GetController());
	}
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Binding inputs
		if (IAWalk)
		{
			PlayerEnhancedInputComponent->BindAction(IAWalk, ETriggerEvent::Triggered, this, &ADEPlayer::Walk); 
		}

		if (IAJump)
		{
			PlayerEnhancedInputComponent->BindAction(IAJump, ETriggerEvent::Started, this, &ADEPlayer::InputJump); 
		}

		if (IALook)
		{
			PlayerEnhancedInputComponent->BindAction(IALook, ETriggerEvent::Triggered, this, &ADEPlayer::Look); 
		}

		if (IADoomLegCharge)
		{
			PlayerEnhancedInputComponent->BindAction(IADoomLegCharge, ETriggerEvent::Started, this, &ADEPlayer::DoomLegJump);
		}

		if (IACrouchSlide)
		{
			PlayerEnhancedInputComponent->BindAction(IACrouchSlide, ETriggerEvent::Started, this, &ADEPlayer::CrouchSlideStarted); 
			PlayerEnhancedInputComponent->BindAction(IACrouchSlide, ETriggerEvent::Triggered, this, &ADEPlayer::CrouchSlideTriggered); 
			PlayerEnhancedInputComponent->BindAction(IACrouchSlide, ETriggerEvent::Completed, this, &ADEPlayer::CrouchSlideCompleted); 
		}


		if (IAPause)
		{
			PlayerEnhancedInputComponent->BindAction(IAPause, ETriggerEvent::Started, this, &ADEPlayer::PauseCalled); 
		}
	}

}

void ADEPlayer::BindController(AController* NewController) noexcept
{
	const APlayerController* PlayerController = Cast<ADEPlayerController>(NewController);
	// Adding Mapping Context
	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->GetLocalPlayer()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (!InputMappingContext.IsNull())
			{
				InputSystem->AddMappingContext(InputMappingContext.LoadSynchronous(), 1);
			}
		}
	}
}

void ADEPlayer::EnteredLadder(const FVector& LadderForward) noexcept
{
	if (bCanEnterLadder)
	{
		bCanEnterLadder = false;
		LadderForwardVector = LadderForward;
		bIsOnLadder = true;
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
		MovementComponent->StopMovementImmediately();
	}
}

void ADEPlayer::ExittedLadder() noexcept
{
	bIsOnLadder = false;
	
	MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	
	const float LadderCooldown = 0.2f;
	GetWorldTimerManager().SetTimer(LadderCooldownHandle, [this]() { bCanEnterLadder = true; }, LadderCooldown, false);
}

void ADEPlayer::ExitLadderBoost() noexcept
{
	const uint32 LaunchForce = 50;
	LaunchCharacter(GetActorUpVector() * LaunchForce, false, false);
}

void ADEPlayer::OnDestroyed() noexcept
{
}

