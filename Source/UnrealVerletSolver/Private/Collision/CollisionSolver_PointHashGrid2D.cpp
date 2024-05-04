// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/CollisionSolver_PointHashGrid2D.h"



UCollisionSolver_PointHashGrid2D::UCollisionSolver_PointHashGrid2D()
: m_spPointHashGrid2D(nullptr)
{
	m_spPointHashGrid2D = MakeShareable<FPointHashGrid2D>(new FPointHashGrid2D);
}

void UCollisionSolver_PointHashGrid2D::InitialiseCollisionSolver(FParticlesData& arrParticles)
{
	Super::InitialiseCollisionSolver(arrParticles);


	int32 iPointIndex = 0;
	for (FVector2D vPos : arrParticles.arrPositions)
	{
		m_spPointHashGrid2D->InsertPoint(iPointIndex, FVector2f(vPos.X, vPos.Y));

		iPointIndex++;
	}
}

void UCollisionSolver_PointHashGrid2D::UpdateParticleCollision(int32 iParticleIndex)
{
	SCOPE_CYCLE_COUNTER(STAT_CollisionResolution);

	const FVector2D vPos = PR_ParticlesData->arrPositions[iParticleIndex];
	const FVector2D vPosPv = PR_ParticlesData->arrPositionsPrev[iParticleIndex];
	float fRadius = PR_ParticlesData->arrParticlesRadius[iParticleIndex];

	{
		SCOPE_CYCLE_COUNTER(STAT_Collision_UpdateParticlePosition);
		m_spPointHashGrid2D->UpdatePointUnsafe(iParticleIndex, FVector2f(vPosPv.X, vPosPv.Y), FVector2f(vPos.X, vPos.Y));
	}

	TArray<int32> arrIndexes;
	{
		SCOPE_CYCLE_COUNTER(STAT_Collision_FindNeighbours);
		m_spPointHashGrid2D->GetPointsInSphere(FVector2f(vPosPv.X, vPosPv.Y), fRadius * 2, arrIndexes);
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_Collision_ResolveCollisions);
		for (int32 iIndex : arrIndexes)
		{
			CheckAndHandleCollision(iParticleIndex, iIndex);
		}
	}
}

void UCollisionSolver_PointHashGrid2D::InsertsParticle(int32 iParticleIndex, const FVector2D& vParticlePosition)
{
	m_spPointHashGrid2D->InsertPoint(iParticleIndex, FVector2f(vParticlePosition.X, vParticlePosition.Y));
}

void UCollisionSolver_PointHashGrid2D::DebugDraw()
{
	m_spPointHashGrid2D->DebugDrawGrid(*this);
}

