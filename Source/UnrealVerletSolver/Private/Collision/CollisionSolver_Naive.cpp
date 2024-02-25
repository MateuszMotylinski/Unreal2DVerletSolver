// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/CollisionSolver_Naive.h"

UCollisionSolver_Naive::UCollisionSolver_Naive()
{

}

void UCollisionSolver_Naive::UpdateParticleCollision(int32 iParticleIndex)
{
	const FVector2D& vPos = PR_ParticlesData->arrPositions[iParticleIndex];
	const FVector2D& vVel = PR_ParticlesData->arrVelocities[iParticleIndex];

	int32 iIndex = 0;
	for (const FVector2D& vOtherPos : PR_ParticlesData->arrPositions)
	{
		/*	if(iIndex != iParticleIndex
			&& IsColliding_CircleCircle(iParticleIndex, iIndex))
			{
				CheckAndHandleCollision(iParticleIndex, iIndex);
			}*/

		CheckAndHandleCollision2(iParticleIndex, iIndex);
		iIndex++;
	}

}