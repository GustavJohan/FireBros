// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGamemode.h"

#include "Kismet/GameplayStatics.h"

void ALobbyGamemode::BeginPlay()
{
	Super::BeginPlay();

	
}


void ALobbyGamemode::PostLogin(APlayerController* NewPlayer)
{
	if (!LobbyManager)
	{
		LobbyManager = Cast<ALobbyManager>(GetWorld()->SpawnActor(LobbyManagerClass));
		LobbyManager->SetOwner(GetWorld()->GetFirstPlayerController());
	}
	

	  
	LobbyManager->AddPlayer(NewPlayer);
	playerAmount++;
	
	Super::PostLogin(NewPlayer);
}

void ALobbyGamemode::Logout(AController* Exiting)
{
	playerAmount--;
	LobbyManager->RemovePlayer(Exiting);
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE,5, FColor::Green, "player leave");

	
	Super::Logout(Exiting);
}

