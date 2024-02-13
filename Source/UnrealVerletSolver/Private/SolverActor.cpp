// Fill out your copyright notice in the Description page of Project Settings.


#include "SolverActor.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASolverActor::ASolverActor()
:PR_fSimBoundingBoxWidth(100.0f)
, PR_fSimBoundingBoxHeight(100.0f)
{
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
	
}

// Called every frame
void ASolverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugBox(GetWorld(), GetActorLocation(), FVector(PR_fSimBoundingBoxWidth, PR_fSimBoundingBoxHeight, 0.0f), GetActorRotation().Quaternion(), FColor::Blue, false, 0.0f, 0, 4.0f);
}

void ASolverActor::OnConstruction(const FTransform& xTransform)
{
	Super::OnConstruction(xTransform);


	UKismetSystemLibrary::FlushPersistentDebugLines(this);
	DrawDebugBox(GetWorld(), GetActorLocation(), FVector(PR_fSimBoundingBoxWidth, PR_fSimBoundingBoxHeight, 0.0f), GetActorRotation().Quaternion(), FColor::Blue, true, 0.0f, 0, 4.0f);
}

