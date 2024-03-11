// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpatialPartitioning/PointHashGrid2D.h"

#include "CoreMinimal.h"
#include "Collision/CollisionSolver.h"
#include "CollisionSolver_PointHashGrid2D.generated.h"

/**
 * 
 */
UCLASS()
class UNREALVERLETSOLVER_API UCollisionSolver_PointHashGrid2D : public UCollisionSolver
{
	GENERATED_BODY()
public:
	UCollisionSolver_PointHashGrid2D();
	virtual void InitialiseCollisionSolver(FParticlesData& arrParticles) override;
	virtual void UpdateParticleCollision(int32 iParticleIndex) override;
	virtual void InsertsParticle(int32 iParticleIndex, const FVector2D& vParticlePosition) override;
	virtual void DebugDraw() override;

	public:
	TSharedPtr<FPointHashGrid2D> m_spPointHashGrid2D;
};
