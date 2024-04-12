// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/CollisionSolver_LightGrid.h"



UCollisionSolver_LightGrid::UCollisionSolver_LightGrid()
{
}

void UCollisionSolver_LightGrid::InitialiseCollisionSolver(FParticlesData& arrParticles)
{
	Super::InitialiseCollisionSolver(arrParticles);

	//m_xGrid.clear();
	//m_xGrid.init(900, 900, 2);

	//m_xGrid.reserve(1000);

	int32 iPointIndex = 0;
	for (FVector2D vPos : arrParticles.arrPositions)
	{

		float fRadius = arrParticles.arrParticlesRadius[iPointIndex];
		lightgrid::bounds xBounds;
		xBounds.x = vPos.X;
		xBounds.w = fRadius;
		xBounds.y = vPos.Y;
		xBounds.h = fRadius;

		//m_xGrid.insert(vPos, xBounds);
		auto i = m_xGrid.insert(iPointIndex, xBounds);

		m_arrNodes.Add(i);
		iPointIndex++;
	}
}

void UCollisionSolver_LightGrid::UpdateParticleCollision(int32 iParticleIndex)
{
	const FVector2D vPos = PR_ParticlesData->arrPositions[iParticleIndex];
	const FVector2D vPosPv = PR_ParticlesData->arrPositionsPrev[iParticleIndex];
	float fRadius = PR_ParticlesData->arrParticlesRadius[iParticleIndex];

	int iNode = m_arrNodes[iParticleIndex];

	//lightgrid::bounds xBoundsOld;
	//xBoundsOld.x = vPosPv.X - fRadius / 10;
	//xBoundsOld.w = vPosPv.X + fRadius / 10;
	//xBoundsOld.y = vPosPv.Y - fRadius / 10;
	//xBoundsOld.h = vPosPv.Y + fRadius / 10;
	//lightgrid::bounds xBoundsNew;
	//xBoundsNew.x = vPos.X - fRadius / 10;
	//xBoundsNew.w = vPos.X + fRadius / 10;
	//xBoundsNew.y = vPos.Y - fRadius / 10;
	//xBoundsNew.h = vPos.Y + fRadius / 10;

	lightgrid::bounds xBoundsOld;
	xBoundsOld.x = vPosPv.X;
	xBoundsOld.w = fRadius;
	xBoundsOld.y = vPosPv.Y;
	xBoundsOld.h = fRadius;
	lightgrid::bounds xBoundsNew;
	xBoundsNew.x = vPos.X;
	xBoundsNew.w = fRadius;
	xBoundsNew.y = vPos.Y;
	xBoundsNew.h = fRadius;

	/*if (xBoundsNew.x == xBoundsOld.x
	&& xBoundsNew.y == xBoundsOld.y
	&& xBoundsNew.w == xBoundsOld.w
	&& xBoundsNew.h == xBoundsOld.h)
	{

	}
	else
	{
		m_xGrid.update(iParticleIndex, xBoundsOld, xBoundsNew);
	}*/
	m_xGrid.update(iNode, xBoundsOld, xBoundsNew);

	lightgrid::bounds xBoundsQuery;
	xBoundsQuery.x = vPos.X - fRadius;
	xBoundsQuery.w = fRadius * 2;
	xBoundsQuery.y = vPos.Y - fRadius;
	xBoundsQuery.h = fRadius * 2;

	std::vector<int32> arrIndexes;
	m_xGrid.query(xBoundsQuery, arrIndexes);

	for (int32 iIndex : arrIndexes)
	{
		CheckAndHandleCollision(iParticleIndex, iIndex);
	}
}

void UCollisionSolver_LightGrid::InsertsParticle(int32 iParticleIndex, const FVector2D& vParticlePosition)
{
	//m_spLightGrid->InsertPoint(iParticleIndex, FVector2f(vParticlePosition.X, vParticlePosition.Y));

	float fRadius = PR_ParticlesData->arrParticlesRadius[iParticleIndex];
	lightgrid::bounds xBounds;
	xBounds.x = vParticlePosition.X;
	xBounds.w =  fRadius;
	xBounds.y = vParticlePosition.Y;
	xBounds.h = fRadius;

	//m_xGrid.insert(vPos, xBounds);
	int i = m_xGrid.insert(iParticleIndex, xBounds);

	m_arrNodes.Add(i);
}

void UCollisionSolver_LightGrid::DebugDraw()
{
	//m_spLightGrid->DebugDrawGrid(*this);
}

