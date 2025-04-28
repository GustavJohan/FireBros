#include "FireFighter.h"

#include "BreakableObject.h"
#include "CivilianCharacter.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "FireFighterRagdoll.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Tool.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Light.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameSession.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AFireFighter::AFireFighter()
{
	PrimaryActorTick.bCanEverTick = true;
	_CameraArmComponent = CreateDefaultSubobject<USpringArmComponent>("camera arm");
	_CameraArmComponent->SetupAttachment(RootComponent);

	_RagdollMeshAnchor = CreateDefaultSubobject<USceneComponent>("mesh anchor");
	_RagdollMeshAnchor->SetupAttachment(RootComponent);

	_PickedUpObjAnchor = CreateDefaultSubobject<USceneComponent>("pickup anchor");
	_PickedUpObjAnchor->SetupAttachment(RootComponent);

	_PickUpObjHitBox = CreateDefaultSubobject<UBoxComponent>("pickup Box");
	_PickUpObjHitBox->SetupAttachment(RootComponent);
}

/*
void AFireFighter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME();
}
*/

void AFireFighter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);


	if (_move){Input->BindAction(_move, ETriggerEvent::Triggered, this, &AFireFighter::MoveAction);}
	if (_look){Input->BindAction(_look, ETriggerEvent::Triggered, this, &AFireFighter::LookAction);}
	if (_jump){Input->BindAction(_jump, ETriggerEvent::Triggered, this, &AFireFighter::JumpAction);}
	if (_pickUp){Input->BindAction(_pickUp, ETriggerEvent::Started, this, &AFireFighter::PickupAction);}
	if (_pickUp){Input->BindAction(_pickUp, ETriggerEvent::Completed, this, &AFireFighter::DiscardAction);}
	if (_hitObj){Input->BindAction(_hitObj, ETriggerEvent::Started, this, &AFireFighter::UseToolAction);}
}

void AFireFighter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(_defaultInputMapping.LoadSynchronous(), 0);
		}
	}

	for (int i = 0; i < 60; ++i)
	{
		CameraMovementLog.Add(FRotator::ZeroRotator);
		mouseMovementLog.Add(FVector2d::Zero());
	}
	
	SpawnRagdollRPCToServer();

	
}

void AFireFighter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!_isRagDolling)
	{
		_CameraArmComponent->SetWorldLocation(GetActorLocation());
	}
	
	if (!ragdollActor){return;}

	if (!_isRagDolling)
	{
		_CameraArmComponent->SetWorldLocation(GetActorLocation());
		//ragdollActor->SetActorTransform(_RagdollMeshAnchor->GetComponentTransform(), false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		if (ragdollActor)
		{
			SetCameraPositionOnClient(ragdollActor->GetActorLocation());
		}
	}
}

void AFireFighter::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);

	
	
	if (!GetController()){return;}

	if (!pickedUpItem){return;}
	
	//FVector2f currentCameraRotation = FVector2f(GetController()->GetDesiredRotation().Yaw, GetController()->GetDesiredRotation().Pitch);

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, (currentCameraRotation-PrevCameraRot).ToString());
	
	CameraMovementLog[cameraMovementLogCurrent] = GetController()->GetDesiredRotation();//currentCameraRotation-PrevCameraRot;
	mouseMovementLog[cameraMovementLogCurrent] = mouseMoveThisTick;
	mouseMoveThisTick = FVector2d::Zero();

	cameraMovementLogCurrent = cameraMovementLogCurrent++ % 60;


	//PrevCameraRot = FVector2f(GetController()->GetDesiredRotation().Yaw, GetController()->GetDesiredRotation().Pitch);
}

void AFireFighter::MoveAction(const FInputActionValue& Value)
{
	FVector2d inputVector = Value.Get<FVector2d>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FRotator RollPitch(Rotation.Roll, 0, Rotation.Pitch);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	// get right vector 
	//const FVector RightDirection = FRotationMatrix(RollPitch).GetUnitAxis(EAxis::Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(ForwardDirection, inputVector.Y);
	AddMovementInput(RightDirection,   inputVector.X);
}

void AFireFighter::LookAction(const FInputActionValue& Value)
{
	FVector2d inputVector = Value.Get<FVector2d>();

	mouseMoveThisTick += inputVector;
	
	AddControllerPitchInput(inputVector.Y*invertCameraVertical);
	AddControllerYawInput(inputVector.X*invertCameraHorizontal);
}

void AFireFighter::JumpAction(const FInputActionValue& Value)
{
	Jump();
}

void AFireFighter::PickupAction(const FInputActionValue& Value)
{
	pickupToServer();
}

void AFireFighter::pickupToServer_Implementation()
{
	pickupMulticast();
}

void AFireFighter::pickupMulticast_Implementation()
{
	if (pickedUpItem)
	{
		if (pickedUpItem->IsA<ATool>())
		{
			Cast<ATool>(pickedUpItem)->DiscardToolMulticast();
			DiscardTool(pickedUpItem);
			pickedUpItem = nullptr;
			return;
		}

		pickedUpItem = nullptr;
	}
	
	TArray<AActor*> actorsInBounds;
	_PickUpObjHitBox->GetOverlappingActors(actorsInBounds);
    	
	for (AActor* pickedUp : actorsInBounds)
	{
		if (pickedUp->IsA<APickUpActor>())
		{
			if (!Cast<APickUpActor>(pickedUp)->pickedUp)
			{
				if (pickedUp->IsA<ATool>())
				{
					pickedUpItem = pickedUp;
					PickUpTool(pickedUp);
					Cast<ATool>(pickedUpItem)->PickupToolMulticast();
					return;
				}
				else
				{
					pickedUpItem = pickedUp;
					PickUpObject(pickedUp);
					Cast<APickUpActor>(pickedUp)->pickupActor();
					GetCharacterMovement()->bUseControllerDesiredRotation = true;
					GetCharacterMovement()->bOrientRotationToMovement = false;
					return;
					
				}
			}
			
		}
		else if (pickedUp->IsA<ARagdollCharacter>() && pickedUp != this)
		{
			pickedUpItem = pickedUp;
			PickUpObject(pickedUp);
			Cast<ARagdollCharacter>(pickedUp)->RagdollPickup();
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			return;
		}
	}
}

void AFireFighter::DiscardAction(const FInputActionValue& Value)
{
	discardToServer();
}

void AFireFighter::discardToServer_Implementation()
{
	discardMulticast();
}

void AFireFighter::discardMulticast_Implementation()
{
	if (pickedUpItem)
	{
		if (pickedUpItem->IsA<ATool>()) {return;}

		
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Discarding");
		DiscardObject(pickedUpItem);
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		if (pickedUpItem->IsA<APickUpActor>())
		{
			Cast<APickUpActor>(pickedUpItem)->discardActor();
		}
			
			

			
			
		FVector2d cameraDelta;
                                   	
		for (FVector2d cameraMove : mouseMovementLog)
		{
			cameraDelta += cameraMove;
		}
		cameraDelta.X *= throwModifier.X;
		cameraDelta.Y *= throwModifier.Y;

		//throwing downwards can cause ragdolls to clip through the floor. Since there is basically no reason to do it I decided to just prevent it altogether 
		if (cameraDelta.Y < 0)
		{
			cameraDelta.Y = 0;
		}
		
		FVector throwDirection = GetActorRightVector() * cameraDelta.X + GetActorUpVector() * cameraDelta.Y;

		if (!GetController()){return;}

		ThrowFromFireFighterToServer(throwDirection);

		
	}
}

void AFireFighter::ThrowFromFireFighterToServer_Implementation(FVector throwDirection)
{
	//Idiot proofing
	if (!pickedUpItem)
	{
		return;
	}
    			
	if (pickedUpItem->IsA<APickUpActor>())
	{
		Cast<APickUpActor>(pickedUpItem)->throwActor(throwDirection*throwStrength);
	}
	else
	{
		Cast<ARagdollCharacter>(pickedUpItem)->RagdollThrowToServer(throwDirection*throwStrength);
	}
                                                               			
                                                               			
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "camera delta is: " + throwDirection.ToString());

	//pickedUpItem = nullptr;
	//ResetHelditemRefMulticast();
}

void AFireFighter::ResetHelditemRefMulticast_Implementation()
{
	pickedUpItem = nullptr;
}


void AFireFighter::UseToolAction(const FInputActionValue& Value)
{
	

	if (!pickedUpItem){return;}
	if (!pickedUpItem->IsA<ATool>()){return;}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Tool Used");

	UseToolRPCToServerFromFireFighter();
}

void AFireFighter::UseToolRPCToServerFromFireFighter_Implementation()
{
	if (pickedUpItem)
	{
		Cast<ATool>(pickedUpItem)->UseToolToServer();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "what?");
	}
	
}

void AFireFighter::SetCameraPositionOnClient_Implementation(FVector pos)
{
	_CameraArmComponent->SetWorldLocation(pos + FVector::UpVector*50);
}


void AFireFighter::RagdollPickup()
{
	discardToServer();
	Super::RagdollPickup();
}

void AFireFighter::beginRagdoll(float ragdollTime)
{
	discardToServer();
	Super::beginRagdoll(ragdollTime);
}

