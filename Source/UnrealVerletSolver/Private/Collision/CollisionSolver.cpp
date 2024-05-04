// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/CollisionSolver.h"

bool UCollisionSolver::IsColliding_CircleCircle(int32 iParticleIndex1, int32 iParticleIndex2)
{

	FVector2D& vP1 = PR_ParticlesData->arrPositions[iParticleIndex1];
	FVector2D& vP2 = PR_ParticlesData->arrPositions[iParticleIndex2];

	float fRadius1 = PR_ParticlesData->arrParticlesRadius[iParticleIndex1];
	float fRadius2 = PR_ParticlesData->arrParticlesRadius[iParticleIndex2];

	double dDistSquared = FVector2D::DistSquared(vP1, vP2);
	double dRadiusSquared = FMath::Square(fRadius1 + fRadius2);
	return dDistSquared <= dRadiusSquared;
}

void UCollisionSolver::CheckAndHandleCollision(int32 iParticleIndex1, int32 iParticleIndex2)
{
	//SCOPE_CYCLE_COUNTER(STAT_CollisionResolution);

	//DECLARE_SCOPE_CYCLE_COUNTER(TEXT("CollisionResolution"), STAT_StatsCollisionResolution, STATGROUP_CollisionSolver);

	if (iParticleIndex1 == iParticleIndex2)
	{
		return;
	}



	FVector2D& vP1 = PR_ParticlesData->arrPositions[iParticleIndex1];
	FVector2D& vP2 = PR_ParticlesData->arrPositions[iParticleIndex2];
	float fRadius1 = PR_ParticlesData->arrParticlesRadius[iParticleIndex1];
	float fRadius2 = PR_ParticlesData->arrParticlesRadius[iParticleIndex2];
	float fMass1 = PR_ParticlesData->arrParticlesMass[iParticleIndex1];
	float fMass2 = PR_ParticlesData->arrParticlesMass[iParticleIndex2];

	double fDist = FVector2D::Distance(vP1, vP2);
	double fRadiusSum = fRadius1 + fRadius2;

	FVector2D vDir = vP2 - vP1;
	float fDinstanceSq = vDir.X * vDir.X + vDir.Y * vDir.Y;

	float fRestitution1 = 1.0f;
	float fRestitution2 = 1.0f;
	// objects in collision
	if (fRadiusSum * fRadiusSum > fDinstanceSq)
	{
		float fDistance = sqrt(fDinstanceSq);
		float fMassSum = fMass1 + fMass2;
		FVector2D vNormalDir = vDir / (fDistance * fMassSum);
		float fDirAdjust = 0.5 * fRestitution1 * fRestitution2 * (fRadiusSum - fDistance);

		// apply collision
		vP1 -= vNormalDir * fMass2 * fDirAdjust;
		vP2 += vNormalDir * fMass1 * fDirAdjust;
	}
}
