// Fill out your copyright notice in the Description page of Project Settings.


#include "SolverActor.h"

#include "Collision/CollisionSolver_Naive.h"
#include "Collision/CollisionSolver_PointHashGrid2D.h"
#include "Rendering/NiagaraRenderer.h"

#include "NiagaraSystem.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

#define GRAVITY -9.8      // Gravitational acceleration in the y-direction (m/s^2)

// Sets default values
ASolverActor::ASolverActor()
:PR_fSimBoundingBoxWidth(100.0f)
, PR_fSimBoundingBoxHeight(100.0f)
, m_fParticlesRadius(4.0f)
, PR_bDrawPositions(false)
, PR_iParticlesToSpawn(1)
, PR_pCollisionSolver(nullptr)
, PR_bParticlesDebugDraw(false)
, PR_bCollisionSolverDebugDraw(false)
, PR_iSubsteps(1)
, PR_bUseGravity(true)
, PR_bBounceFromBoundary(false)
{
	PR_pRenderer = CreateDefaultSubobject<UNiagaraRenderer>("NiagaraRenderer");
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.TickInterval = 0.1f;

	if (!GEngine)
	{
		return;
	}
}

// Called when the game starts or when spawned
void ASolverActor::BeginPlay()
{
	Super::BeginPlay();

	m_xParticles.m_fParticlesRadius = m_fParticlesRadius;

	PR_vParticlesSpawnPoint = FVector2D(PR_fSimBoundingBoxWidth / 2, PR_fSimBoundingBoxHeight / 2);

	if (PR_bBurstSpawn)
	{
		for (int32 i = 0; i < PR_iParticlesToSpawn; i++)
		{
			AddParticle(FVector2D(FMath::RandRange(0.0f, PR_fSimBoundingBoxWidth), FMath::RandRange(0.0f, PR_fSimBoundingBoxHeight)), FVector2D(FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity), FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity)));
		}
	}

	//PR_CollisionSolver = NewObject<UCollisionSolver_Naive>(this);
	PR_pCollisionSolver = NewObject<UCollisionSolver_PointHashGrid2D>(this);

	PR_pCollisionSolver->InitialiseCollisionSolver(m_xParticles);

	PR_pRenderer->Initialise(PR_pNiagaraSystemAsset, PR_iParticlesToSpawn);
	PR_pRenderer->UpdateParitclePositions(m_xParticles.arrPositions);
}

// Called every frame
void ASolverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PR_bBurstSpawn && m_xParticles.arrPositions.Num() < PR_iParticlesToSpawn)
	{
		FVector2D vRandomPosOffset = FVector2D(FMath::RandRange(-10, 10), FMath::RandRange(-10, 10));
		AddParticle(PR_vParticlesSpawnPoint + vRandomPosOffset, FVector2D(FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity), FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity)));

		PR_pCollisionSolver->InsertsParticle(m_xParticles.arrPositions.Num() - 1, PR_vParticlesSpawnPoint);
		PR_pRenderer->Reinit(m_xParticles.arrPositions.Num());
	}

	DrawDebugBox(GetWorld(), FVector(GetActorLocation().X + PR_fSimBoundingBoxWidth / 2, 0.0f, GetActorLocation().Y + PR_fSimBoundingBoxHeight / 2), FVector(PR_fSimBoundingBoxWidth / 2, 0.0f, PR_fSimBoundingBoxHeight / 2), GetActorRotation().Quaternion(), FColor::Blue, false, 0.0f, 0, 2.0f);

	//UpdateSolver(0.001f);
	
	const float fSubDT = DeltaTime / static_cast<float>(PR_iSubsteps);

	for (int32 iSubstep = 0; iSubstep < PR_iSubsteps; iSubstep++)
	{
		UpdateSolver(fSubDT);
	}

	if (PR_bParticlesDebugDraw)
	{
		int32 iIndex = 0;
		for (const FVector2D& vPos : m_xParticles.arrPositions)
		{

			//DrawDebugSphere(GetWorld(),FVector(vPos.X, 0.0f, vPos.Y), m_fParticlesRadius, 4, FColor::Yellow);
			DrawDebugCircle(GetWorld(), FVector(vPos.X, 0.0f, vPos.Y), m_fParticlesRadius, 34, FColor::Yellow, false, -1.0f, 0, 0.2f
				, FVector(0.0f, 0.0f, 1.0f)
				, FVector(1.0f, 0.0f, 0.0f)
				, false);

			if (PR_bDrawPositions)
			{
				UKismetSystemLibrary::DrawDebugString(GetWorld(), FVector(vPos.X, 0.0f, vPos.Y), vPos.ToString());
			}



			iIndex++;
		}
	}

	if (PR_bCollisionSolverDebugDraw)
	{
		PR_pCollisionSolver->DebugDraw();
	}

	PR_pRenderer->UpdateParitclePositions(m_xParticles.arrPositions);
}

void ASolverActor::UpdateSolver(float fDeltaTime)
{   
	for (int32 i = 0; i < m_xParticles.arrPositions.Num(); i++)
	{
		FVector2D& vCurrentPos = m_xParticles.arrPositions[i];
		FVector2D& vPreviousPos = m_xParticles.arrPositionsPrev[i];
		FVector2D& vAcceleration = m_xParticles.arrAccelerations[i];
		FVector2D& vVelocity = m_xParticles.arrVelocities[i];

		FVector2D vTempPosition = m_xParticles.arrPositions[i];

		if (!PR_bFullPhysicsSimulation)
		{
			vVelocity = vCurrentPos - vPreviousPos;
			vPreviousPos =  vCurrentPos - vVelocity / 1.1;
			//vPreviousPos += vVelocity / 2;
		}

		if (!PR_bFullPhysicsSimulation)
		{
			vAcceleration = FVector2D::Zero();
		}

		//vPreviousPos = (PR_bForceNullAccelerationAndVelocity) ? vPreviousPos : vCurrentPos;
		//vAcceleration = (!PR_bFullPhysicsSimulation) ? vAcceleration : FVector2D::Zero();

		// Verlet integration
		vCurrentPos = 2 * vCurrentPos - vPreviousPos + vAcceleration * FMath::Square(fDeltaTime);

		// Set velocity (CurrentPos - PreviousPos)
		vVelocity = vCurrentPos - vPreviousPos;

		// Update previous position vector
		vPreviousPos = vTempPosition;

		PR_pCollisionSolver->UpdateParticleCollision(i);


		// Check and correct for boundaries
		if (vCurrentPos.X - m_fParticlesRadius < 0.0f || vCurrentPos.X + m_fParticlesRadius > PR_fSimBoundingBoxWidth)
		{
			vCurrentPos.X = FMath::Clamp(vCurrentPos.X, m_fParticlesRadius, PR_fSimBoundingBoxWidth - m_fParticlesRadius);
			
			if (PR_bBounceFromBoundary)
			{
				vCurrentPos.X -= vVelocity.X / 2;
			}
		}

		if (vCurrentPos.Y - m_fParticlesRadius < 0.0f || vCurrentPos.Y + m_fParticlesRadius > PR_fSimBoundingBoxHeight)
		{
			vCurrentPos.Y = FMath::Clamp(vCurrentPos.Y, m_fParticlesRadius, PR_fSimBoundingBoxHeight - m_fParticlesRadius);

			if (PR_bBounceFromBoundary)
			{
				vCurrentPos.Y -= vVelocity.Y / 2;
			}
		}
	}
}

void ASolverActor::OnConstruction(const FTransform& xTransform)
{
	Super::OnConstruction(xTransform);

	UKismetSystemLibrary::FlushPersistentDebugLines(this);
	DrawDebugBox(GetWorld(), FVector(GetActorLocation().X + PR_fSimBoundingBoxWidth / 2, 0.0f, GetActorLocation().Y + PR_fSimBoundingBoxHeight / 2), FVector(PR_fSimBoundingBoxWidth / 2, 0.0f, PR_fSimBoundingBoxHeight / 2), GetActorRotation().Quaternion(), FColor::Blue, true, 0.0f, 0, 2.0f);
}

void ASolverActor::AddParticle(const FVector2D& vStartPosition, const FVector2D& vStartVelocity)
{
	m_xParticles.arrPositions.Add(vStartPosition);

	if (PR_bUseGravity)
	{
		m_xParticles.arrAccelerations.Add(FVector2D(0.0f, GRAVITY * PR_fGravityMultiplier));
	}
	else
	{
		m_xParticles.arrAccelerations.Add(vStartVelocity);
		//m_xParticles.arrAccelerations.Add(FVector2D::Zero());
	}
	
	m_xParticles.arrPositionsPrev.Add(vStartPosition);
	m_xParticles.arrVelocities.Add(vStartVelocity);
}

