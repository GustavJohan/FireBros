// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyManager.h"

// Sets default values
ALobbyManager::ALobbyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    /*for (int i = 0; i < 4; ++i)
    {
	    PlayerBasePlates.Add(CreateDefaultSubobject<UStaticMeshComponent>("player platform " +  i));
    	PlayerBasePlates[i]->SetupAttachment(RootComponent);
    }*/

	PlayerBasePlate0 = CreateDefaultSubobject<UStaticMeshComponent>("player platform 0");
	PlayerBasePlate1 = CreateDefaultSubobject<UStaticMeshComponent>("player platform 1");
	PlayerBasePlate2 = CreateDefaultSubobject<UStaticMeshComponent>("player platform 2");
	PlayerBasePlate3 = CreateDefaultSubobject<UStaticMeshComponent>("player platform 3");

	PlayerBasePlate0->SetupAttachment(RootComponent);
	PlayerBasePlate1->SetupAttachment(RootComponent);
	PlayerBasePlate2->SetupAttachment(RootComponent);
	PlayerBasePlate3->SetupAttachment(RootComponent);

	PlayerBasePlates.Empty();
}

// Called when the game starts or when spawned
void ALobbyManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALobbyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyManager::AddPlayer_Implementation(AController* relevantPlayer)
{
	if (!PlayerBasePlates[0]) {PlayerBasePlates[0] = PlayerBasePlate0;}
	if (!PlayerBasePlates[1]) {PlayerBasePlates[1] = PlayerBasePlate1;}
	if (!PlayerBasePlates[2]) {PlayerBasePlates[2] = PlayerBasePlate2;}
	if (!PlayerBasePlates[3]) {PlayerBasePlates[3] = PlayerBasePlate3;}
	
	FPlayerMannequinInformation PlayerMannequinInformation;

	for (int i = 0; i < 4; ++i)
	{
		if (!occupiedSlots.Contains(i))
		{
			occupiedSlots.Add(i);
			PlayerMannequinInformation.MannequinActor = GetWorld()->SpawnActor(PlayerModelClass, &PlayerBasePlates[i]->GetComponentTransform());
			PlayerMannequinInformation.MannequinSlot = i;
			PlayerManequins.Add(relevantPlayer, PlayerMannequinInformation);
			SetPlayerName(relevantPlayer);
			return;
		}
	}
}

void ALobbyManager::RemovePlayer_Implementation(AController* relevantPlayer)
{
	FPlayerMannequinInformation MannequinInformation = PlayerManequins[relevantPlayer];
	PlayerManequins.Remove(relevantPlayer);
	
	MannequinInformation.MannequinActor->Destroy();
	occupiedSlots.Remove(MannequinInformation.MannequinSlot);
}
