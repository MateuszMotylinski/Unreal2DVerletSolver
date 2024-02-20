// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/ScopeLock.h"
#include "Util/GridIndexing2.h"

/**
 * Hash Grid for values associated with 2D points.
 *
 * This class addresses the situation where you have a list of points with associated indices and you
 * would like to be able to do efficient proximity queries, i.e. find the nearest point, find all points within a sphere or box etc.
 * for a given query point.
 *
 * We don't store copies of the 3D points. You provide an integer index for each point.
 * Insert and Remove functions require you to pass in the point index and it's current position.
 * To Update a point you need to know its old and new 3D coordinates as well as it's index.
 */

using namespace UE;
using namespace Geometry;
using namespace UE::Math;

class FPointHashGrid
{

public:

	/**
	 * Construct 3D hash grid
	 * @param cellSize size of grid cells
	 * @param InvalidValue this value will be returned by queries if no valid result is found (e.g. bounded-distance query)
	 */
	FPointHashGrid(float cellSize, int32 InvalidValue = -1) : Indexer(cellSize), InvalidValue(InvalidValue)
	{
	}

	FPointHashGrid()
		:Indexer(200)
		, InvalidValue(-1)
	{
	}

	/**
	 * ReConstruct 3D hash grid
	 * Clears the current 3D hash great and contructs a new one with specified parameters
	 * @param cellSize size of grid cells
	 * @param InvalidValue this value will be returned by queries if no valid result is found (e.g. bounded-distance query)
	 */
	void ReconstructGrid(float NewCellSize, int32 NewInvalidValue = -1);

	/** Invalid grid value */
	int32 GetInvalidValue() const
	{
		return InvalidValue;
	}

	/**
	 * Insert a new point at given position. This function is thread-safe.
	 * @param PointIndex Index of the new the point to insert
	 * @param Position the position associated with this point
	 */
	void InsertPoint(const int32& PointIndex, const FVector2f& Position);

	/**
	 * Insert at given position, without locking / thread-safety
	 * @param PointIndex Index of the the point to insert
	 * @param Position the position associated with this point
	 */
	void InsertPointUnsafe(const int32& PointIndex, const FVector2f& Position);

	/**
	 * Remove at given position. This function is thread-safe.
	 * @param PointIndex Index of the point to remove
	 * @param Position the position associated with this point
	 * @return true if there is valid index at this position
	 */
	bool RemovePoint(const int32& PointIndex, const FVector2f& Position);

	/**
	 * Remove at given position, without locking / thread-safety
	 * @param PointIndex Index of the point to remove
	 * @param Position the position associated with this point
	 * @return true if there is valid index at this position
	 */
	bool RemovePointUnsafe(const int32& PointIndex, const FVector2f& Position);

	/**
	 * Move value from old to new position. This function is thread-safe.
	 * @param PointIndex Index of the point to update
	 * @param OldPosition the current position associated with this point
	 * @param NewPosition the new position for this point
	 */
	void UpdatePoint(const int32& PointIndex, const FVector2f& OldPosition, const FVector2f& NewPosition);

	/**
	 * Move value from old to new position, without locking / thread-safety
	 * @param PointIndex Index of the point to update
	 * @param OldPosition the current position associated with this point
	 * @param NewPosition the new position for this point
	 */
	void UpdatePointUnsafe(const int32& PointIndex, const FVector2f& OldPosition, const FVector2f& NewPosition);

	/**
	 * Get all points Ids found within a specified sphere
	 * @param SphereOrigin Origin of the sphere
	 * @param SphereRadius Radius of the Spehere
	 * @param ArrayOfPoints Array of points ids. Ids of points found within the sphere will be inserted into this array.
	 */
	void GetPointsInSphere(const FVector2f& SphereOrigin, float SphereRadius, TArray<int32>& ArrayOfPoints);

	/**
	* Get all points Ids found within a specified box
	* @param Box Box structure
	* @param ArrayOfPoints Array of points ids. Ids of points found within the box will be inserted into this array.
	*/
	void GetPointsInBox(const FBox& Box, TArray<int32>& ArrayOfPoints);

	/**
	 * Draw all Grid cells
	 */
	void DebugDrawGrid(UObject& WorldObject);

private:
	TMultiMap<FVector2i, int32> Hash;
	FCriticalSection CriticalSection;
	TScaleGridIndexer2<float> Indexer;
	int32 InvalidValue;
};