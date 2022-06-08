// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class CINEMACHINE_API MathFormula
{
public:
	MathFormula();
	~MathFormula();

	static float GetVectorCos(const FVector& vec1, const FVector& vec2);
					   					    
	static float GetVectorSin(const FVector& vec1, const FVector& vec2);

	static float Dot(const FVector& vec1, const FVector& vec2);

	static float Cross(const FVector& vec1, const FVector& vec2);
	
	static FVector Normalize(const FVector& vec);

	static float Length(const FVector& vec);

	static FRotator YawRotate(const FVector& vec1, const FVector& vec2);
};
