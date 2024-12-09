// Fill out your copyright notice in the Description page of Project Settings.


#include "FireFighterRagdoll.h"

// Sets default values
AFireFighterRagdoll::AFireFighterRagdoll()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	_RagdollMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ragdoll mesh");

	_RagdollMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AFireFighterRagdoll::BeginPlay()
{
	Super::BeginPlay();

	_RagdollMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	_RagdollMesh->SetVisibility(false);
}

// Called every frame
void AFireFighterRagdoll::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isRagdolling)
	{
		_RagdollMesh->SetWorldTransform(GetActorTransform(), false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		SetActorLocation(_RagdollMesh->GetComponentLocation());
	}
}

void AFireFighterRagdoll::SetRagdollMesh(USkeletalMesh* mesh, ACharacter* OwningCharacter)
{
	_RagdollMesh->SetSkeletalMeshAsset(mesh);
	_owningCharacter = OwningCharacter;
}


void AFireFighterRagdoll::BeginCharacterRagdoll_Implementation()
{
	//if (!HasAuthority()){return;}
	isRagdolling = true;
	
	_RagdollMesh->SetSimulatePhysics(false);
	_RagdollMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	_RagdollMesh->SetWorldLocation(GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);

	RagdollClient();
	
	if (HasAuthority())
	{
		_RagdollMesh->SetSimulatePhysics(true);
	}
	
	_RagdollMesh->SetVisibility(true);
}

void AFireFighterRagdoll::EndCharacterRagdoll_Implementation()
{
	//if (!HasAuthority()){return;}
	isRagdolling = false;
	
	_RagdollMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{
		_RagdollMesh->SetSimulatePhysics(false);
	}
	_RagdollMesh->SetVisibility(false);
}