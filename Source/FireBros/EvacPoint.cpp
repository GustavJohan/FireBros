// Fill out your copyright notice in the Description page of Project Settings.


#include "EvacPoint.h"

#include "Components/BoxComponent.h"

// Sets default values
AEvacPoint::AEvacPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EvacPointBounds = CreateDefaultSubobject<UBoxComponent>("bounds");
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

