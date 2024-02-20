// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Collision/CollisionSolver.h"
#include "CollisionSolver_Naive.generated.h"

/**
 * Naive Collision Solver
 * Simply check collision against all particles (On2 complexity)
 */
UCLASS()
class UNREALVERLETSOLVER_API UCollisionSolver_Naive : public UCollisionSolver
{
	GENERATED_BODY()

	public:

	UCollisionSolver_Naive();
	virtual void UpdateParticleCollision(int32 iParticleIndex) override;
	
	

};
