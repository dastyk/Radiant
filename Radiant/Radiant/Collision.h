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
	void CheckSingleAgainstMultiple(const DirectX::BoundingOrientedBox& box, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);
	void CheckMultipleAgainstSingle(const DirectX::BoundingOrientedBox& box, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);

	void CheckMultipleAgainstMultiple(const DirectX::BoundingOrientedBox* arr1, const DirectX::BoundingOrientedBox* arr2, int sizeOfArray1, int sizeOfArray2, int** returnArr);
	
	int TestSingleAgainstBBT(const BBT& treeToTest, const DirectX::BoundingOrientedBox& box);
	void TestMultipleAgainstBBT(const BBT& treeToTest, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);
	int TestBBTAgainstSingle(const BBT& treeToTest, const DirectX::BoundingOrientedBox& box);
	void TestBBTAgainstMultiple(const BBT& treeToTest, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);
	int TestBBTAgainstBBT(const BBT& tree1, const BBT& tree2);

	BBT TransformBBT(BBT& tree, const DirectX::XMMATRIX& mat);

	BBT CreateBBT(const DirectX::XMFLOAT3* vertices, unsigned int offset, unsigned int* indices, SubMeshInfo* submeshes, unsigned int nrOfMeshes);
	BBT CreateBBT(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2);




private:
	int _FindSizeOfVertices(unsigned int* indices, int nrOfIndices, int startPoint, int* foundVertices);

};

#endif