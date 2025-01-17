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
};
