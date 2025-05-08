// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireManager.generated.h"

class AFireSphere;

USTRUCT()
struct FFireSpawnInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) float FireStartTimestamp = 0.1f;
	UPROPERTY(EditAnywhere) AActor* FireStartLocation = nullptr;
};
UCLASS()
class FIREBROS_API AFireManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TSet<AFireSphere*> FullyGrownFires;
	TSet<AFireSphere*> Fires;

	UFUNCTION(Server, Reliable)void SpreadFire();

	void AddFullyGrownFire(AFireSphere* fire);
	void RemoveFullyGrownFire(AFireSphere* fire);

	UPROPERTY(EditDefaultsOnly) TSubclassOf<AFireSphere> fireClass;

	//the amount of time in seconds between each spread of the fire
	UPROPERTY(EditAnywhere) float FireSpreadRate = 30;
	float FireSpreadClock;

	//Empty actors that correspond to the positions that fire will be spawned in
	//UPROPERTY(EditAnywhere, Category="FireLocations") TArray<AActor*> FireStartLocations;
	//amount of seconds that it will take for the fire to spawn
	//UPROPERTY(EditAnywhere, Category="FireLocations") TArray<float> FireStartTimestamps;

	UPROPERTY(EditAnywhere, Category="FireLocations") TArray<FFireSpawnInfo> FireSpawnInfos;

	TArray<FTimerHandle> fireLocationTimer;

	UFUNCTION(Server, Reliable) void SpawnFireAtLocation(FVector location);

	UFUNCTION(Server, Reliable) void DestroyFireToServer(AFireSphere* sphere);

	void ClearTimers();
};
