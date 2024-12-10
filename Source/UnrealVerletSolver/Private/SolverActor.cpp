// Fill out your copyright notice in the Description page of Project Settings.


#include "SolverActor.h"

#include "Collision/CollisionSolver_Naive.h"
#include "Collision/CollisionSolver_PointHashGrid2D.h"
#include "Collision/CollisionSolver_LightGrid.h"

#include "Rendering/NiagaraRenderer.h"

#include "NiagaraSystem.h"
#include "Components/LineBatchComponent.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Algo/MinElement.h"

#define GRAVITY -9.8      // Gravitational acceleration in the y-direction (m/s^2)

// Sets default values
ASolverActor::ASolverActor()
:PR_fSimBoundingBoxWidth(100.0f)
, PR_fSimBoundingBoxHeight(100.0f)
, PR_bDrawPositions(false)
, PR_iParticlesToSpawn(1)
, PR_pCollisionSolver(nullptr)
, PR_bParticlesDebugDraw(false)
, PR_bCollisionSolverDebugDraw(false)
, PR_iSubsteps(1)
, PR_bFullPhysicsSimulation(true)
, PR_bUseGravity(true)
, PR_bBounceFromBoundary(false)
, PR_fMinInitialParticleVelocity(-1.0f)
, PR_fMaxInitialParticleVelocity(1.0f)
, PR_fGravityMultiplier(1.0f)
, PR_bBurstSpawn(true)
, PR_iParticlesToSpawnPerFrame(1)
, PR_bRandomParticleRadius(false)
, PR_fParticleRadius(10.0f)
, PR_eCollisionSolverType(ECollisionSolverType::HASH_GRID)
{
	PR_pRenderer = CreateDefaultSubobject<UNiagaraRenderer>("NiagaraRenderer");
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!GEngine)
	{
		return;
	}
}

// Called when the game starts or when spawned
void ASolverActor::BeginPlay()
{
	Super::BeginPlay();

	InitialiseParticles();
	InitialiseCollisionSolver();
	InitialiseRenderer();
}

void ASolverActor::InitialiseParticles()
{
	PR_xParticles = FParticlesData();
	PR_vParticlesSpawnPoint = FVector2D(PR_fSimBoundingBoxWidth / 2, PR_fSimBoundingBoxHeight / 2);

	if (PR_bBurstSpawn)
	{
		for (int32 i = 0; i < PR_iParticlesToSpawn; i++)
		{
			AddParticle(FVector2D(FMath::RandRange(0.0f, PR_fSimBoundingBoxWidth), FMath::RandRange(0.0f, PR_fSimBoundingBoxHeight)), FVector2D(FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity), FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity)));

			//AddParticle(FVector2D(FMath::RandRange(200.0f, PR_fSimBoundingBoxWidth - 200), FMath::RandRange(200.0f, PR_fSimBoundingBoxHeight - 200)), FVector2D(FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity), FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity)));
		}
	}
}

void ASolverActor::InitialiseCollisionSolver()
{
	switch (PR_eCollisionSolverType)
	{
		case ECollisionSolverType::NAIVE:
		{
			PR_pCollisionSolver = NewObject<UCollisionSolver_Naive>(this);
		}
		break;
		case ECollisionSolverType::HASH_GRID:
		{
			PR_pCollisionSolver = NewObject<UCollisionSolver_PointHashGrid2D>(this);
		}
		break;
		case ECollisionSolverType::LIGHTGRID:
		{
			PR_pCollisionSolver = NewObject<UCollisionSolver_LightGrid>(this);

			UCollisionSolver_LightGrid* pGrid = Cast<UCollisionSolver_LightGrid>(PR_pCollisionSolver);

			float* pfRadiusMin = Algo::MinElement(PR_xParticles.arrParticlesRadius);

			if (!pfRadiusMin)
			{
				ensure(0);
				return;
			}

			pGrid->m_xGrid.init(PR_fSimBoundingBoxWidth, PR_fSimBoundingBoxHeight, *pfRadiusMin);
		}
		break;
	}

	PR_pCollisionSolver->InitialiseCollisionSolver(PR_xParticles);
}

void ASolverActor::InitialiseRenderer()
{
	PR_pRenderer->Initialise(PR_pNiagaraSystemAsset, PR_xParticles);
}

void ASolverActor::AddPerFrameParticles()
{
	if (!PR_bBurstSpawn && PR_xParticles.arrPositions.Num() < PR_iParticlesToSpawn)
	{
		for (int32 i = 0; i < PR_iParticlesToSpawnPerFrame; i++)
		{
			FVector2D vRandomPosOffset = FVector2D(FMath::RandRange(-10, 10), FMath::RandRange(-10, 10));
			AddParticle(PR_vParticlesSpawnPoint + vRandomPosOffset, FVector2D(FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity), FMath::RandRange(PR_fMinInitialParticleVelocity, PR_fMaxInitialParticleVelocity)));

			PR_pCollisionSolver->InsertsParticle(PR_xParticles.arrPositions.Num() - 1, PR_vParticlesSpawnPoint);
			PR_pRenderer->Reinit(PR_xParticles);
		}
	}
}

void ASolverActor::DebugDraw()
{

	ULineBatchComponent* pLineBatcher = GetWorld()->LineBatcher;

	// Sim Bounds Debug Draw (all builds)
	if (pLineBatcher)
	{
		FVector vCenter = FVector(GetActorLocation().X + PR_fSimBoundingBoxWidth / 2, 0.0f, GetActorLocation().Y + PR_fSimBoundingBoxHeight / 2);
		FVector vExtent = FVector(PR_fSimBoundingBoxWidth / 2, 0.0f, PR_fSimBoundingBoxHeight / 2);
		FQuat qRotation = GetActorRotation().Quaternion();
		FColor vColor = FColor::Blue;
		float fLifeTime = 0.0f;
		float fThickness = 2.0f;

		TArray<FBatchedLine> arrLines;

		FTransform const Transform(qRotation);
		FVector Start = Transform.TransformPosition(FVector(vExtent.X, vExtent.Y, vExtent.Z));
		FVector End = Transform.TransformPosition(FVector(vExtent.X, -vExtent.Y, vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(vExtent.X, -vExtent.Y, vExtent.Z));
		End = Transform.TransformPosition(FVector(-vExtent.X, -vExtent.Y, vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(-vExtent.X, -vExtent.Y, vExtent.Z));
		End = Transform.TransformPosition(FVector(-vExtent.X, vExtent.Y, vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(-vExtent.X, vExtent.Y, vExtent.Z));
		End = Transform.TransformPosition(FVector(vExtent.X, vExtent.Y, vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(vExtent.X, vExtent.Y, -vExtent.Z));
		End = Transform.TransformPosition(FVector(vExtent.X, -vExtent.Y, -vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(vExtent.X, -vExtent.Y, -vExtent.Z));
		End = Transform.TransformPosition(FVector(-vExtent.X, -vExtent.Y, -vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(-vExtent.X, -vExtent.Y, -vExtent.Z));
		End = Transform.TransformPosition(FVector(-vExtent.X, vExtent.Y, -vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(-vExtent.X, vExtent.Y, -vExtent.Z));
		End = Transform.TransformPosition(FVector(vExtent.X, vExtent.Y, -vExtent.Z));
		new(arrLines)FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(vExtent.X, vExtent.Y, vExtent.Z));
		End = Transform.TransformPosition(FVector(vExtent.X, vExtent.Y, -vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(vExtent.X, -vExtent.Y, vExtent.Z));
		End = Transform.TransformPosition(FVector(vExtent.X, -vExtent.Y, -vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(-vExtent.X, -vExtent.Y, vExtent.Z));
		End = Transform.TransformPosition(FVector(-vExtent.X, -vExtent.Y, -vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		Start = Transform.TransformPosition(FVector(-vExtent.X, vExtent.Y, vExtent.Z));
		End = Transform.TransformPosition(FVector(-vExtent.X, vExtent.Y, -vExtent.Z));
		new(arrLines) FBatchedLine(vCenter + Start, vCenter + End, vColor, fLifeTime, fThickness, 0);

		pLineBatcher->DrawLines(arrLines);
	}

	// Sim Bounds Debug Draw
	//DrawDebugBox(GetWorld(), FVector(GetActorLocation().X + PR_fSimBoundingBoxWidth / 2, 0.0f, GetActorLocation().Y + PR_fSimBoundingBoxHeight / 2), FVector(PR_fSimBoundingBoxWidth / 2, 0.0f, PR_fSimBoundingBoxHeight / 2), GetActorRotation().Quaternion(), FColor::Blue, false, 0.0f, 0, 2.0f);

	// Particles Debug Draw
	if (PR_bParticlesDebugDraw)
	{
		int32 iIndex = 0;
		for (const FVector2D& vPos : PR_xParticles.arrPositions)
		{

			//DrawDebugSphere(GetWorld(),FVector(vPos.X, 0.0f, vPos.Y), m_fParticlesRadius, 4, FColor::Yellow);
			DrawDebugCircle(GetWorld(), FVector(vPos.X, 0.0f, vPos.Y), PR_xParticles.arrParticlesRadius[iIndex], 34, FColor::Yellow, false, -1.0f, 0, 0.2f
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

	// Collision Debug Draw
	if (PR_bCollisionSolverDebugDraw)
	{
		PR_pCollisionSolver->DebugDraw();
	}
}

// Called every frame
void ASolverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SCOPE_CYCLE_COUNTER(STAT_SimUpdate_Total);

	AddPerFrameParticles();

	const float fSubDT = DeltaTime / static_cast<float>(PR_iSubsteps);

	for (int32 iSubstep = 0; iSubstep < PR_iSubsteps; iSubstep++)
	{
		UpdateSolver(fSubDT);
	}

	// Update Niagara Renderer
	PR_pRenderer->UpdateParticlePositions(PR_xParticles.arrPositions);

	DebugDraw();
}

void ASolverActor::Restart()
{
	InitialiseParticles();
	InitialiseCollisionSolver();

	//PR_pRenderer = NewObject<UNiagaraRenderer>();
	PR_pRenderer->Reinit(PR_xParticles);
	PR_pRenderer->UpdateParticlePositions(PR_xParticles.arrPositions);
}

void ASolverActor::UpdateSolver(float fDeltaTime)
{
	for (int32 i = 0; i < PR_xParticles.arrPositions.Num(); i++)
	{
		FVector2D& vCurrentPos = PR_xParticles.arrPositions[i];
		FVector2D& vPreviousPos = PR_xParticles.arrPositionsPrev[i];
		FVector2D& vAcceleration = PR_xParticles.arrAccelerations[i];

		FVector2D vTempPosition = PR_xParticles.arrPositions[i];
		FVector2D vVelocity = vCurrentPos - vPreviousPos;

		if (!PR_bFullPhysicsSimulation)
		{
			vPreviousPos =  vCurrentPos - vVelocity / 1.1;
		}

		if (!PR_bFullPhysicsSimulation)
		{
			vAcceleration = FVector2D::Zero();
		}

		// Verlet integration
		vCurrentPos = 2 * vCurrentPos - vPreviousPos + vAcceleration * FMath::Square(fDeltaTime);

		// Update previous position vector
		vPreviousPos = vTempPosition;

		PR_pCollisionSolver->UpdateParticleCollision(i);


		float fParticleRadius = PR_xParticles.arrParticlesRadius[i];

		// Check and correct for boundaries
		if (vCurrentPos.X - fParticleRadius < 0.0f || vCurrentPos.X + fParticleRadius > PR_fSimBoundingBoxWidth)
		{
			vCurrentPos.X = FMath::Clamp(vCurrentPos.X, fParticleRadius, PR_fSimBoundingBoxWidth - fParticleRadius);

			if (PR_bBounceFromBoundary)
			{
				vCurrentPos.X -= vVelocity.X / 2;
			}
		}

		if (vCurrentPos.Y - fParticleRadius < 0.0f || vCurrentPos.Y + fParticleRadius > PR_fSimBoundingBoxHeight)
		{
			vCurrentPos.Y = FMath::Clamp(vCurrentPos.Y, fParticleRadius, PR_fSimBoundingBoxHeight - fParticleRadius);

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
	PR_xParticles.arrPositions.Add(vStartPosition);

	if (PR_bUseGravity)
	{
		PR_xParticles.arrAccelerations.Add(FVector2D(0.0f, GRAVITY * PR_fGravityMultiplier));
	}
	else
	{
		PR_xParticles.arrAccelerations.Add(vStartVelocity);
	}
	PR_xParticles.arrPositionsPrev.Add(vStartPosition);
	PR_xParticles.arrParticlesMass.Add(1.0f);

	if (PR_bRandomParticleRadius)
	{
		PR_xParticles.arrParticlesRadius.Add(FMath::RandRange(1.0f, 10.0f));
	}
	else
	{
		PR_xParticles.arrParticlesRadius.Add(PR_fParticleRadius);
	}
}

