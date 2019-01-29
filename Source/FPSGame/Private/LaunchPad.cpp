// Fill out your copyright notice in the Description page of Project Settings.

#include "LaunchPad.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>



// Sets default values
ALaunchPad::ALaunchPad()
{
	PrimaryActorTick.bCanEverTick = true;

	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetBoxExtent(FVector(75, 75, 50));
	RootComponent = OverlapComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	//Bind to event
	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ALaunchPad::OverlapLaunchPad);

	launchForce = 1500.0f;
	launchPitch = 35.0f;



}

void ALaunchPad::OverlapLaunchPad(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	FRotator LaunchDirection = GetActorRotation();
	LaunchDirection.Pitch += launchPitch;
	FVector LaunchVelocity = LaunchDirection.Vector() * launchForce;

	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	if (OtherCharacter) {

		OtherCharacter->LaunchCharacter(LaunchVelocity, true, true);

		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PadEffect, GetActorLocation());

	}

	else if (OtherComp && OtherComp->IsSimulatingPhysics()) {

		OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);


	}

}




