// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "VirtualCamera.generated.h"


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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	class UCameraComponent* CameraComp;

	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere)
		AActor* followAt;
	
	FVector followVec;

	UPROPERTY(EditAnywhere)
		AActor* lookAt;
};
