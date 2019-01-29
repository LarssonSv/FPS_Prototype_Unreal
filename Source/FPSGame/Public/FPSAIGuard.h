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

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState;



	void MoveToNextPatrolPoint();

	UPROPERTY(EditInstanceOnly, Category = "AI")
	bool bPatrol;

	void SetGuardState(EAIState x);

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
		AActor* FirstControlPoint;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	AActor* SecondControlPoint;

	AActor* CurrentControlPoint;

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState x);

	UFUNCTION()
	void OnRep_GuardState();


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
};

