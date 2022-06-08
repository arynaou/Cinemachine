// Fill out your copyright notice in the Description page of Project Settings.


#include "VirtualCamera.h"
#include "Kismet/KismetMathLibrary.h"
#include "Public/MathFormula.h"

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

void AVirtualCamera::OnConstruction(const FTransform& Transform)
{
	if (followAt != nullptr)
	{
		switch (Body)
		{
		case EBody::ThirdPersonFollow:
			SetActorLocation(followAt->GetActorLocation());
			AddActorWorldOffset(FVector(-ShoulderOffset.X, -ShoulderOffset.Y, -ShoulderOffset.Z + VerticalArmLength));
			SetActorLocation(FVector(GetActorLocation().X - ShoulderOffset.X * CameraDistance, GetActorLocation().Y + CameraSide * 2 * ((followAt->GetActorLocation().Y) - GetActorLocation().Y), GetActorLocation().Z));
			followVec = GetActorLocation() - followAt->GetActorLocation();
			for (size_t i = 0; i < CameraCollisionFilter.Num(); ++i)
			{
				StaticMeshComp->SetCollisionResponseToChannel(CameraCollisionFilter[i], ECollisionResponse::ECR_Block);
			}
			break;
		case EBody::HardLockToTarget:
			followVec = followAt->GetActorLocation();
			SetActorLocation(followVec);
			break;
		case EBody::Transposer:
			SetActorLocation(followAt->GetActorLocation());
			AddActorWorldOffset(-FollowOffset);
			followVec = GetActorLocation() - followAt->GetActorLocation();
			break;
		}
	}
}

// Called when the game starts or when spawned
void AVirtualCamera::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AVirtualCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (followAt != nullptr)
	{
		switch (Body) 
		{
		case EBody::ThirdPersonFollow:
			SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, followAt->GetActorLocation().X + followVec.X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, followAt->GetActorLocation().Y + followVec.Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, followAt->GetActorLocation().Z + followVec.Z, DeltaTime, Damping.Z)));
			break;
		case EBody::HardLockToTarget:
			SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, followAt->GetActorLocation().X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, followAt->GetActorLocation().Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, followAt->GetActorLocation().Z, DeltaTime, Damping.Z)));
			
			break;
		case EBody::Transposer:
			switch (BindingMode)
			{
				case EBindingMode::LockToTargetOnAssign:
					SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, followAt->GetActorLocation().X + followVec.X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, followAt->GetActorLocation().Y + followVec.Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, followAt->GetActorLocation().Z + followVec.Z, DeltaTime, Damping.Z)));
					break;
				case EBindingMode::LockToTargetWithWorldUp:
					SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, followAt->GetActorLocation().X + followVec.X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, followAt->GetActorLocation().Y + followVec.Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, followAt->GetActorLocation().Z + followVec.Z, DeltaTime, Damping.Z)));
					break;
			}
		}
	}

	if (lookAt != nullptr)
	{
		FVector lookAtLocation = lookAt->GetActorLocation();
		FVector forward = (lookAtLocation- GetActorLocation());
		FVector right = FVector::CrossProduct(FVector::UpVector, forward);

		SetActorRotation(UKismetMathLibrary::MakeRotFromXY(forward, right));
	}
}

