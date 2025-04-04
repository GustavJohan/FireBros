// Fill out your copyright notice in the Description page of Project Settings.


#include "FireSphere.h"

#include "FireManager.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFireSphere::AFireSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FireCollider = CreateDefaultSubobject<USphereComponent>("Fire Collider");
	FireCollider->SetupAttachment(RootComponent);
	FireMesh = CreateDefaultSubobject<UStaticMeshComponent>("Fire Mesh");
	FireMesh->SetupAttachment(FireCollider);
}

// Called when the game starts or when spawned
void AFireSphere::BeginPlay()
{
	Super::BeginPlay();

	FHitResult GroundCheck;
	if (GetWorld()->LineTraceSingleByChannel(GroundCheck, GetActorLocation(),
		GetActorLocation() + FVector::DownVector*MaxFireRadius*5, ECC_WorldStatic))
	{
		GroundLocation = GroundCheck.Location;
		SetActorLocation(GroundCheck.Location);
	}
	else
	{
		GroundLocation = GetActorLocation();
	}
}

// Called every frame
void AFireSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GrowFire(DeltaTime);
	

	for (auto Set : SpreadLocations)
	{
		DrawDebugSphere(GetWorld(), Set, 200, 8, FColor::Purple);
	}

	if (!UGameplayStatics::GetGameMode(GetWorld())){return;}
	
	if (CurrentFireRadius == MaxFireRadius && !fullyGrown)
	{
		if (!FireManager){FireManager = Cast<AFireManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFireManager::StaticClass()));}
		FireManager->AddFullyGrownFire(this);
		fullyGrown = true;
	}

	if (CurrentFireRadius != MaxFireRadius && fullyGrown)
	{
		if (!FireManager){FireManager = Cast<AFireManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFireManager::StaticClass()));}
		FireManager->RemoveFullyGrownFire(this);
		fullyGrown = false;
	}
}

void AFireSphere::GrowFire(float DeltaTime)
{
	if (CurrentFireRadius != MaxFireRadius)
	{
		ChangeFireSize(DeltaTime*FireGrowthRate);
		
	}
	//DrawDebugSphere(GetWorld(), FireCollider->GetComponentLocation(), CurrentFireRadius, 32, FColor::Green);
}

void AFireSphere::ChangeFireSize(float DeltaRadius)
{
	CurrentFireRadius += DeltaRadius;
	CurrentFireRadius = FMathf::Clamp(CurrentFireRadius, 0, MaxFireRadius);
    
	SetActorLocation(GroundLocation+FVector::UpVector*CurrentFireRadius);
	FireCollider->SetSphereRadius(CurrentFireRadius);
	//FireMesh->SetRelativeScale3D_Direct(FVector::One()*FireMeshScaler);
	FireMesh->SetRelativeScale3D(FVector::One()*FireMeshScaler*CurrentFireRadius);

	if (CurrentFireRadius == 0)
	{
		DestroyFire();
	}
}

TArray<FVector> AFireSphere::CheckSpreadLocations(TSet<AFireSphere*> Allfires)
{
	SpreadLocations.Empty();

	float degreeSteps = 360/NumSpreadLocations;
	degreeSteps *= FMathf::DegToRad; //convert to radians
	
	for (int i = 0; i < NumSpreadLocations; ++i)
	{
		float currentDegree = degreeSteps*i;

		FVector positionToCheck = GetActorLocation();
		
		positionToCheck.X += FMathf::Cos(currentDegree) * MaxFireRadius * 2;
		positionToCheck.Y +=	FMathf::Sin(currentDegree) * MaxFireRadius * 2;

		FVector WallCheck = GetActorLocation();

		WallCheck.X += FMathf::Cos(currentDegree) * MaxFireRadius * 3;
		WallCheck.Y += FMathf::Sin(currentDegree) * MaxFireRadius * 3;

		FHitResult WallCheckResult;
		if (GetWorld()->LineTraceSingleByChannel(WallCheckResult, GetActorLocation(),WallCheck, ECC_WorldStatic))
		{
			continue;
		}

		bool fireTooClose = false;
		for (AFireSphere* otherFire : Allfires)
		{
			if (otherFire == this){continue;}

			if (FVector::Dist(otherFire->GetActorLocation(), positionToCheck) < MaxFireRadius*2)
			{
				fireTooClose = true;
				break;
			}
		}

		if (fireTooClose){continue;}
		

		SpreadLocations.Add(positionToCheck);
	}

	return SpreadLocations;
}

void AFireSphere::DestroyFire()
{
	if (UGameplayStatics::GetGameMode(GetWorld()) && !persistent)
	{
		if (!FireManager){FireManager = Cast<AFireManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFireManager::StaticClass()));}
		FireManager->DestroyFireToServer(this);
	}
}
