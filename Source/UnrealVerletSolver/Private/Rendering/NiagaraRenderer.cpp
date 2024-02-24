// Fill out your copyright notice in the Description page of Project Settings.


#include "Rendering/NiagaraRenderer.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

#define NS_VARIABLE_PARTICLES_POSITIONS	"arrParticlesPositions"
#define NS_VARIABLE_PARTICLES_NUM	"iParticlesNumToSpawn"

UNiagaraRenderer::UNiagaraRenderer()
{
	PR_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
}

void UNiagaraRenderer::Initialise(int32 iNumberOfParticles)
{
	for (int32 i = 0; i < iNumberOfParticles; i++)
	{
		PR_arrParticlesPositions.Add(FVector2D());
	}
}

void UNiagaraRenderer::UpdateParitclePositions(const TArray<FVector2D>& arrPositions)
{
	PR_NiagaraComponent->SetIntParameter(NS_VARIABLE_PARTICLES_NUM, arrPositions.Num());
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector2D(PR_NiagaraComponent, NS_VARIABLE_PARTICLES_POSITIONS, arrPositions);
}
