// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableObject.generated.h"

class UGeometryCollectionComponent;
class UBoxComponent;

UCLASS()
class FIREBROS_API ABreakableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere) UBoxComponent*				 surroundingCollider = nullptr;
	UPROPERTY(VisibleAnywhere) UGeometryCollectionComponent* shatterObject		 = nullptr;

	
	UFUNCTION(Server, Reliable) void BreakObjectToServer();
	
	UFUNCTION(NetMulticast,Reliable) void BreakObjectMulticast(FVector direction);
};
