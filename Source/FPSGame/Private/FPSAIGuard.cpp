// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Net/UnrealNetwork.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include <Blueprint/AIBlueprintHelperLibrary.h>


AFPSAIGuard::AFPSAIGuard()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnPawnHeard);
	GuardState = EAIState::Idle;

}

void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OrginalRotation = this->GetActorRotation();

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

void AFPSAIGuard::CheckDistanceToWaypoint()
{
	if (CurrentControlPoint)
	{
		FVector Delta = GetActorLocation() - CurrentControlPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		if (DistanceToGoal < 100)
		{
			MoveToNextPatrolPoint();
		}

	}
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn != nullptr)
	{
		DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Yellow, false, 10.0f);

		AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			GM->CompleteMission(SeenPawn, false);
		}
		SetGuardState(EAIState::Alerted);

		if (AController* Controller = GetController())
		{
			Controller->StopMovement();
		}

	}
}

void AFPSAIGuard::OnPawnHeard(APawn* SeenTarget, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}


	if (SeenTarget != nullptr)
	{
		DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);
	}


	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();

	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	FTimerManager& TimerManager = GetWorldTimerManager();

	TimerManager.ClearTimer(TimerHandle_ResetRotation);
	TimerManager.SetTimer(TimerHandle_ResetRotation, this, &AFPSAIGuard::ResetOrientation, 3.0f, false);

	SetGuardState(EAIState::Suspicious);

	if (AController* Controller = GetController())
	{
		Controller->StopMovement();
	}

}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	SetActorRotation(OrginalRotation);
	SetGuardState(EAIState::Idle);

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}

}

void AFPSAIGuard::MoveToNextPatrolPoint()
{
	if (CurrentControlPoint == nullptr || CurrentControlPoint == SecondControlPoint)
	{
		CurrentControlPoint = FirstControlPoint;
	}

	else
	{
		CurrentControlPoint = SecondControlPoint;
	}
	
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), CurrentControlPoint->GetActorLocation());
}

void AFPSAIGuard::SetGuardState(EAIState x)
{
	if (GuardState == x)
	{
		return;
	}

	GuardState = x;
	OnRep_GuardState();
}

void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckDistanceToWaypoint();


}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}