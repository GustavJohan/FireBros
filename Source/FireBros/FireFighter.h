#pragma once

#include "PickUpActor.h"
#include "GameFramework/Character.h"
#include "FireFighter.generated.h"

class ABreakableObject;
class UBoxComponent;
class AFireFighterRagdoll;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class FIREBROS_API AFireFighter : public ACharacter
{
	GENERATED_BODY()
	AFireFighter();

public:
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _move;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _look;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _jump;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _pickUp;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _hitObj;

	UPROPERTY(EditAnywhere, Category=Input) TSoftObjectPtr<UInputMappingContext> _defaultInputMapping;

	
	UPROPERTY(EditAnywhere, Category=Input) int invertCameraVertical = 1;
	UPROPERTY(EditAnywhere, Category=Input) int invertCameraHorizontal = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USpringArmComponent*		    _CameraArmComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent*				_RagdollMeshAnchor  = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent*				_PickedUpObjAnchor  = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent*				_PickUpObjHitBox    = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent*				_HitObjBox		    = nullptr;

	UPROPERTY(EditDefaultsOnly) USkeletalMesh* RagdollMesh;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<AFireFighterRagdoll> ragdollActorClass;
	UPROPERTY() AFireFighterRagdoll* ragdollActor;

	UPROPERTY(EditDefaultsOnly, Category="Player Stat") int throwStrength = 100;
	//UPROPERTY(ReplicatedUsing=RagDollActorSet) AFireFighterRagdoll* ragdollActor;
	//UFUNCTION() void RagDollActorSet();
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) APickUpActor* pickedUpItem = nullptr;
	
	bool _isRagDolling = false;
	
	UFUNCTION(BlueprintImplementableEvent) void HitObj();
	UFUNCTION(BlueprintImplementableEvent) void PickUpTool(AActor* PickedUp);
	UFUNCTION(BlueprintImplementableEvent) void DiscardTool  (AActor* Discarded);
	UFUNCTION(BlueprintImplementableEvent) void PickUpObject(AActor* PickedUp);
	UFUNCTION(BlueprintImplementableEvent) void DiscardObject(AActor* Discarded);
private:
	void MoveAction       (const FInputActionValue& Value);
	void LookAction       (const FInputActionValue& Value);
	void JumpAction       (const FInputActionValue& Value);
	void UseToolAction	  (const FInputActionValue& Value);

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

	FTimerHandle resetRagdoll;

	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable) void beginRagdoll(float ragdollTime = 5);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable) void endRagdoll();

	UFUNCTION(Client, Reliable) void SetCameraPositionOnClient(FVector pos);
	
	UFUNCTION(Server, Reliable) void UseToolRPCToServerFromFireFighter();
	
	UFUNCTION(Server, Reliable) void SpawnRagdollRPCToServer();


	void PickupAction     (const FInputActionValue& Value);
	void DiscardAction     (const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable) void pickupToServer();
	UFUNCTION(NetMulticast, Reliable) void pickupMulticast();

	UFUNCTION(Server, Reliable) void discardToServer();
	UFUNCTION(NetMulticast, Reliable) void discardMulticast();
	
	FVector2f PrevCameraRot;
	TArray<FRotator, TFixedAllocator<60>> CameraMovementLog;
	TArray<FVector2d, TFixedAllocator<60>> mouseMovementLog;
	FVector2d mouseMoveThisTick = FVector2d::Zero();
	int cameraMovementLogCurrent = 0;
	

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
