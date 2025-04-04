// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "FireExtinguisher.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class FIREBROS_API AFireExtinguisher : public ATool
{
	GENERATED_BODY()

	AFireExtinguisher();

public:
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* extinguisherMesh = nullptr;
	UPROPERTY(VisibleAnywhere) USphereComponent* extinguisherCollider = nullptr;

	virtual void UseToolMulticast_Implementation() override;
	virtual void DiscardToolMulticast_Implementation() override;
	virtual void UseToolToServer_Implementation() override;

	void Tick(float DeltaSeconds) override;

	UPROPERTY(ReplicatedUsing=OnRep_Active)bool Active = false;

	UFUNCTION() void OnRep_Active();
};
