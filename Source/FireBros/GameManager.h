// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class ACivilianCharacter;
class AEvacPoint;

UCLASS()
class FIREBROS_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<AEvacPoint*> EvacPoints;
	TArray<ACivilianCharacter*> CivilianCharacters;
	int TotalCivilians;

	AEvacPoint* getClosestEvac(FVector position);

	void WinGame();
	UFUNCTION(BlueprintImplementableEvent) void WinGameBP();
	
	void LoseGame();
	UFUNCTION(BlueprintImplementableEvent) void LoseGameBP();

	void CheckWin();

	void ClearTimers();

	UFUNCTION(BlueprintImplementableEvent) void UpdateScoreUI(int saved, int totalCivilians, int remaining);

	UPROPERTY(EditDefaultsOnly) float RoundTimer = 300;
	FTimerHandle roundTimerHandle;

	UFUNCTION() void EvaluateWinEndOfRound();

	UPROPERTY(ReplicatedUsing=OnRep_TimeRemaining) float TimeRemaining = 0; 
	UFUNCTION(Server, Reliable, BlueprintCallable) void SetTimerRemainingTime();
	UFUNCTION() void OnRep_TimeRemaining();
	UFUNCTION(BlueprintImplementableEvent) void OnRep_TimeRemainingToBlueprint(float newTime);
};
