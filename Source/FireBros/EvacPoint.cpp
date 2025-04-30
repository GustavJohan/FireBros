// Fill out your copyright notice in the Description page of Project Settings.


#include "EvacPoint.h"

#include "CivilianCharacter.h"
#include "GameManager.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEvacPoint::AEvacPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EvacPointBounds = CreateDefaultSubobject<UBoxComponent>("bounds");
	EvacPointBounds->OnComponentBeginOverlap.AddDynamic(this, &AEvacPoint::HandleBeginOverlap);
}

// Called when the game starts or when spawned
void AEvacPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEvacPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AEvacPoint::GetPresentCivilians()
{
	TArray<AActor*> PresentCivilians;
	EvacPointBounds->GetOverlappingActors(PresentCivilians, ACivilianCharacter::StaticClass());

	return PresentCivilians.Num();
}

void AEvacPoint::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ACivilianCharacter>() && UGameplayStatics::GetGameMode(GetWorld()))
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Update win status");
		AActor* gamemanager = UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass());
		if (gamemanager)
		{
			Cast<AGameManager>(gamemanager)->CheckWin();
		}
		
	}
}
