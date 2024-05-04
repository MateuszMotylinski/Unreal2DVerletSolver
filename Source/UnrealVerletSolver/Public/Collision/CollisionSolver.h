// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SolverActor.h"
#include "Stats/StatDeclarations.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CollisionSolver.generated.h"

DECLARE_STATS_GROUP(TEXT("CollisionSolver"), STATGROUP_CollisionSolver, STATCAT_Advanced);
// DisplayName, GroupName (ends up as: "LODZERO"), Third param is always Advanced.

// Keep track of the amount of Actors spawned at runtime (at the top of my class file)
DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("Collisions"), STAT_CollisionsNum, STATGROUP_UnrealVerletSolver);

DECLARE_CYCLE_STAT(TEXT("CollisionResolution"), STAT_CollisionResolution, STATGROUP_UnrealVerletSolver);
DECLARE_CYCLE_STAT(TEXT("Collision_UpdateParticlePosition"), STAT_Collision_UpdateParticlePosition, STATGROUP_UnrealVerletSolver);
DECLARE_CYCLE_STAT(TEXT("Collision_FindNeighbours"), STAT_Collision_FindNeighbours, STATGROUP_UnrealVerletSolver);
DECLARE_CYCLE_STAT(TEXT("Collision_ResolveCollisions"), STAT_Collision_ResolveCollisions, STATGROUP_UnrealVerletSolver);

/**
 * Base class for all collision solvers
 */
UCLASS(Abstract)
class UNREALVERLETSOLVER_API UCollisionSolver : public UObject
{
	GENERATED_BODY()

	public:
	virtual void InitialiseCollisionSolver(FParticlesData& arrParticles) {PR_ParticlesData = &arrParticles;};
	virtual void InsertsParticle(int32 iParticleIndex,  const FVector2D& vParticlePosition) {};

	// Calculates the new position after resolving collision
	virtual void UpdateParticleCollision(int32 iParticleIndex)
	{
		SCOPE_CYCLE_COUNTER(STAT_CollisionResolution);
	};

	virtual void DebugDraw() {};

	protected:
	// Function to check if two particles are colliding
	bool IsColliding_CircleCircle(int32 iParticleIndex1, int32 iParticleIndex2);

	void HandleCollision(int32 iParticleIndex1, int32 iParticleIndex2);
	void CheckAndHandleCollision(int32 iParticleIndex1, int32 iParticleIndex2);

	protected:
	FParticlesData* PR_ParticlesData;

};
