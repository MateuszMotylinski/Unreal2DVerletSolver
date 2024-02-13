// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SolverActor.generated.h"

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


	virtual void OnConstruction(const FTransform& xTransform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PR_fSimBoundingBoxWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PR_fSimBoundingBoxHeight;
};
