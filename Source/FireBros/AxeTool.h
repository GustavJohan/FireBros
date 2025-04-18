// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "AxeTool.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class FIREBROS_API AAxeTool : public ATool
{
	GENERATED_BODY()
	AAxeTool();

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)UBoxComponent* AxeHitBox = nullptr;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)UStaticMeshComponent* AxeMeshComponent = nullptr;

	virtual void UseToolToServer_Implementation() override;
};
