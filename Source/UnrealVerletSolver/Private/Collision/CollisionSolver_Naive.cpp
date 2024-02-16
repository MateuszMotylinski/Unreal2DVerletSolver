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
		if(iIndex != iParticleIndex
		&& IsColliding(vPos, vOtherPos, PR_ParticlesData->m_fParticlesRadius))
		{
			HandleCollision(iParticleIndex, iIndex);
		}
		iIndex++;
	}

}

bool UCollisionSolver_Naive::IsColliding(const FVector2D& vP1, const FVector2D& vP2, double fRadious)
{
	double distanceSquared = std::pow(vP1.X - vP2.X, 2) + std::pow(vP1.Y - vP2.Y, 2);
	double radiusSquared = std::pow(fRadious, 2);
	return distanceSquared <= radiusSquared;
}

void UCollisionSolver_Naive::HandleCollision(int32 iParticleIndex1, int32 iParticleIndex2)
{

	FVector2D& vP1 = PR_ParticlesData->arrPositions[iParticleIndex1];
	FVector2D& vP2 = PR_ParticlesData->arrPositions[iParticleIndex2];

	FVector2D& vVel1 = PR_ParticlesData->arrVelocities[iParticleIndex1];
	FVector2D& vVel2 = PR_ParticlesData->arrVelocities[iParticleIndex2];

	FVector2D DeltaPosition = vP2 - vP1;


	float Distance = DeltaPosition.Size();
	float RadiusSum = PR_ParticlesData->m_fParticlesRadius + PR_ParticlesData->m_fParticlesRadius;



	if (Distance < RadiusSum)
	{
		DeltaPosition.Normalize();
		FVector2D Normal = DeltaPosition;


		// Calculate overlap
		float Overlap = RadiusSum - Distance;
		// Calculate the correction vector to separate particles
		FVector2D Correction = (0.5f * Overlap) * Normal;

		// Move particles to resolve overlap
		vP1 -= Correction;
		vP2 += Correction;

		FVector2D DeltaVelocity = vVel2 - vVel1;
		float RelativeVelocity = FVector2D::DotProduct(DeltaVelocity, Normal);

		if (RelativeVelocity < 0)
		{

			float fP1Mass = PR_ParticlesData->fMass;
			float fP2Mass = PR_ParticlesData->fMass;

			float CoefficientOfRestitution = 0.8f;

			// Calculate minimum impulse required for separation
			float MinImpulse = (1 + CoefficientOfRestitution) * RelativeVelocity / (1 / PR_ParticlesData->fMass + 1 / PR_ParticlesData->fMass);

			//if (MinImpulse > 0)
			{
				FVector2D Impulse = (2.0f * RelativeVelocity / (fP1Mass + fP2Mass)) * Normal;

				vVel1 += Impulse / fP1Mass * 100;
				vVel2 += -Impulse / fP2Mass * 100;
			}

			/*FVector2D Impulse = (2.0f * RelativeVelocity / (fP1Mass + fP2Mass)) * Normal;*/

			/*vVel1 += Impulse / fP1Mass * 100;
			vVel2 += -Impulse / fP2Mass * 100;*/
		}
	}
}
