// Fill out your copyright notice in the Description page of Project Settings.


#include "RagdollCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ARagdollCharacter::ARagdollCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARagdollCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARagdollCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (ragdollActor && !_isRagDolling)
	{
		ragdollActor->SetActorLocationAndRotation(GetMesh()->GetComponentLocation(), GetMesh()->GetComponentRotation());
	}
}

// Called to bind functionality to input
void ARagdollCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARagdollCharacter::SpawnRagdollRPCToServer_Implementation()
{
	ragdollActor = GetWorld()->SpawnActor<AFireFighterRagdoll>(ragdollActorClass);
	ragdollActor->SetRagdollMesh(RagdollMesh, this);
}

void ARagdollCharacter::beginRagdoll_Implementation(float ragdollTime)
{
	GetMesh()->SetVisibility(false);
	_isRagDolling = true;
	GetMovementComponent()->Deactivate();

	
	
	GetWorld()->GetTimerManager().SetTimer(resetRagdoll, FTimerDelegate::CreateLambda(
		[this] {endRagdoll();}), ragdollTime, false);
	if (!ragdollActor){return;}
	ragdollActor->BeginCharacterRagdoll();
}

void ARagdollCharacter::endRagdoll_Implementation()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE,5, FColor::Green, GetActorLocation().ToString());
	
	GetMesh()->SetVisibility(true);
	_isRagDolling = false;
	GetMovementComponent()->Activate();
	
	if (!ragdollActor){return;}
	
	ragdollActor->EndCharacterRagdoll();
	SetActorLocation(ragdollActor->GetActorLocation());
}

void ARagdollCharacter::RagdollPickup()
{
	RagdollPickupEvent();
}

void ARagdollCharacter::RagdollThrowToServer_Implementation(FVector force)
{
	RagdollThrowMultiCast(force);
}


void ARagdollCharacter::RagdollThrowMultiCast_Implementation(FVector force)
{
	RagdollThrowEvent();

	if (HasAuthority())
	{
		beginRagdoll();
		ragdollActor->AddRagdollImpulse(force);
		
	}
}
