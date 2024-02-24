// Fill out your copyright notice in the Description page of Project Settings.


#include "SolverActor.h"

#include "Collision/CollisionSolver_Naive.h"
#include "Collision/CollisionSolver_PointHashGrid2D.h"
#include "Rendering/NiagaraRenderer.h"

#include "NiagaraSystem.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASolverActor::ASolverActor()
:PR_fSimBoundingBoxWidth(100.0f)
, PR_fSimBoundingBoxHeight(100.0f)
, m_fParticlesRadius(4.0f)
, PR_bDrawPositions(false)
, PR_iParticlesToSpawn(1)
, PR_fRestitution(1.0f)
, PR_pCollisionSolver(nullptr)
, PR_bParticlesDebugDraw(false)
, PR_bCollisionSolverDebugDraw(false)
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
	



	//AddParticle(FVector2D(), FVector2D(100.0f, 90.0f));
	//AddParticle(FVector2D(), FVector2D(40.0f, 50.0f));
	//AddParticle(FVector2D(), FVector2D(10.0f, 70.0f));

	m_xParticles.m_fParticlesRadius = m_fParticlesRadius;

	for (int32 i = 0; i < PR_iParticlesToSpawn; i++)
	{
		AddParticle(FVector2D(FMath::RandRange(0.0f, PR_fSimBoundingBoxWidth), FMath::RandRange(0.0f, PR_fSimBoundingBoxHeight)), FVector2D(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100)));
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

	DrawDebugBox(GetWorld(), FVector(GetActorLocation().X + PR_fSimBoundingBoxWidth / 2, 0.0f, GetActorLocation().Y + PR_fSimBoundingBoxHeight / 2), FVector(PR_fSimBoundingBoxWidth / 2, 0.0f, PR_fSimBoundingBoxHeight / 2), GetActorRotation().Quaternion(), FColor::Blue, false, 0.0f, 0, 2.0f);

	UpdateSolver(DeltaTime);

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

		// Verlet integration formula
		FVector2D vTempPosition = m_xParticles.arrPositions[i];

		vCurrentPos += vVelocity * fDeltaTime + 0.5f * vAcceleration * FMath::Square(fDeltaTime);

		vPreviousPos = vTempPosition;

		vVelocity += vAcceleration * fDeltaTime;
	
		// Check and correct for boundaries
		if (vCurrentPos.X - m_fParticlesRadius < 0.0f || vCurrentPos.X + m_fParticlesRadius > PR_fSimBoundingBoxWidth)
		{
			vCurrentPos.X = FMath::Clamp(vCurrentPos.X, m_fParticlesRadius, PR_fSimBoundingBoxWidth - m_fParticlesRadius);
			vVelocity.X = -vVelocity.X / 2;//PR_fRestitution;//vCurrentPos.X - (vCurrentPos.X - vPreviousPos.X);
		}

		if (vCurrentPos.Y - m_fParticlesRadius < 0.0f || vCurrentPos.Y + m_fParticlesRadius > PR_fSimBoundingBoxHeight)
		{
			vCurrentPos.Y = FMath::Clamp(vCurrentPos.Y, m_fParticlesRadius, PR_fSimBoundingBoxHeight - m_fParticlesRadius);
			vVelocity.Y = -vVelocity.Y / 2; //PR_fRestitution;//vCurrentPos.Y - (vCurrentPos.Y - vPreviousPos.Y);
		}


		
		PR_pCollisionSolver->UpdateParticleCollision(i);

	
	}
}

void ASolverActor::OnConstruction(const FTransform& xTransform)
{
	Super::OnConstruction(xTransform);


	UKismetSystemLibrary::FlushPersistentDebugLines(this);
	/*DrawDebugBox(GetWorld(), GetActorLocation(), FVector(PR_fSimBoundingBoxWidth, PR_fSimBoundingBoxHeight, 0.0f), GetActorRotation().Quaternion(), FColor::Blue, true, 0.0f, 0, 4.0f);*/
	DrawDebugBox(GetWorld(), FVector(GetActorLocation().X + PR_fSimBoundingBoxWidth / 2, 0.0f, GetActorLocation().Y + PR_fSimBoundingBoxHeight / 2), FVector(PR_fSimBoundingBoxWidth / 2, 0.0f, PR_fSimBoundingBoxHeight / 2), GetActorRotation().Quaternion(), FColor::Blue, true, 0.0f, 0, 2.0f);
}

void ASolverActor::AddParticle(const FVector2D& vStartPosition, const FVector2D& vStartVelocity)
{
	m_xParticles.arrPositions.Add(vStartPosition);
	m_xParticles.arrAccelerations.Add(FVector2D::One());
	m_xParticles.arrPositionsPrev.Add(vStartPosition);
	m_xParticles.arrVelocities.Add(vStartVelocity);
}

