// Fill out your copyright notice in the Description page of Project Settings.


#include "Rendering/NiagaraRenderer.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "SolverActor.h"

#define NS_VARIABLE_PARTICLES_POSITIONS	"arrParticlesPositions"
#define NS_VARIABLE_PARTICLES_NUM	"iParticlesNumToSpawn"
#define NS_VARIABLE_PARTICLES_RADIUS	"arrParticlesRadius"

UNiagaraRenderer::UNiagaraRenderer()
{
	PR_pNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
}

void UNiagaraRenderer::Initialise(UNiagaraSystem* pNiagaraSystem, const FParticlesData& xParticlesData)
{
	PR_pNiagaraComponent->SetAsset(pNiagaraSystem);

	UpdateParticlePositions(xParticlesData.arrPositions);
	UpdateParticleRadius(xParticlesData.arrParticlesRadius);
}

void UNiagaraRenderer::Reinit(const FParticlesData& xParticlesData)
{
	PR_pNiagaraComponent->ResetSystem();
	PR_pNiagaraComponent->Deactivate();
	PR_pNiagaraComponent->DestroyInstanceNotComponent();
	PR_pNiagaraComponent->ActivateSystem(true);

	UpdateParticlePositions(xParticlesData.arrPositions);
	UpdateParticleRadius(xParticlesData.arrParticlesRadius);
}

void UNiagaraRenderer::UpdateParticlePositions(const TArray<FVector2D>& arrPositions)
{
	PR_pNiagaraComponent->SetIntParameter(NS_VARIABLE_PARTICLES_NUM, arrPositions.Num());
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector2D(PR_pNiagaraComponent, NS_VARIABLE_PARTICLES_POSITIONS, arrPositions);
}

void UNiagaraRenderer::UpdateParticleRadius(const TArray<float>& arrRadiusFloats)
{
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayFloat(PR_pNiagaraComponent, NS_VARIABLE_PARTICLES_RADIUS, arrRadiusFloats);
}
