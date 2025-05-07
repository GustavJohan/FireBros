// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FireFighterRagdoll.h"
#include "GameManager.h"
#include "RagdollCharacter.generated.h"

UCLASS()
class FIREBROS_API ARagdollCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARagdollCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	UFUNCTION(Server, Reliable) void SpawnRagdollRPCToServer();

	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable) virtual void beginRagdoll(float ragdollTime = 5);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable) virtual void endRagdoll();

	
	FTimerHandle resetRagdoll;

	UPROPERTY(EditDefaultsOnly) USkeletalMesh* RagdollMesh;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<AFireFighterRagdoll> ragdollActorClass;
	UPROPERTY() AFireFighterRagdoll* ragdollActor;
	
	UPROPERTY(BlueprintReadOnly)bool _isRagDolling = false;

	UFUNCTION() virtual void RagdollPickup();
	UFUNCTION(BlueprintImplementableEvent) void RagdollPickupEvent();
	UFUNCTION(NetMulticast, Reliable) void RagdollThrowMultiCast(FVector force);
	UFUNCTION(Server, Reliable) void RagdollThrowToServer(FVector force);
	UFUNCTION(BlueprintImplementableEvent) void RagdollThrowEvent();

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent*	_RagdollMeshAnchor  = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly) float Health = 100;

	UFUNCTION()void OnRep_Health();

	UFUNCTION(BlueprintImplementableEvent) void PlayerDie();
	
	UFUNCTION(Server, Reliable) void DeathRagdoll();

	UFUNCTION(BlueprintImplementableEvent) void RepHealthToBlueprint();

	bool HasRagdollTimer();
	UFUNCTION(NetMulticast, Reliable) void ClearTimer();

	FVector RagdollBackupLocation;

	
	UPROPERTY()AGameManager* GameManager = nullptr;
};
