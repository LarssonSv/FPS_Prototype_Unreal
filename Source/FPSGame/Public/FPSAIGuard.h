// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include <Engine/TargetPoint.h>
#include "AIController.h"
#include "FPSAIGuard.generated.h"


class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8 {

	Idle,

	Suspicious,

	Alerted

};


UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

	UPROPERTY(EditAnywhere)
		TArray<ATargetPoint*> Waypoints;

	UFUNCTION()
		void SetTargetLocation(const FVector& TargetLocation);

	UFUNCTION()
		void CheckDistanceToWaypoint();


private:
	int CurrentWaypointIndex;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
		void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
		void OnPawnHeard(APawn* SeenTarget, const FVector& Location, float Volume);

	UPROPERTY(BlueprintReadOnly, Category = "Oringal Rotation")
		FRotator OrginalRotation;

	UFUNCTION()
		void ResetOrientation();

	FTimerHandle TimerHandle_ResetRotation;

	EAIState GuardState;

	UFUNCTION()
		void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = AI)
		void OnStateChanged(EAIState NewState);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};

