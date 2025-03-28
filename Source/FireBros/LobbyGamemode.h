// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LobbyManager.h"
#include "GameFramework/GameMode.h"
#include "LobbyGamemode.generated.h"

/**
 * 
 */
UCLASS()
class FIREBROS_API ALobbyGamemode : public AGameMode
{
	GENERATED_BODY()

	void BeginPlay() override;
	
	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

	ALobbyManager* LobbyManager;

	UPROPERTY(EditDefaultsOnly) TSubclassOf<ALobbyManager> LobbyManagerClass;

	
	UPROPERTY(EditAnywhere) TSubclassOf<AActor> PlayerModelClass;


	int playerAmount = 0;
};
