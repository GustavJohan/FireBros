// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyManager.generated.h"

USTRUCT(Blueprintable)
struct FPlayerMannequinInformation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)AActor* MannequinActor = nullptr;
	UPROPERTY(BlueprintReadOnly)int MannequinSlot;
};

UCLASS()
class FIREBROS_API ALobbyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere) TArray<UStaticMeshComponent*> PlayerBasePlates;
	TArray<AActor* ,TFixedAllocator<4>> PlayerModels;

	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* PlayerBasePlate0 = nullptr;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* PlayerBasePlate1 = nullptr;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* PlayerBasePlate2 = nullptr;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* PlayerBasePlate3 = nullptr; 

	UPROPERTY(BlueprintReadOnly) TMap<AController*, FPlayerMannequinInformation> PlayerManequins;
	TSet<int> occupiedSlots;
	
	UPROPERTY(EditAnywhere) TSubclassOf<AActor> PlayerModelClass;

	UPROPERTY(EditAnywhere) USkeletalMesh* characterMesh;

	UFUNCTION(BlueprintImplementableEvent) void SetPlayerName(AController* relevantPlayer);
	
	UFUNCTION(Server, Reliable) void AddPlayer	 (AController* relevantPlayer);
	UFUNCTION(Server, Reliable) void RemovePlayer(AController* relevantPlayer);
};
