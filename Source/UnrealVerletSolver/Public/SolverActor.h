// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SolverActor.generated.h"

class UCollisionSolver;
class UNiagaraRenderer;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class ECollisionSolverType : uint8
{
	NAIVE,
	HASH_GRID,
	LIGHTGRID
};

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

	void InitialiseParticles();
	void InitialiseCollisionSolver();
	void InitialiseRenderer();

	void AddPerFrameParticles();
	void DebugDraw();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "SimBoundingBoxWidth")
	float PR_fSimBoundingBoxWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "SimBoundingBoxHeight")
	float PR_fSimBoundingBoxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "DrawPositions")
	bool PR_bDrawPositions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "ParticlesToSpawn")
	int32 PR_iParticlesToSpawn;

	UPROPERTY()
	FParticlesData PR_xParticles;

	UPROPERTY()
	UCollisionSolver* PR_pCollisionSolver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "FullPhysicsSimulation")
	bool PR_bFullPhysicsSimulation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Substeps", Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
	 int32 PR_iSubsteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "UseGravit", Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
	bool PR_bUseGravity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "BounceFromBoundary", Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
		bool PR_bBounceFromBoundary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "MinInitialParticleVelocity", Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
		float PR_fMinInitialParticleVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "MaxInitialParticleVelocity", Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
		float PR_fMaxInitialParticleVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "GravityMultiplier", Category = "PhysicsSim", meta = (EditCondition = "PR_bFullPhysicsSimulation", EditConditionHides))
		float PR_fGravityMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "ParticlesDebugDraw", Category = "DebugDraw")
		bool PR_bParticlesDebugDraw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "CollisionSolverDebugDraw", Category = "DebugDraw")
		bool PR_bCollisionSolverDebugDraw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "BurstSpawn", Category = "ParticlesSpawn")
		bool PR_bBurstSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "ParticlesToSpawnPerFrame", Category = "ParticlesSpawn", meta = (EditCondition = "!PR_bBurstSpawn", EditConditionHides))
		int32 PR_iParticlesToSpawnPerFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "RandomParticleRadius", Category = "ParticlesSpawn")
		bool PR_bRandomParticleRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "ParticleRadius", Category = "ParticlesSpawn", meta = (ClampMin = 1, ClampMax = 10, EditCondition = "!PR_bRandomParticleRadius", EditConditionHides))
		float PR_fParticleRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "CollisionSolverType", Category = "Collision")
	ECollisionSolverType PR_eCollisionSolverType;


	UPROPERTY()
		FVector2D PR_vParticlesSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UNiagaraSystem* PR_pNiagaraSystemAsset;

	UPROPERTY()
		UNiagaraRenderer* PR_pRenderer;

	void AddParticle(const FVector2D& vStartPosition, const FVector2D& vStartVelocity);
};
