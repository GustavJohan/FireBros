// Fill out your copyright notice in the Description page of Project Settings.


#include "Tool.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

void ATool::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<UActorComponent*> Components;
	
	GetComponents(Components);
	
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass() == UStaticMeshComponent::StaticClass())
		{
			Cast<UStaticMeshComponent>(Component)->OnComponentHit.AddDynamic(this, &ATool::DisablePhysicsSimulation);
		}
	}
	
}


void ATool::DisablePhysicsSimulation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//if this object hits the floor it will stop simulating physics,
	//this is because small physics objects on the ground can cause interacting characters to "freak out" to use the professional term
	if (OtherActor->GetClass() != AStaticMeshActor::StaticClass()){return;}
	
	TArray<UActorComponent*> Components;

	GetComponents(Components);
	
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass() == UStaticMeshComponent::StaticClass())
		{
			Cast<UStaticMeshComponent>(Component)->SetSimulatePhysics(false);
		}
	}

	SnapToGround();
}

void ATool::SnapToGround()
{
	SetActorRotation(FRotator(90,0,GetActorRotation().Roll));
    
	FHitResult groundHit;
	GetWorld()->LineTraceSingleByChannel(groundHit, GetActorLocation(), GetActorLocation() + FVector::DownVector*100, ECC_WorldStatic);
    
	if (groundHit.bBlockingHit)
	{
		SetActorLocation(groundHit.Location + FVector::UpVector*groundSnapHeightOffset);
	}
}


void ATool::UseToolToServer_Implementation()
{
	
}

void ATool::UseToolMulticast_Implementation()
{
	
}

void ATool::PickupToolToServer_Implementation()
{
	
}

void ATool::PickupToolMulticast_Implementation()
{
	TArray<UActorComponent*> Components;

	GetComponents(Components);
	
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass() == UStaticMeshComponent::StaticClass())
		{
			Cast<UStaticMeshComponent>(Component)->SetCollisionProfileName("NoCollision");
			Cast<UStaticMeshComponent>(Component)->SetSimulatePhysics(false);
		}
	}
}

void ATool::DiscardToolToServer_Implementation()
{
	
}

void ATool::DiscardToolMulticast_Implementation()
{
	TArray<UActorComponent*> Components;

	GetComponents(Components);
	
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass() == UStaticMeshComponent::StaticClass())
		{
			Cast<UStaticMeshComponent>(Component)->SetCollisionProfileName("BlockAllDynamic");
			Cast<UStaticMeshComponent>(Component)->SetSimulatePhysics(true);
			return;
		}
	}
}

