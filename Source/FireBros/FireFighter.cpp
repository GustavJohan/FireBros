#include "FireFighter.h"

#include "BreakableObject.h"
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

	_HitObjBox = CreateDefaultSubobject<UBoxComponent>("hit Box");
	_HitObjBox->SetupAttachment(RootComponent);
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

void AFireFighter::SpawnRagdollRPCToServer_Implementation()
{
	ragdollActor = GetWorld()->SpawnActor<AFireFighterRagdoll>(ragdollActorClass);
	ragdollActor->SetRagdollMesh(RagdollMesh, this);
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
		ragdollActor->SetActorTransform(_RagdollMeshAnchor->GetComponentTransform());
	}
	else
	{
		if (ragdollActor)
		{
			SetCameraPositionOnClient(ragdollActor->GetActorLocation());
		}
		else
		{
			_CameraArmComponent->SetWorldLocation(ragdollActor->GetActorLocation() + FVector::UpVector*50);
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
	FVector2d inputVector = -Value.Get<FVector2d>();

	mouseMoveThisTick += inputVector;
	
	AddControllerPitchInput(-inputVector.Y);
	AddControllerYawInput  (-inputVector.X);
}

void AFireFighter::JumpAction(const FInputActionValue& Value)
{
	Jump();
}

void AFireFighter::beginRagdoll_Implementation(float ragdollTime)
{
	GetMesh()->SetVisibility(false);
	_isRagDolling = true;

	
	GetWorld()->GetTimerManager().SetTimer(resetRagdoll, FTimerDelegate::CreateLambda(
		[this] {endRagdoll();}), ragdollTime, false);
	if (!ragdollActor){return;}
	ragdollActor->BeginCharacterRagdoll();
}

void AFireFighter::endRagdoll_Implementation()
{
	
	GetMesh()->SetVisibility(true);
	_isRagDolling = false;
	
	if (!ragdollActor){return;}
	
	ragdollActor->EndCharacterRagdoll();
	SetActorLocation(ragdollActor->GetActorLocation());
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
		}
		else
		{
			pickedUpItem = Cast<APickUpActor>(pickedUpItem);
			DiscardObject(pickedUpItem);
			//pickedUpItem->SetActorEnableCollision(true);
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			pickedUpItem->discardActor();
			
			

			if (!GetController()){return;}
			FRotator rotDelta = UKismetMathLibrary::NormalizedDeltaRotator(
				GetController()->GetDesiredRotation(), CameraMovementLog[(cameraMovementLogCurrent+1)%60]);
			
			FRotator bestRot = FRotator::ZeroRotator;
			float bestDotProd = 1;
			/*
			for (FRotator rotations : CameraMovementLog)
			{
				float currentDotProd = UKismetMathLibrary::Abs(UKismetMathLibrary::Dot_VectorVector(
				rotations.Quaternion().GetForwardVector(),
				GetController()->GetDesiredRotation().Quaternion().GetForwardVector()));

				if (bestDotProd > currentDotProd)
				{
					GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, FString::SanitizeFloat(currentDotProd) + " is the new best");
					bestDotProd = currentDotProd;
					bestRot = rotations;
				}
			}
			*/
			
			FVector2d cameraDelta;
                                   	
			for (FVector2d cameraMove : mouseMovementLog)
			{
				cameraDelta += cameraMove;
			}
			
			//rotDelta *= UKismetMathLibrary::Vector4_DotProduct(GetController()->GetDesiredRotation().Euler(), CameraMovementLog[(cameraMovementLogCurrent+1)%60].Euler());

			
			//FVector throwDirection = GetActorRightVector() * rotDelta.Yaw + GetActorUpVector() * rotDelta.Pitch;
			FVector throwDirection = GetActorRightVector() * -cameraDelta.X + GetActorUpVector() * cameraDelta.Y;

			
			
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "camera delta is: " + rotDelta.ToString());
			//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, FString::SanitizeFloat(UKismetMathLibrary::Vector4_DotProduct(
			//	GetController()->GetDesiredRotation().Euler(), CameraMovementLog[(cameraMovementLogCurrent+1)%60].Euler())));

			//float mouseMoveModifier = 1 - UKismetMathLibrary::Abs(UKismetMathLibrary::Dot_VectorVector(
			//	CameraMovementLog[(cameraMovementLogCurrent+1)%60].Quaternion().GetForwardVector(),
			//	GetController()->GetDesiredRotation().Quaternion().GetForwardVector()));
			
			//throwDirection *= mouseMoveModifier;
			//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow,FString::SanitizeFloat(mouseMoveModifier));
			
			pickedUpItem->throwActor(throwDirection*throwStrength);
			pickedUpItem = nullptr;
		}
		return;
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
					pickedUpItem = Cast<APickUpActor>(pickedUp);
					PickUpTool(pickedUp);
					Cast<ATool>(pickedUpItem)->PickupToolMulticast();
				}
				else
				{
					pickedUpItem = Cast<APickUpActor>(pickedUp);
					PickUpObject(pickedUp);
					pickedUpItem->pickupActor();
					//pickedUpItem->SetActorEnableCollision(false);
					GetCharacterMovement()->bUseControllerDesiredRotation = true;
					GetCharacterMovement()->bOrientRotationToMovement = false;
					
				}
			}
		}
	}
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
	Cast<ATool>(pickedUpItem)->UseToolToServer();
}

void AFireFighter::SetCameraPositionOnClient_Implementation(FVector pos)
{
	_CameraArmComponent->SetWorldLocation(pos + FVector::UpVector*50);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Ragdolling");
}
