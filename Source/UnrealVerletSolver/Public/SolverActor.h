// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SolverActor.generated.h"

class UCollisionSolver;
class UNiagaraRenderer;
class UNiagaraSystem;

USTRUCT(Blueprintable)
struct UNREALVERLETSOLVER_API FParticlesData
{
	GENERATED_BODY()
	
	public:
	TArray<FVector2D> arrPositions;
	TArray<FVector2D> arrPositionsPrev;
	TArray<FVector2D> arrAccelerations;
	TArray<float> arrParticlesMass;
	TArray<float> arrParticlesRadius;
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

	UFUNCTION(BlueprintCallable)
	void Restart();

	UFUNCTION(BlueprintCallable)
	int32 GetParticlesNum() const { return PR_xParticles.arrPositions.Num(); }

	void UpdateSolver(float fDeltaTime);

	virtual void OnConstruction(const FTransform& xTransform) override;

	public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PR_fSimBoundingBoxWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PR_fSimBoundingBoxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PR_bDrawPositions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PR_iParticlesToSpawn;

	UPROPERTY()
	FParticlesData PR_xParticles;

	UPROPERTY()
	UCollisionSolver* PR_pCollisionSolver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PR_bFullPhysicsSimulation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
	 int32 PR_iSubsteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
	bool PR_bUseGravity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
		bool PR_bBounceFromBoundary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
		float PR_fMinInitialParticleVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
		float PR_fMaxInitialParticleVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
		float PR_fGravityMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugDraw")
		bool PR_bParticlesDebugDraw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugDraw")
		bool PR_bCollisionSolverDebugDraw;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParticlesSpawn")
		bool PR_bBurstSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParticlesSpawn", meta = (EditCondition = "!PR_bBurstSpawn", EditConditionHides))
		int32 PR_iParticlesToSpawnPerFrame;
	
	UPROPERTY()
		FVector2D PR_vParticlesSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UNiagaraSystem* PR_pNiagaraSystemAsset;

	UPROPERTY()
		UNiagaraRenderer* PR_pRenderer;

	void AddParticle(const FVector2D& vStartPosition, const FVector2D& vStartVelocity);
};
