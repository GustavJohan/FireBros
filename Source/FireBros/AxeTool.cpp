// Fill out your copyright notice in the Description page of Project Settings.


#include "AxeTool.h"

#include "FireFighter.h"
#include "BreakableObject.h"
#include "Components/BoxComponent.h"

AAxeTool::AAxeTool()
{
	AxeHitBox = CreateDefaultSubobject<UBoxComponent>("hitBox");
	AxeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Axe Mesh");

	AxeHitBox->SetupAttachment(AxeMeshComponent);

	//AxeMeshComponent->OnComponentHit.AddDynamic(this, &ATool::DisablePhysicsSimulation);
}


void AAxeTool::UseToolToServer_Implementation()
{
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Tool Used on server");
	TArray<AActor*> hitObjs;
	
	AxeHitBox->GetOverlappingActors(hitObjs);

	for (AActor* HitObj : hitObjs)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Green, "break");
		if (HitObj->IsA<ABreakableObject>())
		{
			//Cast<ABreakableObject>(HitObj)->BreakObjectToServer_Implementation()
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Cyan, "break axe to server");
			//HitObj->SetOwner(GetController());
			//BreakObjectRPCToServerFromFireFighter(Cast<ABreakableObject>(HitObj));
			FVector direction = HitObj->GetActorLocation() - GetActorLocation();
			direction.Normalize();
			direction += FVector::UpVector;
			direction.Normalize();

			ABreakableObject* objectToBreak = Cast<ABreakableObject>(HitObj);

			objectToBreak->BreakObjectMulticast(direction);
			
		}
	}
}
