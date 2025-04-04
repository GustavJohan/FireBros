// Fill out your copyright notice in the Description page of Project Settings.


#include "Tool.h"

#include "RagdollCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

void ATool::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<UActorComponent*> Components;
	
	GetComponents(Components);
	
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass() == UStaticMeshComponent::StaticClass())
		{
			Cast<UStaticMeshComponent>(Component)->SetCollisionProfileName("NoCollision");
			//Cast<UStaticMeshComponent>(Component)->OnComponentHit.AddDynamic(this, &ATool::DisablePhysicsSimulation);
		}
	}
	
}


void ATool::DisablePhysicsSimulation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// currently out of use, tools will snap to the ground and will never simulate physics due to stability issues
	/*
	//if this object hits the floor it will stop simulating physics,
	//this is because small physics objects on the ground can cause interacting characters to "freak out" to use the professional term

	if (!UGameplayStatics::GetGameMode(GetWorld())){return;}
	
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
	*/
}

void ATool::SnapToGround()
{
	if (!UGameplayStatics::GetGameMode(GetWorld())){return;}
	
	SetActorRotation(FRotator(90,0,GetActorRotation().Roll));
    
	FHitResult groundHit;

	FCollisionQueryParams groundCheckParams = FCollisionQueryParams(FName(TEXT("PlaceVerticalTrace")), true);

	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARagdollCharacter::StaticClass(), PlayerActors);
	groundCheckParams.AddIgnoredActors(PlayerActors);
	
	GetWorld()->LineTraceSingleByChannel(groundHit, GetActorLocation(), GetActorLocation() + FVector::DownVector*400, ECC_WorldStatic, groundCheckParams);
    
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
	/*
	TArray<UActorComponent*> Components;

	GetComponents(Components);
	
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass() == UStaticMeshComponent::StaticClass())
		{
			//Cast<UStaticMeshComponent>(Component)->SetCollisionProfileName("NoCollision");
			//Cast<UStaticMeshComponent>(Component)->SetSimulatePhysics(false);
		}
	}
	*/
}

void ATool::DiscardToolToServer_Implementation()
{
	
}

void ATool::DiscardToolMulticast_Implementation()
{
	SnapToGround();

	/*
	TArray<UActorComponent*> Components;

	GetComponents(Components);
	
	for (UActorComponent* Component : Components)
	{
		if (Component->GetClass() == UStaticMeshComponent::StaticClass())
		{
			//Cast<UStaticMeshComponent>(Component)->SetCollisionProfileName("BlockAllDynamic");
		}
	}
	*/
}

