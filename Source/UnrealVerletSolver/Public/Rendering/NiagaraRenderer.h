// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NiagaraRenderer.generated.h"

class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class UNREALVERLETSOLVER_API UNiagaraRenderer : public UObject
{
	GENERATED_BODY()

	public:
	UNiagaraRenderer();


	void Initialise(int32 iNumberOfParticles);

	void UpdateParitclePositions(const TArray<FVector2D>& arrPositions);

protected:
	UPROPERTY()
	UNiagaraComponent* PR_NiagaraComponent;

	UPROPERTY()
	TArray<FVector2D> PR_arrParticlesPositions;

};

