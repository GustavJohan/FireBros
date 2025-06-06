// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpActor.generated.h"

UCLASS()
class FIREBROS_API APickUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PickedUp) bool pickedUp = false;
	
	UFUNCTION() void OnRep_PickedUp();

	UFUNCTION(Server, Reliable) void pickupActor();
	UFUNCTION(Server, Reliable) void discardActor();

	UStaticMeshComponent* ObjectMesh = nullptr;

	UFUNCTION(Server, Reliable) void throwActor(FVector impulseDirection);
};
