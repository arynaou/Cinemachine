// Fill out your copyright notice in the Description page of Project Settings.


#include "MathFormula.h"
#include "Math/UnrealMathUtility.h"

MathFormula::MathFormula()
{
}

MathFormula::~MathFormula()
{
}

float MathFormula::Dot(const FVector& vec1, const FVector& vec2)
{
	return vec1.X * vec2.X+ vec1.Y * vec2.Y+ vec1.Z * vec2.Z;
}

float MathFormula::Cross(const FVector& vec1, const FVector& vec2)
{
	return vec1.Y * vec2.Z - vec1.Z * vec2.Y + vec1.Z * vec2.X - vec1.X * vec2.Z + vec1.X * vec2.Y - vec1.Y * vec2.X;
}

FVector MathFormula::Normalize(const FVector& vec)
{
	float length = MathFormula::Length(vec);
	return FVector(vec.X / length, vec.Y/length, vec.Z/length);
}

float MathFormula::Length(const FVector& vec)
{
	return FMath::Sqrt(vec.X * vec.X + vec.Y * vec.Y + vec.Z * vec.Z);
}

float MathFormula::GetVectorCos(const FVector& vec1, const FVector& vec2)
{
	return MathFormula::Dot(vec1, vec2) / (MathFormula::Length(vec1) * MathFormula::Length(vec2));
}

float MathFormula::GetVectorSin(const FVector& vec1, const FVector& vec2)
{
	return MathFormula::Cross(vec1,vec2)/ (MathFormula::Length(vec1) * MathFormula::Length(vec2));
}

FRotator MathFormula::YawRotate(const FVector& vec1, const FVector& vec2)
{
	FVector norVec1 = MathFormula::Normalize(vec1);
	FVector norVec2 = MathFormula::Normalize(vec2);

	float angle = FMath::Acos(MathFormula::Dot(norVec1, norVec2));
	if (!FMath::IsNearlyZero(angle))
	{
		angle = MathFormula::Cross(norVec1, norVec2) > 0 ? angle : -angle;
	}
	else
	{
		angle = 0;
	}
	return FRotator(0, angle, 0);
}