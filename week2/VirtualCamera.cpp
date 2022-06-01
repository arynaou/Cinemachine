// Fill out your copyright notice in the Description page of Project Settings.


#include "VirtualCamera.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AVirtualCamera::AVirtualCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	StaticMeshComp = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	StaticMeshComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(StaticMeshComp);


}

// Called when the game starts or when spawned
void AVirtualCamera::BeginPlay()
{
	Super::BeginPlay();

	if (followAt != nullptr)
	{
		followVec = GetActorLocation() - (*followAt).GetActorLocation();
	}
}

// Called every frame
void AVirtualCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (followAt != nullptr)
	{
		SetActorLocation(followVec + followAt->GetActorLocation());
	}

	if (lookAt != nullptr)
	{
		FVector lookAtLocation = lookAt->GetActorLocation();
		FVector forward = (lookAtLocation- GetActorLocation());
		FVector right = FVector::CrossProduct(FVector::UpVector, forward);

		SetActorRotation(UKismetMathLibrary::MakeRotFromXY(forward, right));
	}
}

