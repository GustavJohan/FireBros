// Fill out your copyright notice in the Description page of Project Settings.


#include "CivilianCharacter.h"

#include "GameManager.h"
//#include "Runtime/AIModule/Classes/AIController.h"
// "Runtime/AIModule/Classes/AIController.h"
#include "AIController.h"
#include "EvacPoint.h"


ACivilianCharacter::ACivilianCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACivilianCharacter::BeginPlay()
{
	Super::BeginPlay();

	_AIController = Cast<AAIController>(GetController());

	FTimerHandle Handle;


	GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda(
		[this] {Evacuate();}), 2, false);
}

void ACivilianCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (ragdollActor)
	{
		ragdollActor->SetActorLocationAndRotation(GetMesh()->GetComponentLocation(), GetMesh()->GetComponentRotation());
	}
	
}

void ACivilianCharacter::Evacuate()
{
	if (GameManager)
	{
		_AIController->MoveToActor(GameManager->getClosestEvac(GetActorLocation()));
	}
}
