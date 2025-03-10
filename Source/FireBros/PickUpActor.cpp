// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpActor.h"

// Sets default values
APickUpActor::APickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();

	ObjectMesh = GetComponentByClass<UStaticMeshComponent>();

	if (HasAuthority())
	{
		ObjectMesh->SetSimulatePhysics(true);
	}
}

// Called every frame
void APickUpActor::Tick(float DeltaTime)
{
	if (pickedUp && HasAuthority())
 	{
	Super::Tick(DeltaTime);

	
		ObjectMesh->SetWorldLocation(RootComponent->GetComponentToWorld().GetLocation(), true);
		ObjectMesh->SetWorldRotation(RootComponent->GetComponentToWorld().Rotator());
	}
}

void APickUpActor::pickupActor()
{
	if (HasAuthority())
 	{
	pickedUp = true;
	
		ObjectMesh->SetCollisionProfileName("IgnoreOnlyPawn");
		ObjectMesh->SetEnableGravity(false);
	}
}

void APickUpActor::discardActor_Implementation()
{
	if (true)
 	{
	pickedUp = false;
	
	
		ObjectMesh->SetEnableGravity(true);
		ObjectMesh->SetCollisionProfileName("PhysicsActor");
	}
}

void APickUpActor::throwActor_Implementation(FVector impulseDirection)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "adding impulse: " + impulseDirection.ToString());
	if (true)
	{
		ObjectMesh->AddImpulse(impulseDirection);
	}
}
