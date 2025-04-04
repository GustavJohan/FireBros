// Fill out your copyright notice in the Description page of Project Settings.


#include "FireExtinguisher.h"

#include "FireSphere.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AFireExtinguisher::AFireExtinguisher()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	extinguisherMesh = CreateDefaultSubobject<UStaticMeshComponent>("extinguisher");
	extinguisherMesh->SetupAttachment(RootComponent);
	extinguisherCollider = CreateDefaultSubobject<USphereComponent>("collider");
	extinguisherCollider->SetupAttachment(extinguisherMesh);
}

void AFireExtinguisher::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFireExtinguisher, Active);
}

void AFireExtinguisher::UseToolToServer_Implementation()
{
	Super::DiscardToolToServer_Implementation();
	//UseToolMulticast();
	Active = !Active;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE,5, FColor::Green, "multicast");
}

void AFireExtinguisher::UseToolMulticast_Implementation()
{
	//Super::UseToolMulticast_Implementation();
	

	
}

void AFireExtinguisher::DiscardToolMulticast_Implementation()
{
	Super::DiscardToolMulticast_Implementation();
	
	Active = false;

	extinguisherCollider->SetActive(Active);

	
	
}


void AFireExtinguisher::Tick(float DeltaSeconds)
{
	if (Active)
	{
		TArray<AActor*> overlapFire;
		extinguisherCollider->GetOverlappingActors(overlapFire, AFireSphere::StaticClass());

		GEngine->AddOnScreenDebugMessage(INDEX_NONE,5, FColor::Green, "" + overlapFire.Num());
		
		for (auto OverlapFire : overlapFire)
		{
			Cast<AFireSphere>(OverlapFire)->ChangeFireSize(-DeltaSeconds*20);
		}
	}
}

void AFireExtinguisher::OnRep_Active()
{
	extinguisherCollider->SetActive(Active);
    
	if (Active)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE,5, FColor::Green, "true");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE,5, FColor::Green, "false");
	}
}
