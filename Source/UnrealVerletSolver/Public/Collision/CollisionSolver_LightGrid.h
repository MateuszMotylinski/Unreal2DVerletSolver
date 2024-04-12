// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpatialPartitioning/Lightgrid/include/lightgrid/grid.hpp"

#include "CoreMinimal.h"
#include "Collision/CollisionSolver.h"
#include "CollisionSolver_LightGrid.generated.h"

/**
 * 
 */
UCLASS()
class UNREALVERLETSOLVER_API UCollisionSolver_LightGrid : public UCollisionSolver
{
	GENERATED_BODY()
public:
	UCollisionSolver_LightGrid();
	virtual void InitialiseCollisionSolver(FParticlesData& arrParticles) override;
	virtual void UpdateParticleCollision(int32 iParticleIndex) override;
	virtual void InsertsParticle(int32 iParticleIndex, const FVector2D& vParticlePosition) override;
	virtual void DebugDraw() override;

	public:
	//lightgrid::grid<FVector2D> m_xGrid;
	lightgrid::grid<int32> m_xGrid;


	TArray<int> m_arrNodes;
};
