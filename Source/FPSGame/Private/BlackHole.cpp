// Fill out your copyright notice in the Description page of Project Settings.

#include "BlackHole.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

class USphereComponent;

// Sets default values
ABlackHole::ABlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	SphereCompInner = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCompInner"));
	SphereCompInner->SetSphereRadius(100);
	SphereCompInner->SetupAttachment(MeshComp);

	SphereCompInner->OnComponentBeginOverlap.AddDynamic(this,&ABlackHole::OverlapInnerSphere);

	SphereCompOuter = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCompOuter"));
	SphereCompOuter->SetSphereRadius(3000);
	SphereCompOuter->SetupAttachment(MeshComp);


}

void ABlackHole::OverlapInnerSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	
	if(OtherActor) {
		OtherActor->Destroy();
	}

}

void ABlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> OverlappingComps;				//Creates an array to fill
	SphereCompOuter->GetOverlappingComponents(OverlappingComps);  //Fills our array with overlapping objects

	for (int32 i = 0; i < OverlappingComps.Num(); i++) {				//note normal for loop but  in C++ we need to define int32 and use the old school .num method instead of c# .length or .count class for List and arrays

		UPrimitiveComponent* PrimComp = OverlappingComps[i];  //Adds object from array to our current for search
		if (PrimComp->IsSimulatingPhysics() && PrimComp) {		//nullcheck, should not be needed but why not

			const float SphereRadius = SphereCompOuter->GetScaledSphereRadius();
			const float ForceStrength = -2000;

			PrimComp->AddRadialForce(GetActorLocation(), SphereRadius, ForceStrength, ERadialImpulseFalloff::RIF_Constant, true);

		}

	}





}


