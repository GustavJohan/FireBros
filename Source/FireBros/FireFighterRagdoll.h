// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireFighterRagdoll.generated.h"

UCLASS()
class FIREBROS_API AFireFighterRagdoll : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireFighterRagdoll();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USkeletalMeshComponent*		_RagdollMesh	    = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) ACharacter* _owningCharacter = nullptr;

	bool isRagdolling = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetRagdollMesh(USkeletalMesh* mesh, ACharacter* OwningCharacter);
	
	UFUNCTION(NetMulticast, Reliable)void BeginCharacterRagdoll();
	UFUNCTION(NetMulticast, Reliable)void EndCharacterRagdoll();

	UFUNCTION(BlueprintImplementableEvent) void RagdollClient();
};
