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

	int CheckSingleAgainstSingle(DirectX::BoundingOrientedBox box1, DirectX::BoundingOrientedBox box2);
	void CheckSingleAgainstMultiple(DirectX::BoundingOrientedBox box, DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);
	void CheckMultipleAgainstSingle(DirectX::BoundingOrientedBox box, DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);

	void CheckMultipleAgainstMultiple(DirectX::BoundingOrientedBox* arr1, DirectX::BoundingOrientedBox* arr2, int sizeOfArray1, int sizeOfArray2, int** returnArr);

	int TestSingleAgainstBBT(BBT treeToTest, DirectX::BoundingOrientedBox box);
	void TestMultipleAgainstBBT(BBT treeToTest, DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);
	int TestBBTAgainstSingle(BBT treeToTest, DirectX::BoundingOrientedBox box);
	void TestBBTAgainstMultiple(BBT treeToTest, DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr);
	int TestBBTAgainstBBT(BBT tree1, BBT tree2);



	BBT CreateBBT(DirectX::XMFLOAT3* vertices, unsigned int offset, unsigned int* indices, SubMeshInfo* submeshes, unsigned int nrOfMeshes);




private:
	int _FindSizeOfVertices(unsigned int* indices, int nrOfIndices, int startPoint, int* foundVertices);

};

#endif