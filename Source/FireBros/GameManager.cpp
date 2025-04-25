// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

#include "CivilianCharacter.h"
#include "EvacPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AGameManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameManager, TimeRemaining);
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> CivilianCharactersActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACivilianCharacter::StaticClass(), CivilianCharactersActors);
	
	for (auto CivilianCharacter : CivilianCharactersActors)
	{
		ACivilianCharacter* Character = Cast<ACivilianCharacter>(CivilianCharacter);
		Character->SpawnRagdollRPCToServer();
		Character->SetOwner(GetOwner());
		Character->GameManager = this;
		CivilianCharacters.Add(Character);
	}
	
	TotalCivilians = CivilianCharacters.Num();

	TArray<AActor*> evacuationPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEvacPoint::StaticClass(), evacuationPoints);

	for (auto point : evacuationPoints)
	{
		AEvacPoint* EvacPoint = Cast<AEvacPoint, AActor>(point);
		EvacPoints.Add(EvacPoint);  
	}

	if (!UGameplayStatics::GetGameMode(GetWorld())){return;}

	GetWorldTimerManager().SetTimer(roundTimerHandle, FTimerDelegate::CreateLambda([this]{EvaluateWinEndOfRound();}), RoundTimer, false);
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

void AGameManager::WinGame()
{
	//GetWorldTimerManager().ClearAllTimersForObject(this); // this aparently does not work
	GetWorldTimerManager().ClearTimer(roundTimerHandle);
	WinGameBP();
}

void AGameManager::LoseGame()
{
	//GetWorldTimerManager().ClearAllTimersForObject(this); // this aparently does not work
	GetWorldTimerManager().ClearTimer(roundTimerHandle);
	LoseGameBP();
}

void AGameManager::CheckWin()
{
	int SavedCivilians = 0;

	for (int i = 0; i < EvacPoints.Num(); ++i)
	{
		if (EvacPoints[i])
		{
			SavedCivilians += EvacPoints[i]->GetPresentCivilians();
		}
	}

	UpdateScoreUI(SavedCivilians, TotalCivilians, CivilianCharacters.Num() - SavedCivilians);

	// if all civilians are dead then the players lose
	if (CivilianCharacters.Num() == 0)
	{
		LoseGame();
	}

	
	//for now a win will be triggered if all civilians have evacuated
	if (CivilianCharacters.Num() == SavedCivilians)
	{
		WinGame();
	}
}

void AGameManager::EvaluateWinEndOfRound()
{
	int SavedCivilians = 0;

	for (int i = 0; i < EvacPoints.Num(); ++i)
	{
		if (EvacPoints[i])
		{
			SavedCivilians += EvacPoints[i]->GetPresentCivilians();
		}
	}

	
	if (TotalCivilians/2 >= SavedCivilians)
	{
		LoseGame();
		return;
	}

	
	
	WinGame();
}

void AGameManager::SetTimerRemainingTime_Implementation()
{
	TimeRemaining = GetWorldTimerManager().GetTimerRemaining(roundTimerHandle);
	OnRep_TimeRemaining();
}

void AGameManager::OnRep_TimeRemaining()
{
	OnRep_TimeRemainingToBlueprint(TimeRemaining);
}
