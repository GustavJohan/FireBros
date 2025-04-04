// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpActor.h"
#include "Tool.generated.h"

/**
 * 
 */
UCLASS()
class FIREBROS_API ATool : public APickUpActor
{
	GENERATED_BODY()

public:
	
	UFUNCTION(Server	  , Reliable) virtual void UseToolToServer();
	UFUNCTION(NetMulticast, Reliable) virtual void UseToolMulticast();

	UFUNCTION(Server	  , Reliable) virtual void PickupToolToServer();
	UFUNCTION(NetMulticast, Reliable) virtual void PickupToolMulticast();

	
	UFUNCTION(Server	  , Reliable) virtual void DiscardToolToServer();
	UFUNCTION(NetMulticast, Reliable) virtual void DiscardToolMulticast();

	void BeginPlay() override;

	UPROPERTY() float groundSnapHeightOffset = 10;
	UFUNCTION()void SnapToGround();
	
	UFUNCTION()void DisablePhysicsSimulation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
