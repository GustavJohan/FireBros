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
#include "GameFramework/GameSession.h"

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

	//look into setting ownership to server instead of this terribleness
	DOREPLIFETIME(AFireFighter, ragdollActor);
}


void AFireFighter::RagDollActorSet()
{
	ragdollActor->SetRagdollMesh(RagdollMesh, this);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Purple, "ragdoll mesh set");
}
*/

void AFireFighter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);


	if (_move){Input->BindAction(_move, ETriggerEvent::Triggered, this, &AFireFighter::MoveAction);}
	if (_look){Input->BindAction(_look, ETriggerEvent::Triggered, this, &AFireFighter::LookAction);}
	if (_jump){Input->BindAction(_jump, ETriggerEvent::Triggered, this, &AFireFighter::JumpAction);}
	if (_pickUp){Input->BindAction(_pickUp, ETriggerEvent::Triggered, this, &AFireFighter::PickupAction);}
	if (_hitObj){Input->BindAction(_hitObj, ETriggerEvent::Started, this, &AFireFighter::HitObjAction);}
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
	
	SpawnRagdollRPCToServer();
}

void AFireFighter::SpawnRagdollRPCToServer_Implementation()
{
	ragdollActor = GetWorld()->SpawnActor<AFireFighterRagdoll>(ragdollActorClass);
	ragdollActor->SetRagdollMesh(RagdollMesh, this);
	//ragdollActor->SetOwner(GetPlayerControllerFromNetId(GetWorld(), 0));
	//setUpRagdollRPCMulticast(ragdollActor);
}

void AFireFighter::setUpRagdollRPCMulticast_Implementation(AFireFighterRagdoll* Ragdoll)
{
	if (ragdollActor == nullptr)
	{
		ragdollActor = Ragdoll;
	}
	
	//Ragdoll->SetRagdollMesh(RagdollMesh, this);
	
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
		//_RagdollMesh->SetWorldTransform(_RagdollMeshAnchor->GetComponentTransform());
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

	if (pickedUpItem)
	{
		if (pickedUpItem->IsA<ATool>())
		{
			//pickedUpItem->SetActorLocation()
		}
		else
		{
			pickedUpItem->SetActorLocation(_PickedUpObjAnchor->GetComponentLocation());
		}
	}
}


void AFireFighter::MoveAction(const FInputActionValue& Value)
{
	FVector2d inputVector = Value.Get<FVector2d>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	
	AddMovementInput(ForwardDirection, inputVector.Y);
	AddMovementInput(RightDirection,   inputVector.X);

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Moving");
	//AddMovementInput(_CameraArmComponent->GetForwardVector(),inputVector.Y);
	//AddMovementInput(_CameraArmComponent->GetRightVector(), inputVector.X);
}

void AFireFighter::LookAction(const FInputActionValue& Value)
{
	FVector2d inputVector = -Value.Get<FVector2d>();

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, inputVector.ToString());
	
	AddControllerPitchInput(inputVector.Y);
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
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "pickup");

	if (pickedUpItem){return;}

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
				}
				else
				{
					pickedUpItem = Cast<APickUpActor>(pickedUp);
					PickUpObject(pickedUp);
				}
			}
			
		}
		
	}
}

void AFireFighter::HitObjAction(const FInputActionValue& Value)
{
	TArray<AActor*> hitObjs;

	HitObj();
	
	_HitObjBox->GetOverlappingActors(hitObjs);

	for (AActor* HitObj : hitObjs)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Green, "break");
		if (HitObj->IsA<ABreakableObject>())
		{
			//Cast<ABreakableObject>(HitObj)->BreakObjectToServer_Implementation()
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Cyan, "break firefighter to server");
			//HitObj->SetOwner(GetController());
			BreakObjectRPCToServerFromFireFighter(Cast<ABreakableObject>(HitObj));
				
			
		}
	}
}

void AFireFighter::BreakObjectRPCToServerFromFireFighter_Implementation(ABreakableObject* objectToBreak)
{
	FVector direction = objectToBreak->GetActorLocation() - GetActorLocation();
	direction.Normalize();
	direction += FVector::UpVector;
	direction.Normalize();

	objectToBreak->BreakObjectMulticast(direction);
}

void AFireFighter::SetCameraPositionOnClient_Implementation(FVector pos)
{
	_CameraArmComponent->SetWorldLocation(pos + FVector::UpVector*50);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Ragdolling");
}
