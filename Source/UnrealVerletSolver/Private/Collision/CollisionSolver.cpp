// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/CollisionSolver.h"

bool UCollisionSolver::IsColliding_CircleCircle(int32 iParticleIndex1, int32 iParticleIndex2)
{
	FVector2D& vP1 = PR_ParticlesData->arrPositions[iParticleIndex1];
	FVector2D& vP2 = PR_ParticlesData->arrPositions[iParticleIndex2];

	double fDistSquared = FVector2D::DistSquared(vP1, vP2);
	double radiusSquared = FMath::Square(PR_ParticlesData->m_fParticlesRadius + PR_ParticlesData->m_fParticlesRadius);
	return fDistSquared <= radiusSquared;
}

void UCollisionSolver::HandleCollision(int32 iParticleIndex1, int32 iParticleIndex2)
{
	FVector2D& vP1 = PR_ParticlesData->arrPositions[iParticleIndex1];
	FVector2D& vP2 = PR_ParticlesData->arrPositions[iParticleIndex2];

	FVector2D& vVel1 = PR_ParticlesData->arrVelocities[iParticleIndex1];
	FVector2D& vVel2 = PR_ParticlesData->arrVelocities[iParticleIndex2];

	float fP1Mass = PR_ParticlesData->fMass;
	float fP2Mass = PR_ParticlesData->fMass;

	// Find the distance between the two points of collision
	double d = FMath::Sqrt(FMath::Pow(vP1.X - vP2.X, 2) + FMath::Pow(vP1.Y - vP2.Y, 2));

	// Find the norm of the vector from the point of collision for the first circleand the point of collision of the second circle
	FVector2D vNormal = (vP2 - vP1) / d;

	// Calculate the p - value that takes into account the velocities of both circles
	double p = 2 * (vVel1.X * vNormal.X + vVel1.Y * vNormal.Y - vVel2.X * vNormal.X - vVel2.Y * vNormal.Y) /
		(fP1Mass + fP2Mass);

	// Calculate the final velocity of each circle using each p - value.Note that each resultant is just the opposite sign with each variable replaced with the corresponding variable

	vVel1 = vVel1 - p * fP1Mass * vNormal;
	vVel2 = vVel2 + p * fP2Mass * vNormal;


	// If after we aplied the force the circles are still colliding, then separate the
	double fDist = FVector2D::Distance(vP1, vP2);
	double fOverlap = PR_ParticlesData->m_fParticlesRadius + PR_ParticlesData->m_fParticlesRadius - fDist;

	if (fOverlap > 0)
	{

		FVector2D vMoveVector = (0.5f * fOverlap) * vNormal;

		vP1 -= vMoveVector;
		vP2 += vMoveVector;
	}
}

void UCollisionSolver::CheckAndHandleCollision(int32 iParticleIndex1, int32 iParticleIndex2)
{
	if (iParticleIndex1 == iParticleIndex2)
	{
		return;
	}

	FVector2D& vP1 = PR_ParticlesData->arrPositions[iParticleIndex1];
	FVector2D& vP2 = PR_ParticlesData->arrPositions[iParticleIndex2];

	double fDist = FVector2D::Distance(vP1, vP2);
	double fRadiusSum = PR_ParticlesData->m_fParticlesRadius + PR_ParticlesData->m_fParticlesRadius;

	if (fDist > fRadiusSum)
	{
		return;
	}

	FVector2D& vVel1 = PR_ParticlesData->arrVelocities[iParticleIndex1];
	FVector2D& vVel2 = PR_ParticlesData->arrVelocities[iParticleIndex2];

	float fP1Mass = PR_ParticlesData->fMass;
	float fP2Mass = PR_ParticlesData->fMass;

	// Find the distance between the two points of collision
	double d = FMath::Sqrt(FMath::Pow(vP1.X - vP2.X, 2) + FMath::Pow(vP1.Y - vP2.Y, 2));

	// Find the norm of the vector from the point of collision for the first circleand the point of collision of the second circle
	FVector2D vNormal = (vP2 - vP1) / d;

	// Calculate the p - value that takes into account the velocities of both circles
	double p = 2 * (vVel1.X * vNormal.X + vVel1.Y * vNormal.Y - vVel2.X * vNormal.X - vVel2.Y * vNormal.Y) /
		(fP1Mass + fP2Mass);

	// Calculate the final velocity of each circle using each p - value.Note that each resultant is just the opposite sign with each variable replaced with the corresponding variable

	vVel1 = vVel1 - p * fP1Mass * vNormal;
	vVel2 = vVel2 + p * fP2Mass * vNormal;


	// If after we applied the force the circles are still colliding, then separate them
	double fOverlap = fRadiusSum - fDist;

	if (fOverlap > 0)
	{

		FVector2D vMoveVector = (0.5f * fOverlap) * vNormal;

		vP1 -= vMoveVector;
		vP2 += vMoveVector;
	}

}
