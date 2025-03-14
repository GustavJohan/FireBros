#pragma once

#include "PickUpActor.h"
#include "RagdollCharacter.h"
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
class FIREBROS_API AFireFighter : public ARagdollCharacter
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent*				_PickedUpObjAnchor  = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent*				_PickUpObjHitBox    = nullptr;

	

	UPROPERTY(EditDefaultsOnly, Category="Player Stat") int throwStrength = 100;
	//UPROPERTY(ReplicatedUsing=RagDollActorSet) AFireFighterRagdoll* ragdollActor;
	//UFUNCTION() void RagDollActorSet();
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) AActor* pickedUpItem = nullptr;
	
	
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


	UFUNCTION(Client, Reliable) void SetCameraPositionOnClient(FVector pos);
	
	UFUNCTION(Server, Reliable) void UseToolRPCToServerFromFireFighter();
	


	void PickupAction     (const FInputActionValue& Value);
	void DiscardAction     (const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable) void pickupToServer();
	UFUNCTION(NetMulticast, Reliable) void pickupMulticast();

	UFUNCTION(Server, Reliable) void discardToServer();
	UFUNCTION(Server, Reliable) void ThrowFromFireFighterToServer(FVector throwDirection);
	UFUNCTION(NetMulticast, Reliable) void discardMulticast();
	UFUNCTION(NetMulticast, Reliable) void ResetHelditemRefMulticast();
	
	FVector2f PrevCameraRot;
	TArray<FRotator, TFixedAllocator<60>> CameraMovementLog;
	TArray<FVector2d, TFixedAllocator<60>> mouseMovementLog;
	FVector2d mouseMoveThisTick = FVector2d::Zero();
	int cameraMovementLogCurrent = 0;

	void beginRagdoll(float ragdollTime) override;
	void RagdollPickup() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
