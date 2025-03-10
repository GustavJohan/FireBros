// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FireFighterRagdoll.h"
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

	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable) void beginRagdoll(float ragdollTime = 5);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable) void endRagdoll();

	
	FTimerHandle resetRagdoll;

	UPROPERTY(EditDefaultsOnly) USkeletalMesh* RagdollMesh;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<AFireFighterRagdoll> ragdollActorClass;
	UPROPERTY() AFireFighterRagdoll* ragdollActor;
	
	bool _isRagDolling = false;

	UFUNCTION() void RagdollPickup();
	UFUNCTION(BlueprintImplementableEvent) void RagdollPickupEvent();
	UFUNCTION(NetMulticast, Reliable) void RagdollThrowMultiCast(FVector force);
	UFUNCTION(Server, Reliable) void RagdollThrowToServer(FVector force);
	UFUNCTION(BlueprintImplementableEvent) void RagdollThrowEvent();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent*				_RagdollMeshAnchor  = nullptr;
};
