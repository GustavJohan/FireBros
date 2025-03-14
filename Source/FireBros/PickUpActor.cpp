// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpActor.h"

#include "Net/UnrealNetwork.h"

// Sets default values
APickUpActor::APickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APickUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickUpActor, pickedUp);
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
	Super::Tick(DeltaTime);
	if (pickedUp && HasAuthority())
 	{
		ObjectMesh->SetWorldLocation(RootComponent->GetComponentToWorld().GetLocation(), true);
		ObjectMesh->SetWorldRotation(RootComponent->GetComponentToWorld().Rotator());
	}

	
	
}

void APickUpActor::OnRep_PickedUp()
{
	FString pickedUpString = pickedUp ? "true" : "false";
 	
 	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, pickedUpString);
	ObjectMesh->SetEnableGravity(!pickedUp);
}


void APickUpActor::pickupActor_Implementation()
{
	pickedUp = true;
	
	ObjectMesh->SetCollisionProfileName("IgnoreOnlyPawn");
	OnRep_PickedUp();
}

void APickUpActor::discardActor_Implementation()
{
	if (true)
 	{
	pickedUp = false;
	
	
		OnRep_PickedUp();
		//ObjectMesh->SetEnableGravity(true);
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
