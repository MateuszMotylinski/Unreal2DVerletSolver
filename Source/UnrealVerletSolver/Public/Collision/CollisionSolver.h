// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SolverActor.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CollisionSolver.generated.h"



/**
 * Base class for all collision solvers
 */
UCLASS(Abstract)
class UNREALVERLETSOLVER_API UCollisionSolver : public UObject
{
	GENERATED_BODY()
	
	public:
	virtual void InitialiseCollisionSolver(FParticlesData& arrParticles) {PR_ParticlesData = &arrParticles;};
	virtual void InsertsParticle(const FVector2D& vParticlePosition) {};

	// Calculates the new position after resolving collision
	virtual void UpdateParticleCollision(int32 iParticleIndex) {};

	protected:
	FParticlesData* PR_ParticlesData;

};
