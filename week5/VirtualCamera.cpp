// Fill out your copyright notice in the Description page of Project Settings.


#include "VirtualCamera.h"
#include "Kismet/KismetMathLibrary.h"
#include "Public/MathFormula.h"
#include "Engine/GameEngine.h"
#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
#include "Components/SplineComponent.h"

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
	FlushPersistentDebugLines(GetWorld());
	if (followAt != nullptr)
	{
		switch (Body)
		{
		case EBody::ThirdPersonFollow:
			SetActorRotation(followAt->GetActorRotation());
			SetActorLocation(followAt->GetActorLocation());
			AddActorWorldOffset(FVector(-ShoulderOffset.X, -ShoulderOffset.Y, -ShoulderOffset.Z + VerticalArmLength));
			SetActorLocation(FVector(GetActorLocation().X - ShoulderOffset.X * CameraDistance, GetActorLocation().Y + CameraSide * 2 * ((followAt->GetActorLocation().Y) - GetActorLocation().Y), GetActorLocation().Z));
			followVec = GetActorLocation() - followAt->GetActorLocation();

			forwardv = followAt->GetActorForwardVector();
			length = MathFormula::Length(followVec);

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
			SetActorRotation(followAt->GetActorRotation());
			SetActorLocation(followAt->GetActorLocation());
			if (BindingMode == EBindingMode::LockToTargetOnAssign)
			{
				AddActorLocalOffset(FVector(-FollowOffset));
			}
			else
			{
				AddActorWorldOffset(FVector(-FollowOffset));
			}
			followVec = GetActorLocation() - followAt->GetActorLocation();

			forwardv = followAt->GetActorForwardVector();
			length = MathFormula::Length(followVec);
			break;
		case EBody::FramingTransposer:
			SetActorLocation(followAt->GetActorLocation());
			AddActorWorldOffset(FVector(-TrackedObjectOffset.X, 0,0));
			SetActorLocation(FVector(GetActorLocation().X - TrackedObjectOffset.X * CameraDistance, GetActorLocation().Y , GetActorLocation().Z));
			followVec = GetActorLocation() - followAt->GetActorLocation();

			AddActorWorldOffset(FVector(0,-TrackedObjectOffset.Y, -TrackedObjectOffset.Z));

			forwardv = followAt->GetActorForwardVector();
			length = MathFormula::Length(followVec);

			if (UnlimitedSoftZone)
			{
				SoftZoneWidth = 2;
				SoftZoneHeight = 2;
			}
			else
			{
				if (SoftZoneWidth < DeadZoneWidth)
				{
					DeadZoneWidth = SoftZoneWidth;
				}

				if (SoftZoneHeight < DeadZoneHeight)
				{
					DeadZoneHeight = SoftZoneHeight;
				}
			}

			break;
		case EBody::OrbitalTransposer:
			SetActorLocation(followAt->GetActorLocation());
			AddActorWorldOffset(FVector(-FollowOffset.X, 0, 0));
			SetActorLocation(FVector(GetActorLocation().X - FollowOffset.X * CameraDistance, GetActorLocation().Y, GetActorLocation().Z));
			followVec = GetActorLocation() - followAt->GetActorLocation();

			DrawCircle(GetWorld(), followAt->GetActorLocation(), followAt->GetActorForwardVector(), followAt->GetActorRightVector(), FColor::White, MathFormula::Length(followVec), 360,false, 5);

			//AddActorWorldOffset(FVector(0, -FollowOffset.Y, -FollowOffset.Z));

			if (AxisRangeMin > AxisStartValue)
			{
				AxisStartValue = AxisRangeMin;
			}
			else if (AxisRangeMax < AxisStartValue)
			{
				AxisStartValue = AxisRangeMax;
			}

			orbitalLength = MathFormula::Length(FVector2D(followAt->GetActorLocation().X - GetActorLocation().X, followAt->GetActorLocation().Y - GetActorLocation().Y));
			SetActorLocation(FVector(followAt->GetActorLocation().X - orbitalLength * FMath::Cos(AxisStartValue * 3.14 / 180), followAt->GetActorLocation().Y - orbitalLength * FMath::Sin(AxisStartValue * 3.14 / 180), GetActorLocation().Z));
			break;

		case EBody::TrackedDolly:
			if (Path != nullptr) 
			{
				SetActorLocation(Path->Spline->GetLocationAtDistanceAlongSpline(Path->Spline->GetSplineLength() * PathPosition, ESplineCoordinateSpace::World) + PathOffset);
				PathStartDistance = Path->Spline->GetSplineLength() * PathPosition;
				//SetActorLocation(Path->Spline->GetComponentLocation()+PathOffset);
				//SetActorLocation(Path->Spline->GetSplineLength())
			}
			break;
		}
	}
	else
	{
		Body = EBody::DoNothing;
	}

	if (lookAt == nullptr)
	{
		Aim = EAim::DoNothing;
	}
}

// Called when the game starts or when spawned
void AVirtualCamera::BeginPlay()
{
	Super::BeginPlay();

	BindToInput();
	GetWorld()->GetFirstPlayerController()->GetViewportSize(ScreenWidth, ScreenHeight);
	SetZone();

	bCanMoveCamera = true;
	StartTime = GetWorld()->GetTimeSeconds();
}


// Called every frame
void AVirtualCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//should not be here
	GetWorld()->GetFirstPlayerController()->GetViewportSize(ScreenWidth, ScreenHeight);
	//if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, GetScreenPosition(followAt->GetActorLocation()).ToString());

	if (followAt != nullptr)
	{
		forwardv = followAt->GetActorForwardVector();
		switch (Body)
		{
		case EBody::ThirdPersonFollow:
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), followAt->GetActorRotation(), DeltaTime, 50));
			SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, (followAt->GetActorLocation() - forwardv * length).X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, (followAt->GetActorLocation() - forwardv * length).Y - ShoulderOffset.Y + CameraSide * 2 * (followVec).Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, (followAt->GetActorLocation() - forwardv * length).Z - ShoulderOffset.Z + VerticalArmLength, DeltaTime, Damping.Z)));
			break;
		case EBody::HardLockToTarget:
			SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, followAt->GetActorLocation().X, DeltaTime, DampingTime), FMath::FInterpTo(GetActorLocation().Y, followAt->GetActorLocation().Y, DeltaTime, DampingTime), FMath::FInterpTo(GetActorLocation().Z, followAt->GetActorLocation().Z, DeltaTime, DampingTime)));
			break;
		case EBody::Transposer:
			switch (BindingMode)
			{
			case EBindingMode::LockToTargetOnAssign:
				SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, (followVec + followAt->GetActorLocation()).X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, (followVec + followAt->GetActorLocation()).Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, (followVec + followAt->GetActorLocation()).Z, DeltaTime, Damping.Z)));
				break;
			case EBindingMode::LockToTargetWithWorldUp:
				SetActorRotation(FRotator(0, FMath::FInterpTo(GetActorRotation().Yaw, followAt->GetActorRotation().Yaw, DeltaTime, LUYawDamping), 0));
				SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, (followAt->GetActorLocation() - forwardv * length).X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, (followAt->GetActorLocation() - forwardv * length - FollowOffset.Y).Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, (followAt->GetActorLocation() - forwardv * length).Z - FollowOffset.Z, DeltaTime, Damping.Z)));
				break;
			case EBindingMode::LockToTargetNoRoll:
				SetActorRotation(FRotator(FMath::FInterpTo(GetActorRotation().Pitch, followAt->GetActorRotation().Pitch, DeltaTime, NRPitchDamping), FMath::FInterpTo(GetActorRotation().Yaw, followAt->GetActorRotation().Yaw, DeltaTime, NRYawDamping), 0));
				SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, (followAt->GetActorLocation() - forwardv * length).X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, (followAt->GetActorLocation() - forwardv * length).Y - FollowOffset.Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, (followAt->GetActorLocation() - forwardv * length).Z - FollowOffset.Z, DeltaTime, Damping.Z)));
				break;
			case EBindingMode::LockToTarget:
				SetActorRotation(FMath::RInterpTo(GetActorRotation(), followAt->GetActorRotation(), DeltaTime, LUYawDamping));
				SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, (followAt->GetActorLocation() - forwardv * length).X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, (followAt->GetActorLocation() - forwardv * length).Y - FollowOffset.Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, (followAt->GetActorLocation() - forwardv * length).Z - FollowOffset.Z, DeltaTime, Damping.Z)));
				break;
			case EBindingMode::WorldSpace:
				SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, (followVec + followAt->GetActorLocation()).X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, (followVec + followAt->GetActorLocation()).Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, (followVec + followAt->GetActorLocation()).Z, DeltaTime, Damping.Z)));
				break;
			case EBindingMode::SimpleFollowWithWorldUp:
				FVector forward = (followAt->GetActorLocation() - GetActorLocation());
				FVector right = FVector::CrossProduct(FVector::UpVector, forward);

				SetActorRotation(FRotator(FMath::RInterpTo(GetActorRotation(), UKismetMathLibrary::MakeRotFromXY(forward, right), DeltaTime, 50)));
				if (MathFormula::Length(GetActorLocation() - followAt->GetActorLocation()) - length > 50)
				{
					AddActorWorldOffset(MathFormula::Normalize(followAt->GetActorLocation() - GetActorLocation()));
				}
				else if (MathFormula::Length(GetActorLocation() - followAt->GetActorLocation()) - length < -50)
				{
					AddActorWorldOffset(-(MathFormula::Normalize(followAt->GetActorLocation() - GetActorLocation())));
				}
				break;
			}
			break;
		case EBody::FramingTransposer:
			//SetZone();
			if (GetScreenPosition(followAt->GetActorLocation()).X <= 0 && GetScreenPosition(followAt->GetActorLocation()).Y <= 0)
			{
				SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, (followVec + followAt->GetActorLocation()).X, DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, (followVec + followAt->GetActorLocation()).Y - TrackedObjectOffset.Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, (followVec + followAt->GetActorLocation()).Z - TrackedObjectOffset.Z, DeltaTime, Damping.Z)));
			}
			else
			{
				if (GetScreenPosition(followAt->GetActorLocation()).X > DeadZoneX[0] && GetScreenPosition(followAt->GetActorLocation()).X < DeadZoneX[1] && GetScreenPosition(followAt->GetActorLocation()).Y>DeadZoneY[0] && GetScreenPosition(followAt->GetActorLocation()).Y < DeadZoneY[1])
				{
				}
				else if (GetScreenPosition(followAt->GetActorLocation()).X < SoftZoneX[0])
				{
					SetActorLocation(GetActorLocation() - followAt->GetActorRightVector() * 8);
				}
				else if (GetScreenPosition(followAt->GetActorLocation()).X > SoftZoneX[1])
				{
					SetActorLocation(GetActorLocation() + followAt->GetActorRightVector() * 8);
				}
				else
				{
					if (!LookaheadIgnoreY)
					{
						SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, (followVec + followAt->GetActorLocation()).X, DeltaTime, 1+Damping.X), FMath::FInterpTo(GetActorLocation().Y, (followVec + followAt->GetActorLocation()).Y - TrackedObjectOffset.Y, DeltaTime, 1+Damping.Y), FMath::FInterpTo(GetActorLocation().Z, (followVec + followAt->GetActorLocation()).Z - TrackedObjectOffset.Z, DeltaTime, 1+Damping.Z)));
					}
					else
					{
						SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X, (followVec + followAt->GetActorLocation()).X, DeltaTime, 1 + Damping.X), FMath::FInterpTo(GetActorLocation().Y, (followVec + followAt->GetActorLocation()).Y - TrackedObjectOffset.Y, DeltaTime, 1 + Damping.Y), FMath::FInterpTo(GetActorLocation().Z, (followVec + followAt->GetActorLocation()).Z - TrackedObjectOffset.Z, DeltaTime, 1 + Damping.Z)));
					}
				}
			}
			break;
		case EBody::OrbitalTransposer:
			SetActorLocation(FVector(followAt->GetActorLocation().X - orbitalLength * FMath::Cos(angle * 3.14 / 180), followAt->GetActorLocation().Y - orbitalLength * FMath::Sin(angle * 3.14 / 180), GetActorLocation().Z));
			break;
		case EBody::TrackedDolly:
			if (Path != nullptr)
			{
				if (bCanMoveCamera)
				{
					float CurrentSplineTime = (GetWorld()->GetTimeSeconds() - StartTime) / PathTime;
					float Distance = PathStartDistance + Path->Spline->GetSplineLength() * CurrentSplineTime;

					FVector Position = Path->Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
					SetActorLocation(FVector(FMath::FInterpTo(GetActorLocation().X,Position.X,DeltaTime, Damping.X), FMath::FInterpTo(GetActorLocation().Y, Position.Y, DeltaTime, Damping.Y), FMath::FInterpTo(GetActorLocation().Z, Position.Z, DeltaTime, Damping.Z)));

					if (lookAt == nullptr || Aim == EAim::DoNothing)
					{
						
						FVector Direction;
						switch (CameraUp)
						{
						case ECameraUp::Default:
							Direction = FVector(1,0,0);
							break;
						case ECameraUp::FollowTarget:
						case ECameraUp::FollowTargetNoRoll:
							if (followAt != nullptr)
							{
								Direction = followAt->GetActorForwardVector();
							}
							break;
						case ECameraUp::Path:
						case ECameraUp::PathNoRoll:
							Direction = Path->Spline->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
						}

						FRotator Rotator;
						switch (CameraUp)
						{
						case ECameraUp::Default:
						case ECameraUp::FollowTarget:
						case ECameraUp::Path:
							Rotator = FRotationMatrix::MakeFromX(Direction).Rotator();
							break;
						case ECameraUp::FollowTargetNoRoll:
						case ECameraUp::PathNoRoll:
							Rotator = FRotator(FRotationMatrix::MakeFromX(Direction).Rotator().Pitch, FRotationMatrix::MakeFromX(Direction).Rotator().Yaw, GetActorRotation().Roll);
							break;
						}
						SetActorRotation(Rotator);
					}

					if (CurrentSplineTime >= 1.0f)
					{
						if (Path->Spline->IsClosedLoop())
						{
							bCanMoveCamera = true;
							StartTime = GetWorld()->GetTimeSeconds();
							CurrentSplineTime = (GetWorld()->GetTimeSeconds() - StartTime) / PathTime;
						}
					}
				}
			}
		}
	}

	if (lookAt != nullptr)
	{
		FVector lookAtLocation;
		FVector forward;
		FVector right;
		switch (Aim)
		{
			case EAim::DoNothing:
				break;
			case EAim::Composer:
				if (!(GetScreenPosition(lookAt->GetActorLocation()).X > DeadZoneX[2] && GetScreenPosition(lookAt->GetActorLocation()).X < DeadZoneX[3] && GetScreenPosition(lookAt->GetActorLocation()).Y>DeadZoneY[2] && GetScreenPosition(lookAt->GetActorLocation()).Y < DeadZoneY[3]))
				{
					lookAtLocation = lookAt->GetActorLocation();
					forward = (lookAtLocation - GetActorLocation());
					right = FVector::CrossProduct(FVector::UpVector, forward);

					SetActorRotation(FMath::RInterpTo(GetActorRotation(), UKismetMathLibrary::MakeRotFromXY(forward, right),DeltaTime,1));
				}
				break;
			case EAim::GroupComposer:
				break;
			case EAim::HardLookAt:
				lookAtLocation = lookAt->GetActorLocation();
				forward = (lookAtLocation- GetActorLocation());
				right = FVector::CrossProduct(FVector::UpVector, forward);

				SetActorRotation(UKismetMathLibrary::MakeRotFromXY(forward, right));
				break;
			case EAim::POV:
				break;
			case EAim::SameAsFollowTarget:
				if (followAt != nullptr)
				{
					SetActorRotation(FMath::RInterpTo(GetActorRotation(), followAt->GetActorRotation(), DeltaTime,AimDamping));
				}
				break;
		}
	}
}

FVector2D AVirtualCamera::GetScreenPosition(const FVector& WorldLocation)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	FVector2D ScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation);

	int32 _ScreenX = (int32)ScreenLocation.X;
	int32 _ScreenY = (int32)ScreenLocation.Y;

	return FVector2D(_ScreenX, _ScreenY);
}

void AVirtualCamera::SetZone()
{
	if (UnlimitedSoftZone)
	{
		SoftZoneWidth = 2;
		SoftZoneHeight = 2;
	}

	//Set Dead zone
	DeadZoneX[0] = (ScreenWidth / 2) * (1 - DeadZoneWidth);
	DeadZoneX[1] = ScreenWidth - (ScreenWidth / 2) * (1 - DeadZoneWidth);
	DeadZoneY[0] = (ScreenHeight / 2) * (1 - DeadZoneHeight);
	DeadZoneY[1] = ScreenHeight - (ScreenHeight / 2) * (1 - DeadZoneHeight);
	DeadZoneX[2] = (ScreenWidth / 2) * (1 - AimDeadZoneWidth);
	DeadZoneX[3] = ScreenWidth - (ScreenWidth / 2) * (1 - AimDeadZoneWidth);
	DeadZoneY[2] = (ScreenHeight / 2) * (1 - AimDeadZoneHeight);
	DeadZoneY[3] = ScreenHeight - (ScreenHeight / 2) * (1 - AimDeadZoneHeight);

	//Set Soft zone
	SoftZoneX[0] = (ScreenWidth / 2) * (1 - SoftZoneWidth);
	SoftZoneX[1] = ScreenWidth - (ScreenWidth / 2) * (1 - SoftZoneWidth);
	SoftZoneY[0] = (ScreenHeight / 2) * (1 - SoftZoneHeight);
	SoftZoneY[1] = ScreenHeight - (ScreenHeight / 2) * (1 - SoftZoneHeight);
	SoftZoneX[2] = (ScreenWidth / 2) * (1 - AimSoftZoneWidth);
	SoftZoneX[3] = ScreenWidth - (ScreenWidth / 2) * (1 - AimSoftZoneWidth);
	SoftZoneY[2] = (ScreenHeight / 2) * (1 - AimSoftZoneHeight);
	SoftZoneY[3] = ScreenHeight - (ScreenHeight / 2) * (1 - AimSoftZoneHeight);

	FramingPivot = FVector2D(ScreenWidth * ScreenX, ScreenHeight * ScreenY);
	AimFramingPivot = FVector2D(ScreenWidth * AimScreenX, ScreenHeight * AimScreenY);

	SoftZoneX[0] += FramingPivot.X - ScreenWidth / 2;
	SoftZoneX[1] -= FramingPivot.X - ScreenWidth / 2;
	DeadZoneX[0] += FramingPivot.X - ScreenWidth / 2;
	DeadZoneX[1] -= FramingPivot.X - ScreenWidth / 2;
	SoftZoneX[2] += AimFramingPivot.X - ScreenWidth / 2;
	SoftZoneX[3] -= AimFramingPivot.X - ScreenWidth / 2;
	DeadZoneX[2] += AimFramingPivot.X - ScreenWidth / 2;
	DeadZoneX[3] -= AimFramingPivot.X - ScreenWidth / 2;

	SoftZoneY[0] += FramingPivot.Y - ScreenHeight / 2;
	SoftZoneY[1] -= FramingPivot.Y - ScreenHeight / 2;
	DeadZoneY[0] += FramingPivot.Y - ScreenHeight / 2;
	DeadZoneY[1] -= FramingPivot.Y - ScreenHeight / 2;
	SoftZoneY[2] += AimFramingPivot.Y - ScreenHeight / 2;
	SoftZoneY[3] -= AimFramingPivot.Y - ScreenHeight / 2;
	DeadZoneY[2] += AimFramingPivot.Y - ScreenHeight / 2;
	DeadZoneY[3] -= AimFramingPivot.Y - ScreenHeight / 2;
}

void AVirtualCamera::BindToInput()
{
	// Initialize our component
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	if (InputComponent)
	{
		InputComponent->BindAxis("Turn", this, &AVirtualCamera::MoveOrbital);
		InputComponent->BindAxis("Turn", this, &AVirtualCamera::MoveHorizontal);
		InputComponent->BindAxis("LookUp", this, &AVirtualCamera::MoveVertical);

		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}

void AVirtualCamera::MoveOrbital(float AxisValue)
{
	if (Body == EBody::OrbitalTransposer)
	{
		if (AxisValue > 0)
		{
			if (AxisRangeMax >= 360)
			{
				angle > AxisRangeMax ? angle = 0 : angle += AxisValue * AxisSpeed;
			}
			else
			{
				angle > AxisRangeMax ? angle = AxisRangeMax : angle += AxisValue * AxisSpeed;
			}
		}
		else if (AxisValue < 0)
		{
			if (AxisRangeMin <= 0)
			{
				angle < 0 ? angle = 359 : angle += AxisValue * AxisSpeed;
			}
			else
			{
				angle < AxisRangeMin ? angle = AxisRangeMin : angle += AxisValue * AxisSpeed;
			}
		}
	}
}

void AVirtualCamera::MoveVertical(float AxisValue)
{
	if (Aim == EAim::POV&& lookAt != nullptr)
	{
		if (AxisValue != 0)
		{
			if ((GetActorRotation().Pitch > VerticalAXisRangeMin && GetActorRotation().Pitch < VerticalAXisRangeMax) ||(GetActorRotation().Pitch < VerticalAXisRangeMax && AxisValue < 0) || GetActorRotation().Pitch > VerticalAXisRangeMin && AxisValue >0)
			{
				SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(GetActorRotation().Pitch - AxisValue, GetActorRotation().Yaw, GetActorRotation().Roll), GetWorld()->GetDeltaSeconds(), 10000* VerticalAXisSpeed));
			}
		}
	}
}

void AVirtualCamera::MoveHorizontal(float AxisValue)
{
	if (Aim == EAim::POV && lookAt != nullptr)
	{
		if (AxisValue != 0)
		{
			if ((GetActorRotation().Yaw > HorizontalAXisRangeMin && GetActorRotation().Yaw < HorizontalAXisRangeMax)|| (GetActorRotation().Yaw < HorizontalAXisRangeMax && AxisValue >0 )|| (GetActorRotation().Yaw > HorizontalAXisRangeMin && AxisValue < 0))
			{
				SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw + AxisValue, GetActorRotation().Roll), GetWorld()->GetDeltaSeconds(), 10000 * HorizontalAXisSpeed));
			}
		}
	}
}