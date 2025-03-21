// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

#include "CivilianCharacter.h"
#include "EvacPoint.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> CivilianCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACivilianCharacter::StaticClass(), CivilianCharacters);
	
	for (auto CivilianCharacter : CivilianCharacters)
	{
		ACivilianCharacter* Character = Cast<ACivilianCharacter>(CivilianCharacter);
		Character->SpawnRagdollRPCToServer();
		Character->SetOwner(GetOwner());
		Character->GameManager = this;
	}

	TArray<AActor*> evacuationPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEvacPoint::StaticClass(), evacuationPoints);

	for (auto point : evacuationPoints)
	{
		AEvacPoint* EvacPoint = Cast<AEvacPoint, AActor>(point);
		EvacPoints.Add(EvacPoint);  
	}
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AEvacPoint* AGameManager::getClosestEvac(FVector position)
{
	if (EvacPoints.Num()==0) return nullptr;
	
	float closestEvacDist = FVector::Dist(position, EvacPoints[0]->GetActorLocation());   
	AEvacPoint* closestEvac = EvacPoints[0];

	for (AEvacPoint* EvacPoint : EvacPoints)
	{
		float currentDist = FVector::Dist(position, EvacPoint->GetActorLocation());
		if (currentDist < closestEvacDist)
		{
			closestEvacDist = currentDist;
			closestEvac = EvacPoint;
		}
	}

	return closestEvac;

}
