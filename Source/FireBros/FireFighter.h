#pragma once

#include "PickUpActor.h"
#include "GameFramework/Character.h"
#include "FireFighter.generated.h"

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

	UPROPERTY(EditAnywhere, Category=Input) TSoftObjectPtr<UInputMappingContext> _defaultInputMapping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USpringArmComponent*		    _CameraArmComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent*				_RagdollMeshAnchor  = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent*				_PickedUpObjAnchor  = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent*				_PickUpObjHitBox    = nullptr;

	UPROPERTY(EditDefaultsOnly) USkeletalMesh* RagdollMesh;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<AFireFighterRagdoll> ragdollActorClass;
	UPROPERTY() AFireFighterRagdoll* ragdollActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) APickUpActor* pickedUpItem = nullptr;
	
	bool _isRagDolling = false;
	
	UFUNCTION(BlueprintImplementableEvent) void PickUpTool  (AActor* PickedUp);
	UFUNCTION(BlueprintImplementableEvent) void PickUpObject(AActor* PickedUp);
private:
	void MoveAction       (const FInputActionValue& Value);
	void LookAction       (const FInputActionValue& Value);
	void JumpAction       (const FInputActionValue& Value);
	void PickupAction     (const FInputActionValue& Value);

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	FTimerHandle resetRagdoll;
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable) void beginRagdoll(float ragdollTime = 5);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable) void endRagdoll();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
