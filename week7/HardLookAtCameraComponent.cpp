// Fill out your copyright notice in the Description page of Project Settings.


#include "HardLookAtCameraComponent.h"

// Sets default values for this component's properties
UHardLookAtCameraComponent::UHardLookAtCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHardLookAtCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("debug"));
	// ...
	Spawn();
	
}


// Called every frame
void UHardLookAtCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHardLookAtCameraComponent::Spawn()
{
	UWorld* TheWorld = GetWorld();
	if (TheWorld != nullptr)
	{
		FTransform ComponentTransform(this->GetComponentTransform());
		TheWorld->SpawnActor(ActorToSpawn, &ComponentTransform);
	}
}

