// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NiagaraRenderer.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
struct FParticlesData;
/**
 * 
 */
UCLASS()
class UNREALVERLETSOLVER_API UNiagaraRenderer : public UObject
{
	GENERATED_BODY()

	public:
	UNiagaraRenderer();


	void Initialise(UNiagaraSystem* pNiagaraSystem, const FParticlesData& xParticlesData);
	void Reinit(const FParticlesData& xParticlesData);

	void UpdateParticlePositions(const TArray<FVector2D>& arrPositions);
	void UpdateParticleRadius(const TArray<float>& arrRadiusFloats);

protected:
	UPROPERTY()
	UNiagaraComponent* PR_pNiagaraComponent;

	UPROPERTY()
	TArray<FVector2D> PR_arrParticlesPositions;

};

