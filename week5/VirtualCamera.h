// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Public/SplineController.h"
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

UENUM(BlueprintType)
enum class EAim : uint8
{
	DoNothing,
	Composer,
	GroupComposer,
	HardLookAt,
	POV,
	SameAsFollowTarget
};

UENUM(BlueprintType)
enum class EFramingMode : uint8
{
	Horizontal,
	Vertical,
	HorizontalAndVertical
};

UENUM(BlueprintType)
enum class EAdjustmentMode : uint8
{
	ZoomOnly,
	DollyOnly,
	DollyThenZoom
};

UENUM(BlueprintType)
enum class ERecenterTarget : uint8
{
	None,
	FollowTargetForward,
	LookAtTargetForward
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
	
	UPROPERTY()
	FVector followVec;

	UPROPERTY(EditAnywhere)
		AActor* lookAt;

	UPROPERTY(EditAnywhere, Category="Body", meta = (EditCondition = "followAt!=nullptr"))
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

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides))
		FVector TrackedObjectOffset;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
		float LookaheadTime;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides))
		bool LookaheadIgnoreY;
	
	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
		float ScreenX{0.5};

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
		float ScreenY{0.5};

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float DeadZoneWidth;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float DeadZoneHeight;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer", EditConditionHides))
		bool UnlimitedSoftZone;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer&& !UnlimitedSoftZone", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float SoftZoneWidth;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::FramingTransposer&& !UnlimitedSoftZone", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float SoftZoneHeight;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::HardLockToTarget", EditConditionHides))
		float DampingTime;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer||Body == EBody::Transposer", EditConditionHides))
		FVector FollowOffset;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float YawDamping;

	//UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides))
	//	bool RecenterToTargetHeading;

	//UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides))
	//	float RecenterWaitTime;

	//UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer ||Body == EBody::TrackedDolly", EditConditionHides))
	//	float RecenterTime;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "360.0"))
		int AxisStartValue{0};

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "360.0"))
		float AxisRangeMin {0};

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides, ClampMin = "0.0", ClampMax = "360.0"))
		float AxisRangeMax{ 360 };

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::OrbitalTransposer", EditConditionHides,ClampMin ="0",ClampMax = "10"))
		float AxisSpeed{1};

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::TrackedDolly", EditConditionHides))
		ASplineController* Path;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::TrackedDolly", EditConditionHides, ClampMin = "0", ClampMax = "1"))
		float PathPosition;															   														   
																					   
	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::TrackedDolly", EditConditionHides))
		FVector PathOffset;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::TrackedDolly", EditConditionHides))
		ECameraUp CameraUp;

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::Transposer", EditConditionHides))
		EBindingMode BindingMode {EBindingMode::LockToTarget};

	UPROPERTY(EditAnywhere, Category = "Body", meta = (EditCondition = "Body == EBody::TrackedDolly", EditConditionHides, ClampMin = "0"))
		float PathTime;

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
	
	UPROPERTY()
		float PathStartDistance;

	UPROPERTY()
	float length;

	UPROPERTY()
	FVector forwardv;

	UPROPERTY()
		int32 ScreenWidth;

	UPROPERTY()
		int32 ScreenHeight;

	UPROPERTY()
		FVector2D FramingPivot;

	UPROPERTY()
		FVector2D AimFramingPivot;

	UPROPERTY()
		float DeadZoneX[4];

	UPROPERTY()
		float DeadZoneY[4];

	UPROPERTY()
		float SoftZoneX[4];

	UPROPERTY()
		float SoftZoneY[4];

	UPROPERTY()
		float angle;

	UPROPERTY()
		float orbitalLength;

	UPROPERTY()
		bool bCanMoveCamera;

	UPROPERTY()
	float StartTime;

	FVector2D GetScreenPosition(const FVector& WorldLocation);

	void SetZone();

	void BindToInput();

	void MoveOrbital(float AxisValue);

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "lookAt!=nullptr"))
		EAim Aim = EAim::DoNothing;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer", EditConditionHides))
		FVector AimingObjectOffset;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
		float AimAheadTime;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0", ClampMax = "30.0"))
		float AimAheadSmoothing;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides))
		bool AimAheadIgnoreY;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "-0.5", ClampMax = "1.5"))
		float AimScreenX;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "-0.5", ClampMax = "1.5"))
		float AimScreenY;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float AimDeadZoneWidth;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float AimDeadZoneHeight;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float AimSoftZoneWidth;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0", ClampMax = "2.0"))
		float AimSoftZoneHeight;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "-0.5", ClampMax = "0.5"))
		float AimBiaX;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "-0.5", ClampMax = "0.5"))
		float AimBiaY;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim==EAim::Composer || Aim == EAim::GroupComposer", EditConditionHides))
		bool CenterOnActivate;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float HorizontalDamping;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float VerticalDamping;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0"))
		float GroupFramingSize;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer", EditConditionHides))
		EFramingMode FramingMode;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer", EditConditionHides, ClampMin = "0.0", ClampMax = "20.0"))
		float FramingDampling;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer", EditConditionHides))
		EAdjustmentMode AdjustmentMode;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer && AdjustmentMode==EAdjustmentMode::ZoomOnly", EditConditionHides, ClampMin = "0.0", ClampMax = "179.0"))
		float MinimumFOV;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer && AdjustmentMode==EAdjustmentMode::ZoomOnly", EditConditionHides, ClampMin = "0.0", ClampMax = "179.0"))
		float MaximumFOV;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer && AdjustmentMode==EAdjustmentMode::DollyOnly", EditConditionHides, ClampMin = "0.0"))
		float MaxDollyIn{5000};

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer && AdjustmentMode==EAdjustmentMode::DollyOnly", EditConditionHides, ClampMin = "0.0"))
		float MaxDollyOut{5000};

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer && AdjustmentMode==EAdjustmentMode::DollyOnly", EditConditionHides, ClampMin = "0.0"))
		float MinimumDistance{ 0 };

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::GroupComposer && AdjustmentMode==EAdjustmentMode::DollyOnly", EditConditionHides, ClampMin = "0.0"))
		float MaximumDistance{ 5000 };

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides))
		ERecenterTarget RecenterTarget;

	UPROPERTY(EditAnywhere, Category = "Aim|VerticalAxis", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides))
		float VerticalAXisValue{0};

	UPROPERTY(EditAnywhere, Category = "Aim|VerticalAxis", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides))
		float VerticalAXisRangeMin;

	UPROPERTY(EditAnywhere, Category = "Aim|VerticalAxis", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides))
		float VerticalAXisRangeMax;

	UPROPERTY(EditAnywhere, Category = "Aim|VerticalAxis", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides, ClampMin = "0.0",ClampMax ="10.0"))
		float VerticalAXisSpeed;

	//UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides))
	//	bool VerticalRecenteringEnabled{false};

	//UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides, ClampMin = "0.0"))
	//	float VerticalWaitTime;

	//UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides, ClampMin = "0.0"))
	//	float VerticalRecenteringTime;

	UPROPERTY(EditAnywhere, Category = "Aim|HorizontalAxis", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides, ClampMin = "-90.0", ClampMax = "90.0"))
		float HorizontalAXisValue;

	UPROPERTY(EditAnywhere, Category = "Aim|HorizontalAxis", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides))
		float HorizontalAXisRangeMin;

	UPROPERTY(EditAnywhere, Category = "Aim|HorizontalAxis", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides))
		float HorizontalAXisRangeMax;

	UPROPERTY(EditAnywhere, Category = "Aim|HorizontalAxis", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides, ClampMin = "0.0", ClampMax = "10.0"))
		float HorizontalAXisSpeed;

	//UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides))
	//	bool HorizontalRecenteringEnabled{ false };

	//UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides, ClampMin = "0.0"))
	//	float HorizontalWaitTime;

	//UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::POV", EditConditionHides, ClampMin = "0.0"))
	//	float HorizontalRecenteringTime;

	UPROPERTY(EditAnywhere, Category = "Aim", meta = (EditCondition = "Aim == EAim::SameAsFollowTarget", EditConditionHides))
		float AimDamping;

	void MoveVertical(float AxisValue);

	void MoveHorizontal(float AxisValue);

	//UPROPERTY(VisibleAnywhere)
		//float test;
};