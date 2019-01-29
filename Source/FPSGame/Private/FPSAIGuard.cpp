// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include <GameFramework/Actor.h>
#include "TimerManager.h"
#include <NavigationSystem.h>
#include <AIController.h>
#include "BehaviorTree/BlackboardComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Engine/World.h"



// Sets default values
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



// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OrginalRotation = this->GetActorRotation();

	if (Waypoints.Num() > 1)
	{
		CurrentWaypointIndex = 0;

		FVector temp = Waypoints[0]->GetActorLocation();


		SetTargetLocation(temp);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Too few waypoints!"));
	}
}

void AFPSAIGuard::SetTargetLocation(const FVector& TargetLocation)
{
	FVector ProjectedLocation;

	if (UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), TargetLocation, ProjectedLocation, nullptr, TSubclassOf<UNavigationQueryFilter>()))
	{
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->GetBlackboardComponent()->SetValueAsVector(TEXT("MoveLocation"), TargetLocation);
		}
		else
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString(TEXT("[AFGEnemyCharacter::SetTargetLocation] Unable to project location on navigation.")));
		}


	}

}


void AFPSAIGuard::CheckDistanceToWaypoint()
{
	float distance = (GetActorLocation() - Waypoints[CurrentWaypointIndex]->GetActorLocation()).Size();

	if (distance < 1.0f)
	{
		if (CurrentWaypointIndex < Waypoints.Num())
		{
			CurrentWaypointIndex++;
			
		}
		else
		{
			CurrentWaypointIndex = 0;
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


}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	SetActorRotation(OrginalRotation);
	SetGuardState(EAIState::Idle);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
		return;

	GuardState = NewState;

	OnStateChanged(GuardState);


}


// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckDistanceToWaypoint();


}

