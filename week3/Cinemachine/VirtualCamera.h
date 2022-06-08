// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "VirtualCamera.generated.h"

UENUM(BlueprintType)
enum class EBody : uint8
{
	DoNothing,
	ThirdPersonFollow,
	FramingTransposer,
	HardLockToTarget,
	OrbitalTransposer,
	TrackedDolly,
	Transposer
};

UENUM(BlueprintType)
enum class EHeading : uint8
{
	PositionDelta,
	Velocity,
	TargetForward,
	WorldForward
};

UENUM(BlueprintType)
enum class ECameraUp : uint8
{
	Default,
	Path,
	PathNoRoll,
	FollowTarget,
	FollowTargetNoRoll
};

UENUM(BlueprintType)
enum class EBindingMode : uint8
{
	LockToTargetOnAssign,
	LockToTargetWithWorldUp,
	LockToTargetNoRoll,
	LockToTarget,
	WorldSpace,
	SimpleFollowWithWorldUp
};

UCLASS()
class CINEMACHINE_API AVirtualCamera : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVirtualCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	class UCameraComponent* CameraComp;

	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere)
		AActor* followAt;
	
	UPROPERTY(VisibleAnywhere)
	FVector followVec;

	UPROPERTY(EditAnywhere)
		AActor* lookAt;

	UPROPERTY(EditAnywhere, Category="Body")
		EBody Body = EBody::DoNothing;
	
	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::ThirdPersonFollow || Body == EBody::FramingTransposer || Body == EBody::OrbitalTransposer || Body==EBody::TrackedDolly||Body == EBody::Transposer", EditConditionHides))
		FVector Damping;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::ThirdPersonFollow", EditConditionHides))
		FVector ShoulderOffset {80.f,-40.f,0.f};

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::ThirdPersonFollow", EditConditionHides))
		float VerticalArmLength{50.f};

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::ThirdPersonFollow", EditConditionHides, ClampMin = "0.0", ClampMax ="1.0"))
		float CameraSide;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::ThirdPersonFollow || Body == EBody::FramingTransposer", EditConditionHides))
		float CameraDistance;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::ThirdPersonFollow", EditConditionHides))
		TArray<TEnumAsByte<ECollisionChannel>> CameraCollisionFilter;

	//UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::ThirdPersonFollow", EditConditionHides))
		//FString ObstacleIgnoreTag;

	//UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::ThirdPersonFollow", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
		//float ObstacleCameraRadius;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides))
		FVector TrackedObjectOffset;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
		float LookaheadTime;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides))
		bool LookaheadIgnoreY;
	
	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
		float ScreenX;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
		float ScreenY;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float DeadZoneWidth;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float DeadZoneHeight;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides))
		float DeadZoneDepth;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides))
		bool UnlimitedSoftZone;

	//TODO: Clamp with deadzone
	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer&& !UnlimitedSoftZone", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float SoftZoneWidth;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer&& !UnlimitedSoftZone", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float SoftZoneHeight;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer&& !UnlimitedSoftZone", EditConditionHides, ClampMin = "-0.5", ClampMax = "0.5"))
		float BiasX;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer&& !UnlimitedSoftZone", EditConditionHides, ClampMin = "-0.5", ClampMax = "0.5"))
		float BiasY;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::HardLockToTarget", EditConditionHides))
		float DampingTime;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer||Body == EBody::Transposer", EditConditionHides))
		FVector FollowOffset;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float YawDamping;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides))
		EHeading HeadingDefinition;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides, ClampMin = "-180", ClampMax = "180"))
		int HeadingBias;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides))
		bool RecenterToTargetHeading;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides))
		float RecenterWaitTime;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer ||Body == EBody::TrackedDolly", EditConditionHides))
		float RecenterTime;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::TrackedDolly", EditConditionHides))
		TArray<FVector> Path;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::TrackedDolly", EditConditionHides))
		int PathPosition;															   														   
																					   
	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::TrackedDolly", EditConditionHides))
		FVector PathOffset;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::TrackedDolly", EditConditionHides))
		ECameraUp CameraUp;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::Transposer", EditConditionHides))
		EBindingMode BindingMode {EBindingMode::LockToTarget};

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::Transposer && BindingMode == EBindingMode::LockToTargetWithWorldUp", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float LUYawDamping;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::Transposer && BindingMode == EBindingMode::LockToTargetNoRoll", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float NRYawDamping;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::Transposer && BindingMode == EBindingMode::LockToTargetNoRoll", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float NRPitchDamping;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::Transposer && BindingMode == EBindingMode::LockToTarget", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float TRollDamping;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::Transposer && BindingMode == EBindingMode::LockToTarget", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float TYawDamping;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::Transposer && BindingMode == EBindingMode::LockToTarget", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float TPitchDamping;

	UPROPERTY(VisibleAnywhere)
	float length;

	UPROPERTY(VisibleAnywhere)
	FVector forwardv;
};