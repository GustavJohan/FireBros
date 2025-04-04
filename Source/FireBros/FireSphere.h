// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireSphere.generated.h"

class AFireManager;
class USphereComponent;

UCLASS()
class FIREBROS_API AFireSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireSphere();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AFireManager* FireManager;

	UPROPERTY(VisibleAnywhere) USphereComponent* FireCollider = nullptr;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* FireMesh = nullptr;
	UPROPERTY(EditDefaultsOnly) float FireMeshScaler = 1;
	UPROPERTY(EditDefaultsOnly) float MaxFireRadius = 200;
	UPROPERTY(EditDefaultsOnly) float FireGrowthRate = 5;
	float CurrentFireRadius = 0;

	FVector GroundLocation;

	void GrowFire(float DeltaTime);
	void ChangeFireSize(float DeltaRadius);

	TArray<FVector> CheckSpreadLocations(TSet<AFireSphere*> Allfires);
	TArray<FVector> SpreadLocations;

	bool fullyGrown = false;

	UPROPERTY(EditDefaultsOnly) int NumSpreadLocations = 6;

	bool persistent = false;

	void DestroyFire();
};
