#ifndef _COLLISION_H_
#define _COLLISION_H_

#pragma once

//////////////
// Includes //
//////////////
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <stdlib.h>

////////////////////
// Local Includes //
////////////////////
#include "General.h"

class Collision
{
public:
	Collision();
	~Collision();

	int CheckSingleAgainstSingle(const DirectX::BoundingOrientedBox& box1, const DirectX::BoundingOrientedBox& box2);
	int CheckSingleAgainstSingle(const DirectX::BoundingOrientedBox& box1, const DirectX::BoundingBox& box2);
	int CheckSingleAgainstSingle(const DirectX::BoundingOrientedBox& box1, const DirectX::BoundingSphere& s);
	int CheckSingleAgainstSingle(const DirectX::BoundingSphere& box1, const DirectX::BoundingSphere& s);
	int CheckSingleAgainstSingle(const DirectX::BoundingSphere& box1, const DirectX::BoundingBox& s);
	int CheckSingleAgainstSingle(const DirectX::BoundingSphere& box1, const DirectX::BoundingOrientedBox& s);
	int CheckSingleAgainstSingle(const DirectX::BoundingBox& box1, const DirectX::BoundingSphere& s);
	int CheckSingleAgainstSingle(const DirectX::BoundingBox& box1, const DirectX::BoundingBox& s);
	int CheckSingleAgainstSingle(const DirectX::BoundingBox& box1, const DirectX::BoundingOrientedBox& s);
	int CheckSingleAgainstSingle(const DirectX::BoundingFrustum& f, const DirectX::BoundingOrientedBox& s);
	int CheckSingleAgainstSingle(const DirectX::BoundingFrustum& f, const DirectX::BoundingBox& s);
	int CheckSingleAgainstSingle(const DirectX::BoundingFrustum& f, const DirectX::BoundingSphere& s);

	void CheckSingleAgainstMultiple(const DirectX::BoundingOrientedBox& box, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);
	void CheckMultipleAgainstSingle(const DirectX::BoundingOrientedBox& box, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);

	void CheckMultipleAgainstMultiple(const DirectX::BoundingOrientedBox* arr1, const DirectX::BoundingOrientedBox* arr2, int sizeOfArray1, int sizeOfArray2, int** returnArr);
	
	int TestSingleAgainstBBT(const BBT& treeToTest, const DirectX::BoundingOrientedBox& box);
	int TestSingleAgainstBBT(const BBT& treeToTest, const DirectX::BoundingBox& box);
	int TestSingleAgainstBBT(const BBT& treeToTest, const DirectX::BoundingSphere& box);
	void TestMultipleAgainstBBT(const BBT& treeToTest, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);
	int TestBBTAgainstSingle(const BBT& treeToTest, const DirectX::BoundingOrientedBox& box);
	int TestBBTAgainstSingle(const BBT& treeToTest, const DirectX::BoundingBox& box);
	int TestBBTAgainstSingle(const BBT& treeToTest, const DirectX::BoundingSphere& s);
	void TestBBTAgainstMultiple(const BBT& treeToTest, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);
	int TestBBTAgainstBBT(const BBT& tree1, const BBT& tree2);

	void TransformBBT(BBT & out, const BBT & tree, const DirectX::XMMATRIX& mat);

	void CreateBBT(BBT& out, const DirectX::XMFLOAT3* vertices, unsigned int offset, unsigned int* indices, SubMeshInfo* submeshes, unsigned int nrOfMeshes);
	void CreateAABBT(AABBT& out, const DirectX::XMFLOAT3* vertices, unsigned int offset, unsigned int* indices, SubMeshInfo* submeshes, unsigned int nrOfMeshes);

private:
	int _FindSizeOfVertices(unsigned int* indices, int nrOfIndices, int startPoint, int* foundVertices);

};

#endif