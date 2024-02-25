// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/CollisionSolver_PointHashGrid2D.h"



UCollisionSolver_PointHashGrid2D::UCollisionSolver_PointHashGrid2D()
: spPointHashGrid2D(nullptr)
{
	spPointHashGrid2D = MakeShareable<FPointHashGrid2D>(new FPointHashGrid2D);
}

void UCollisionSolver_PointHashGrid2D::InitialiseCollisionSolver(FParticlesData& arrParticles)
{
	Super::InitialiseCollisionSolver(arrParticles);


	int32 iPointIndex = 0;
	for (FVector2D vPos : arrParticles.arrPositions)
	{
		spPointHashGrid2D->InsertPoint(iPointIndex, FVector2f(vPos.X, vPos.Y));

		iPointIndex++;
	}
}

void UCollisionSolver_PointHashGrid2D::UpdateParticleCollision(int32 iParticleIndex)
{
	const FVector2D vPos = PR_ParticlesData->arrPositions[iParticleIndex];
	const FVector2D vPosPv = PR_ParticlesData->arrPositionsPrev[iParticleIndex];

	spPointHashGrid2D->UpdatePointUnsafe(iParticleIndex, FVector2f(vPosPv.X, vPosPv.Y), FVector2f(vPos.X, vPos.Y));

	TArray<int32> arrIndexes;
	spPointHashGrid2D->GetPointsInSphere(FVector2f(vPosPv.X, vPosPv.Y), PR_ParticlesData->m_fParticlesRadius * 3, arrIndexes);

	for (int32 iIndex : arrIndexes)
	{
		CheckAndHandleCollision2(iParticleIndex, iIndex);
	}
}

void UCollisionSolver_PointHashGrid2D::DebugDraw()
{
	spPointHashGrid2D->DebugDrawGrid(*this);
}

TArray<int32> UCollisionSolver_PointHashGrid2D::BroadPhase(int32 iParticleIndex)
{
	return TArray<int32>();
}
