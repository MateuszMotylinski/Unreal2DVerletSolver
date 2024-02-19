// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SolverActor.generated.h"

class UCollisionSolver;

USTRUCT(Blueprintable)
struct UNREALVERLETSOLVER_API FParticlesData
{
	GENERATED_BODY()
	
	public:
	TArray<FVector2D> arrPositions;
	TArray<FVector2D> arrPositionsPrev;
	TArray<FVector2D> arrAccelerations;
	TArray<FVector2D> arrVelocities;

	float m_fParticlesRadius = 4.0f;
	float fMass = 1.0f;

};


UCLASS(Blueprintable)
class UNREALVERLETSOLVER_API ASolverActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASolverActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateSolver(float fDeltaTime);

	virtual void OnConstruction(const FTransform& xTransform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PR_fSimBoundingBoxWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PR_fSimBoundingBoxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PR_bDrawPositions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PR_iParticlesToSpawn;

	float m_fParticlesRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PR_fRestitution;

	FParticlesData m_xParticles;

	UPROPERTY()
	UCollisionSolver* PR_CollisionSolver;


	void AddParticle(const FVector2D& vStartPosition, const FVector2D& vStartVelocity);
};
