// Fill out your copyright notice in the Description page of Project Settings.


#include "Rendering/NiagaraRenderer.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraSystem.h"

#define NS_VARIABLE_PARTICLES_POSITIONS	"arrParticlesPositions"
#define NS_VARIABLE_PARTICLES_NUM	"iParticlesNumToSpawn"

UNiagaraRenderer::UNiagaraRenderer()
{
	PR_pNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
}

void UNiagaraRenderer::Initialise(UNiagaraSystem* pNiagaraSystem, int32 iNumberOfParticles)
{

	PR_pNiagaraComponent->SetAsset(pNiagaraSystem);

	for (int32 i = 0; i < iNumberOfParticles; i++)
	{
		PR_arrParticlesPositions.Add(FVector2D());
	}
}

void UNiagaraRenderer::Reinit(int32 iNumberOfParticles)
{
	PR_pNiagaraComponent->ResetSystem();
	PR_pNiagaraComponent->ActivateSystem(true);

	if (iNumberOfParticles < PR_arrParticlesPositions.Num())
	{
		PR_arrParticlesPositions.Empty();
	}

	for (int32 i = PR_arrParticlesPositions.Num(); i < iNumberOfParticles; i++)
	{
		PR_arrParticlesPositions.Add(FVector2D());
	}
}

void UNiagaraRenderer::UpdateParitclePositions(const TArray<FVector2D>& arrPositions)
{
	PR_pNiagaraComponent->SetIntParameter(NS_VARIABLE_PARTICLES_NUM, arrPositions.Num());
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector2D(PR_pNiagaraComponent, NS_VARIABLE_PARTICLES_POSITIONS, arrPositions);
}
