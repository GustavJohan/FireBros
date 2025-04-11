// Fill out your copyright notice in the Description page of Project Settings.


#include "FireManager.h"

#include "FireSphere.h"
#include "NavModifierVolume.h"
#include "Kismet/GameplayStatics.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "NavMesh/NavMeshBoundsVolume.h"

// Sets default values
AFireManager::AFireManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFireManager::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> arr;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFireSphere::StaticClass(), arr);

	for (AActor* act : arr)
	{
		Fires.Add(Cast<AFireSphere>(act));
	}

	if (!UGameplayStatics::GetGameMode(GetWorld())){return;}
	
	for (int i = 0; i < FireSpawnInfos.Num(); ++i)
	{
		FTimerHandle handle;
		
		FVector firelocation = FireSpawnInfos[i].FireStartLocation->GetActorLocation();

		GetWorldTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([this, firelocation]{SpawnFireAtLocation(firelocation);}), FireSpawnInfos[i].FireStartTimestamp, false);
		
		fireLocationTimer.Add(handle);
	}
}

// Called every frame
void AFireManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireSpreadClock += DeltaTime;

	if (FireSpreadClock > FireSpreadRate)
	{
		FireSpreadClock = 0;
		if (UGameplayStatics::GetGameMode(GetWorld())){SpreadFire();}
	}
}

void AFireManager::AddFullyGrownFire(AFireSphere* fire)
{
	FullyGrownFires.Add(fire);
	//incase the fire is not already in the set somehow
	Fires.Add(fire);
}

void AFireManager::RemoveFullyGrownFire(AFireSphere* fire)
{
	FullyGrownFires.Remove(fire);
	//incase the fire is not already in the set somehow
	Fires.Add(fire);
}


void AFireManager::SpreadFire_Implementation()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE,5, FColor::Green, "fire spreading");
	
	for (AFireSphere* FiresToSpread : FullyGrownFires)
	{
		TArray<FVector> spreadLocations = FiresToSpread->CheckSpreadLocations(Fires);

		if (spreadLocations.IsEmpty()){continue;}
		
		int random = rand() % spreadLocations.Num();

		Fires.Add(Cast<AFireSphere>(GetWorld()->SpawnActor(fireClass, &spreadLocations[random])));
	}

	
}

void AFireManager::SpawnFireAtLocation_Implementation(FVector location)
{

	/*
	ANavModifierVolume* AvoidVolume = Cast<ANavModifierVolume>(GetWorld()->SpawnActor(ANavModifierVolume::StaticClass(), &location));
	AvoidVolume->SetAreaClass(UNavArea_Obstacle::StaticClass());
	AvoidVolume->Brush.set
	*/
	
	Fires.Add(Cast<AFireSphere>(GetWorld()->SpawnActor(fireClass, &location)));
}

void AFireManager::DestroyFireToServer_Implementation(AFireSphere* sphere)
{
	Fires.Remove(sphere);
	sphere->Destroy();
}
