// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RagdollCharacter.h"
#include "CivilianCharacter.generated.h"

class AGameManager;
class AAIController;
/**
 * 
 */
UCLASS()
class FIREBROS_API ACivilianCharacter : public ARagdollCharacter
{
	GENERATED_BODY()
	ACivilianCharacter();

	AAIController* _AIController = nullptr;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void endRagdoll() override;

public:
	void Evacuate();

	AGameManager* GameManager = nullptr;

	bool rescued = false;
};
