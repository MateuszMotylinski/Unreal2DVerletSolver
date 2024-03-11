// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/CollisionSolver_Naive.h"

UCollisionSolver_Naive::UCollisionSolver_Naive()
{

}

void UCollisionSolver_Naive::UpdateParticleCollision(int32 iParticleIndex)
{
	int32 iIndex = 0;
	for (const FVector2D& vOtherPos : PR_ParticlesData->arrPositions)
	{
		CheckAndHandleCollision(iParticleIndex, iIndex);
		iIndex++;
	}
}