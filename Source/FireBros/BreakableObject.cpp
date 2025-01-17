// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableObject.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/BoxComponent.h"
#include "Field/FieldSystemObjects.h"

// Sets default values
ABreakableObject::ABreakableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	surroundingCollider = CreateDefaultSubobject<UBoxComponent>("Box");
	surroundingCollider->SetupAttachment(RootComponent);
	shatterObject = CreateDefaultSubobject<UGeometryCollectionComponent>("Shatter object");
	shatterObject->SetupAttachment(surroundingCollider);
}

// Called when the game starts or when spawned
void ABreakableObject::BeginPlay()
{
	Super::BeginPlay();
	//shatterObject->SetEnableDamageFromCollision(false);
	shatterObject->ApplyAssetDefaults();
	shatterObject->SetEnableDamageFromCollision(false);
	shatterObject->SetSimulatePhysics(false);

	//FTimerHandle breakTimer;
	//GetWorld()->GetTimerManager().SetTimer(breakTimer, FTimerDelegate::CreateLambda(
	//	[this] {BreakObject();}), 10.f, false);

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, GetNetOwningPlayer()->GetName());
}

// Called every frame
void ABreakableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//shatterObject->SetRelativeLocationAndRotation(surroundingCollider->GetRelativeLocation(), surroundingCollider->GetRelativeRotation());
}

void ABreakableObject::BreakObjectToServer_Implementation()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "break to server");

	
	//BreakObjectMulticast();
}

void ABreakableObject::BreakObjectMulticast_Implementation(FVector direction)
{
	shatterObject->SetSimulatePhysics(true);
	shatterObject->SetEnableDamageFromCollision(true);
        	
	shatterObject->CrumbleActiveClusters();
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "break multicast");

	surroundingCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	//useful code example: https://forums.unrealengine.com/t/triggering-physics-fields-against-a-geometry-collection-in-c/1648054
	
	TObjectPtr<UFieldSystemMetaDataFilter> MetaData = NewObject<UFieldSystemMetaDataFilter>();
	MetaData->SetMetaDataFilterType(
		EFieldFilterType::Field_Filter_All,
		EFieldObjectType::Field_Object_All,
		EFieldPositionType::Field_Position_CenterOfMass);
	

	
	TObjectPtr<URadialVector> ExplodingForce = NewObject<URadialVector>();
	ExplodingForce->Position = GetActorLocation();
	ExplodingForce->Magnitude = 50;
	shatterObject->ApplyPhysicsField(true, EGeometryCollectionPhysicsTypeEnum::Chaos_LinearVelocity, MetaData.Get(), ExplodingForce.Get());
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, direction.ToString());

	TObjectPtr<UUniformVector> pushingForce = NewObject<UUniformVector>();
	pushingForce->Magnitude = 300;
	pushingForce->Direction = direction;
	shatterObject->ApplyPhysicsField(true, EGeometryCollectionPhysicsTypeEnum::Chaos_LinearVelocity, MetaData.Get(), pushingForce);
}