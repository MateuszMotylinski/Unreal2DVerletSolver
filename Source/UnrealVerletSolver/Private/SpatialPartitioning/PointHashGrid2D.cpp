// Fill out your copyright notice in the Description page of Project Settings.


#include "SpatialPartitioning/PointHashGrid2D.h"


#include "Kismet/KismetSystemLibrary.h"

#include "SpatialPartitioning/PointHashGrid2D.h"

void FPointHashGrid2D::ReconstructGrid(float NewCellSize, int32 NewInvalidValue)
{
	this->Hash.Empty();
	Indexer.CellSize = NewCellSize;
	InvalidValue = NewInvalidValue;
}

void FPointHashGrid2D::InsertPoint(const int32& PointIndex, const FVector2f& Position)
{
	FVector2i idx = Indexer.ToGrid(Position);
	{
		FScopeLock Lock(&CriticalSection);
		Hash.Add(idx, PointIndex);
	}
}

void FPointHashGrid2D::InsertPointUnsafe(const int32& PointIndex, const FVector2f& Position)
{
	FVector2i idx = Indexer.ToGrid(Position);
	Hash.Add(idx, PointIndex);
}

bool FPointHashGrid2D::RemovePoint(const int32& PointIndex, const FVector2f& Position)
{
	FVector2i idx = Indexer.ToGrid(Position);
	{
		FScopeLock Lock(&CriticalSection);
		return Hash.RemoveSingle(idx, PointIndex) > 0;
	}
}

bool FPointHashGrid2D::RemovePointUnsafe(const int32& PointIndex, const FVector2f& Position)
{
	FVector2i idx = Indexer.ToGrid(Position);
	return Hash.RemoveSingle(idx, PointIndex) > 0;
}

void FPointHashGrid2D::UpdatePoint(const int32& PointIndex, const FVector2f& OldPosition, const FVector2f& NewPosition)
{
	FVector2i old_idx = Indexer.ToGrid(OldPosition);
	FVector2i new_idx = Indexer.ToGrid(NewPosition);
	if (old_idx == new_idx)
	{
		return;
	}
	bool bWasAtOldPos = false;
	{
		FScopeLock Lock(&CriticalSection);
		if (Hash.FindPair(old_idx, PointIndex))
		{
			bWasAtOldPos = true;
			Hash.RemoveSingle(old_idx, PointIndex);
		}

	}
	check(bWasAtOldPos);
	{
		FScopeLock Lock(&CriticalSection);
		Hash.Add(new_idx, PointIndex);
	}
	return;
}

void FPointHashGrid2D::UpdatePointUnsafe(const int32& PointIndex, const FVector2f& OldPosition, const FVector2f& NewPosition)
{
	FVector2i old_idx = Indexer.ToGrid(OldPosition);
	FVector2i new_idx = Indexer.ToGrid(NewPosition);
	if (old_idx == new_idx)
	{
		return;
	}
	bool bWasAtOldPos = false;

	if (Hash.FindPair(old_idx, PointIndex))
	{
		bWasAtOldPos = true;
		Hash.RemoveSingle(old_idx, PointIndex);
	}

	//check(bWasAtOldPos);
	Hash.Add(new_idx, PointIndex);
	return;
}

void FPointHashGrid2D::GetPointsInSphere(const FVector2f& SphereOrigin, float SphereRadius, TArray<int32>& ArrayOfPoints)
{
	if (!Hash.Num())
	{
		return;
	}

	FVector2i min_idx = Indexer.ToGrid(SphereOrigin - SphereRadius * FVector2f::One());
	FVector2i max_idx = Indexer.ToGrid(SphereOrigin + SphereRadius * FVector2f::One());

	TArray<int32> tPointsIds;
	for (int yi = min_idx.Y; yi <= max_idx.Y; yi++)
	{
		for (int xi = min_idx.X; xi <= max_idx.X; xi++)
		{
			FVector2i idx(xi, yi);
			Hash.MultiFind(idx, tPointsIds);

			ArrayOfPoints = tPointsIds;
		}
	}
}

void FPointHashGrid2D::GetPointsInBox(const FBox& Box, TArray<int32>& ArrayOfPoints)
{
	if (!Hash.Num())
	{
		return;
	}

	FVector2i min_idx = Indexer.ToGrid(FVector2f(Box.Min.X, Box.Min.Y));
	FVector2i max_idx = Indexer.ToGrid(FVector2f(Box.Max.X, Box.Max.Y));

	TArray<int32> Values;
	for (int yi = min_idx.Y; yi <= max_idx.Y; yi++)
	{
		for (int xi = min_idx.X; xi <= max_idx.X; xi++)
		{
			FVector2i idx(xi, yi);
			Hash.MultiFind(idx, Values);

			ArrayOfPoints = Values;
		}
	}
}

void FPointHashGrid2D::DebugDrawGrid(UObject& WorldObject)
{
	TArray<FVector2i> tPositions;

	this->Hash.GenerateKeyArray(tPositions);

	for (auto& Position : tPositions)
	{
		FVector2f tGridPosition = Indexer.FromGrid(Position);

		//FVector2i idx = Indexer.ToGrid(FVector2f(Position.X, Position.Y));

		FVector2f position = tGridPosition;
		FVector extent(Indexer.CellSize * 2, 0, Indexer.CellSize * 2);

		UKismetSystemLibrary::DrawDebugBox(&WorldObject, FVector(position.X, 0.0f, position.Y), extent, FLinearColor::Red);
	}
}