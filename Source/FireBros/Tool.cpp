// Fill out your copyright notice in the Description page of Project Settings.


#include "Tool.h"
#include "Components/StaticMeshComponent.h"


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
		}
	}
}

