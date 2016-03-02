#include "Collision.h"

Collision::Collision()
{
	
}

Collision::~Collision()
{
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingOrientedBox& box1, const DirectX::BoundingOrientedBox& box2)
{
	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&box2.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMLoadFloat3(&box2.Extents);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&box1.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return 0; // Too far away
	}


	return box1.Contains(box2);
			
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingOrientedBox & box1, const DirectX::BoundingBox & box2)
{
	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&box2.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMLoadFloat3(&box2.Extents);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&box1.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return 0; // Too far away
	}


	return box1.Contains(box2);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingOrientedBox & box1, const DirectX::BoundingSphere & s)
{
	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&s.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMVectorReplicatePtr(&s.Radius);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&box1.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return 0; // Too far away
	}


	return box1.Contains(s);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingSphere & s1, const DirectX::BoundingSphere & s2)
{
	return s1.Contains(s2);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingSphere & box1, const DirectX::BoundingBox & s)
{
	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMVectorReplicatePtr(&box1.Radius);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&s.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&s.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return 0; // Too far away
	}


	return box1.Contains(s);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingSphere & box1, const DirectX::BoundingOrientedBox & s)
{
	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMVectorReplicatePtr(&box1.Radius);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&s.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&s.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return 0; // Too far away
	}


	return box1.Contains(s);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingBox & box1, const DirectX::BoundingSphere & s)
{
	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&s.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMVectorReplicatePtr(&s.Radius);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&box1.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return 0; // Too far away
	}


	return box1.Contains(s);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingBox & box1, const DirectX::BoundingBox & s)
{
	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMLoadFloat3(&box1.Extents);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&box1.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return 0; // Too far away
	}


	return box1.Contains(s);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingBox & box1, const DirectX::BoundingOrientedBox & s)
{
	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&s.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMLoadFloat3(&s.Extents);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&box1.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return 0; // Too far away
	}


	return box1.Contains(s);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingFrustum & f, const DirectX::BoundingOrientedBox & s)
{
	return f.Contains(s);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingFrustum & f, const DirectX::BoundingBox & s)
{
	return f.Contains(s);
}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingFrustum & f, const DirectX::BoundingSphere & s)
{
	return f.Contains(s);
}

void Collision::CheckSingleAgainstMultiple(const DirectX::BoundingOrientedBox& box, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr)
{
	if (!arr)
	{
		throw ErrorMsg(7000001, L"arr not defined in function CheckSingleAgainstMultiple");
	}
	if (!returnArr)
	{
		throw ErrorMsg(7000002, L"returnrArr not defined in function CheckSingleAgainstMultiple");
	}

	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(DirectX::XMVectorSet(box.Center.x - box.Extents.x, box.Center.y - box.Extents.y, box.Center.z - box.Extents.z, 0));
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	float radius2 = 0;

	DirectX::XMVECTOR distanceVec;
	float distance;

	for (int i = 0; i < sizeOfArray; i++)
	{
		distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSet(box.Center.x - arr[i].Center.x, box.Center.y - arr[i].Center.y, box.Center.z - arr[i].Center.z, 0));
		distance = DirectX::XMVectorGetByIndex(distanceVec, 0);
		radius2 = DirectX::XMVectorGetByIndex(DirectX::XMVector3Length(DirectX::XMVectorSet(arr[i].Center.x - arr[i].Extents.x, arr[i].Center.y - arr[i].Extents.y, arr[i].Center.z - arr[i].Extents.z, 0)), 0);

		if (distance <= radius + radius2)
		{
			returnArr[i] = box.Contains(arr[i]);
		}
		else
		{
			returnArr[i] = 0;
		}
	}
}

void Collision::CheckMultipleAgainstSingle(const DirectX::BoundingOrientedBox& box, const DirectX::BoundingOrientedBox * arr, int sizeOfArray, int * returnArr)
{
	if (!arr)
	{
		throw ErrorMsg(7000003, L"arr not defined in function CheckMultipleAgainstSingle");
	}
	if (!returnArr)
	{
		throw ErrorMsg(7000004, L"returnrArr not defined in function CheckMultipleAgainstSingle");
	}

	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(DirectX::XMVectorSet(box.Center.x - box.Extents.x, box.Center.y - box.Extents.y, box.Center.z - box.Extents.z, 0));
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	float radius2 = 0;

	DirectX::XMVECTOR distanceVec;
	float distance;

	for (int i = 0; i < sizeOfArray; i++)
	{
		distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSet(box.Center.x - arr[i].Center.x, box.Center.y - arr[i].Center.y, box.Center.z - arr[i].Center.z, 0));
		distance = DirectX::XMVectorGetByIndex(distanceVec, 0);
		radius2 = DirectX::XMVectorGetByIndex(DirectX::XMVector3Length(DirectX::XMVectorSet(arr[i].Center.x - arr[i].Extents.x, arr[i].Center.y - arr[i].Extents.y, arr[i].Center.z - arr[i].Extents.z, 0)), 0);

		if (distance <= radius + radius2)
		{
			returnArr[i] = arr[i].Contains(box);
		}
		else
		{
			returnArr[i] = 0;
		}
	}
}

void Collision::CheckMultipleAgainstMultiple(const DirectX::BoundingOrientedBox * arr1, const DirectX::BoundingOrientedBox * arr2, int sizeOfArray1, int sizeOfArray2, int ** returnArr)
{
	if (!arr1)
	{
		throw ErrorMsg(7000005, L"arr1 not defined in function CheckMultipleAgainstMultiple");
	}
	if (!arr2)
	{
		throw ErrorMsg(7000006, L"arr2 not defined in function CheckMultipleAgainstMultiple");
	}
	if (!returnArr)
	{
		throw ErrorMsg(7000007, L"returnrArr not defined in function CheckMultipleAgainstMultiple");
	}

	for (int i = 0; i < sizeOfArray2; i++)
	{
		CheckMultipleAgainstSingle(arr2[i], arr1, sizeOfArray1, returnArr[i]);
	}
}

int Collision::TestSingleAgainstBBT(const BBT& treeToTest, const DirectX::BoundingOrientedBox& box)
{
	int checkInt = CheckSingleAgainstSingle(box, treeToTest.root);
	if (checkInt != 0)
	{
		if (treeToTest.nrOfChildren != 0)
		{
			if (!treeToTest.children)
			{
				throw ErrorMsg(7000008, L"BBT children not defined in function TestSingleAgainstBBT");
			}

			for (unsigned int i = 0; i < treeToTest.nrOfChildren; i++)
			{
				checkInt = CheckSingleAgainstSingle(box, treeToTest.children[i]);
				if (checkInt != 0)
				{
					return checkInt;
				}
			}
		}

	}
	return checkInt;
}

int Collision::TestSingleAgainstBBT(const BBT & treeToTest, const DirectX::BoundingBox & box)
{
	int checkInt = CheckSingleAgainstSingle(box, treeToTest.root);
	if (checkInt != 0)
	{
		if (treeToTest.nrOfChildren != 0)
		{
			if (!treeToTest.children)
			{
				throw ErrorMsg(7000008, L"BBT children not defined in function TestSingleAgainstBBT");
			}

			for (unsigned int i = 0; i < treeToTest.nrOfChildren; i++)
			{
				checkInt = CheckSingleAgainstSingle(box, treeToTest.children[i]);
				if (checkInt != 0)
				{
					return checkInt;
				}
			}
		}

	}
	return checkInt;
}

int Collision::TestSingleAgainstBBT(const BBT & treeToTest, const DirectX::BoundingSphere & box)
{
	int checkInt = CheckSingleAgainstSingle(box, treeToTest.root);
	if (checkInt != 0)
	{
		if (treeToTest.nrOfChildren != 0)
		{
			if (!treeToTest.children)
			{
				throw ErrorMsg(7000008, L"BBT children not defined in function TestSingleAgainstBBT");
			}

			for (unsigned int i = 0; i < treeToTest.nrOfChildren; i++)
			{
				checkInt = CheckSingleAgainstSingle(box, treeToTest.children[i]);
				if (checkInt != 0)
				{
					return checkInt;
				}
			}
		}

	}
	return checkInt;
}

void Collision::TestMultipleAgainstBBT(const BBT& treeToTest, const DirectX::BoundingOrientedBox * arr, int sizeOfArray, int * returnArr)
{
	if (!arr)
	{
		throw ErrorMsg(7000009, L"arr not defined in function TestMultipleAgainstBBT");
	}
	if (!returnArr)
	{
		throw ErrorMsg(7000010, L"returnArr not defined in function TestMultipleAgainstBBT");
	}

	for (int i = 0; i < sizeOfArray; i++)
	{
		returnArr[i] = TestSingleAgainstBBT(treeToTest, arr[i]);
	}
}

int Collision::TestBBTAgainstSingle(const BBT& treeToTest, const DirectX::BoundingOrientedBox& box)
{
	int checkInt = CheckSingleAgainstSingle(treeToTest.root, box);
	if (checkInt != 0)
	{
		if (treeToTest.nrOfChildren != 0)
		{
			if (!treeToTest.children)
			{
				throw ErrorMsg(7000011, L"BBT children not defined in function TestBBTAgainstSingle");
			}

			for (unsigned int i = 0; i < treeToTest.nrOfChildren; i++)
			{
				checkInt = CheckSingleAgainstSingle(treeToTest.children[i], box);
				if (checkInt != 0)
				{
					return checkInt;
				}
			}
		}

	}
	return checkInt;
}

int Collision::TestBBTAgainstSingle(const BBT & treeToTest, const DirectX::BoundingBox & box)
{
	int checkInt = CheckSingleAgainstSingle(treeToTest.root, box);
	if (checkInt != 0)
	{
		if (treeToTest.nrOfChildren != 0)
		{
			if (!treeToTest.children)
			{
				throw ErrorMsg(7000011, L"BBT children not defined in function TestBBTAgainstSingle");
			}

			for (unsigned int i = 0; i < treeToTest.nrOfChildren; i++)
			{
				checkInt = CheckSingleAgainstSingle(treeToTest.children[i], box);
				if (checkInt != 0)
				{
					return checkInt;
				}
			}
		}

	}
	return checkInt;
}

int Collision::TestBBTAgainstSingle(const BBT & treeToTest, const DirectX::BoundingSphere & s)
{
	int checkInt = CheckSingleAgainstSingle(treeToTest.root, s);
	if (checkInt != 0)
	{
		if (treeToTest.nrOfChildren != 0)
		{
			if (!treeToTest.children)
			{
				throw ErrorMsg(7000011, L"BBT children not defined in function TestBBTAgainstSingle");
			}

			for (unsigned int i = 0; i < treeToTest.nrOfChildren; i++)
			{
				checkInt = CheckSingleAgainstSingle(treeToTest.children[i], s);
				if (checkInt != 0)
				{
					return checkInt;
				}
			}
		}

	}
	return checkInt;
}

void Collision::TestBBTAgainstMultiple(const BBT& treeToTest, const DirectX::BoundingOrientedBox* arr, int sizeOfArray, int* returnArr)
{
	if (!arr)
	{
		throw ErrorMsg(7000012, L"arr not defined in function TestBBTAgainstMultiple");
	}
	if (!returnArr)
	{
		throw ErrorMsg(7000013, L"returnArr not defined in function TestBBTAgainstMultiple");
	}

	for (int i = 0; i < sizeOfArray; i++)
	{
		returnArr[i] = TestBBTAgainstSingle(treeToTest, arr[i]);
	}
}

int Collision::TestBBTAgainstBBT(const BBT& tree1, const BBT& tree2)
{
	if (CheckSingleAgainstSingle(tree1.root, tree2.root))
	{
		if (tree1.nrOfChildren == 0)
		{
			return TestBBTAgainstSingle(tree2, tree1.root);
		}

		int* testArray;

		try { testArray = new int[tree1.nrOfChildren]; }
		catch (std::exception& e)
		{
			e;
			throw ErrorMsg(7000014, L"failed to allocate memory in function TestBBTAgainstBBT");
		}

		TestBBTAgainstMultiple(tree2, tree1.children, tree1.nrOfChildren, testArray);
		for (unsigned int i = 0; i < tree1.nrOfChildren; i++)
		{
			if (testArray[i] != 0)
			{
				int returnValue = testArray[i];
				delete testArray;
				return returnValue;
			}
		}
	}

	return 0;
}

int Collision::TestAABBTAgainstSingle(const AABBT & treeToTest, const DirectX::BoundingSphere & s, DirectX::XMVECTOR& outMTV)
{
	outMTV = DirectX::XMVectorZero();
	int checkInt = CheckSingleAgainstSingle(treeToTest.root, s);
	if (checkInt != 0)
	{
		if (treeToTest.nrOfChildren != 0)
		{
			if (!treeToTest.children)
			{
				throw ErrorMsg(7000011, L"BBT children not defined in function TestBBTAgainstSingle");
			}
			DirectX::XMVECTOR vector;
			for (unsigned int i = 0; i < treeToTest.nrOfChildren; i++)
			{
				
				int temp = CheckSingleAgainstSingle(treeToTest.children[i], s, vector);
				if (temp != 0)
				{
					outMTV = DirectX::XMVectorAdd(outMTV, vector);
					checkInt = temp;
				}
			}
		}

	}
	return checkInt;
}

int Collision::TestSingleAgainstAABBT(const AABBT & treeToTest, const DirectX::BoundingFrustum & f)
{
	return 0;
}

void Collision::TransformBBT(BBT & out, const BBT & tree, const DirectX::XMMATRIX & mat)
{
	out.nrOfChildren = tree.nrOfChildren;

	SAFE_DELETE_ARRAY(out.children);

	tree.root.Transform(out.root, mat);
	if (out.nrOfChildren > 0)
	{
		out.children = new DirectX::BoundingOrientedBox[out.nrOfChildren];
		for (uint i = 0; i < out.nrOfChildren; i++)
		{
			tree.children[i].Transform(out.children[i], mat);
		}

	}

}

void Collision::TransformAABBT(AABBT & out, const AABBT & tree, const DirectX::XMMATRIX & mat)
{
	out.nrOfChildren = tree.nrOfChildren;

	SAFE_DELETE_ARRAY(out.children);

	tree.root.Transform(out.root, mat);
	if (out.nrOfChildren > 0)
	{
		out.children = new DirectX::BoundingBox[out.nrOfChildren];
		for (uint i = 0; i < out.nrOfChildren; i++)
		{
			tree.children[i].Transform(out.children[i], mat);
		}

	}

}

void Collision::CreateBBT(BBT& out, const DirectX::XMFLOAT3 * vertices, unsigned int offset, unsigned int* indices, SubMeshInfo * submeshes, unsigned int nrOfMeshes)
{
	if (!vertices)
	{
		throw ErrorMsg(7000015, L"vertices not defined in function CreateBBT");
	}
	if (!indices)
	{
		throw ErrorMsg(7000016, L"indices not defined in function CreateBBT");
	}
	if (!submeshes)
	{
		throw ErrorMsg(7000017, L"submeshes not defined in function CreateBBT");
	}

	int* foundVertices;

	try { foundVertices = new int[submeshes[nrOfMeshes - 1].count + submeshes[nrOfMeshes - 1].indexStart]; }
	catch (std::exception& e)
	{
		e;
		throw ErrorMsg(7000018, L"failed to allocate memory for foundVertices in function TestBBTAgainstBBT");
	}

	int sizeOfVertices = _FindSizeOfVertices(indices, submeshes[nrOfMeshes - 1].count + submeshes[nrOfMeshes - 1].indexStart, 0, foundVertices);

	delete foundVertices;
	

	out.root.CreateFromPoints(out.root, sizeOfVertices, vertices, offset);

	if (nrOfMeshes > 1)
	{
		out.children = new DirectX::BoundingOrientedBox[nrOfMeshes];
		out.nrOfChildren = nrOfMeshes;
	}
	else
	{
		out.nrOfChildren = 0;
		out.children = nullptr;
		return;
	}

	for (unsigned int i = 0; i < out.nrOfChildren; i++)
	{
		try { foundVertices = new int[submeshes[i].count]; }
		catch (std::exception& e)
		{
			e;
			throw ErrorMsg(7000018, L"failed to allocate memory for foundVertices in function TestBBTAgainstBBT");
		}

		sizeOfVertices = _FindSizeOfVertices(indices, submeshes[i].count, submeshes[i].indexStart, foundVertices);
		DirectX::XMFLOAT3* temp;

		try { temp = new DirectX::XMFLOAT3[submeshes[i].count]; }
		catch (std::exception& e)
		{
			e;
			throw ErrorMsg(7000019, L"failed to allocate memory for temp in function TestBBTAgainstBBT");
		}

		for (int j = 0; j < sizeOfVertices; j++)
		{
			temp[j] = vertices[foundVertices[j]];
		}


		out.children[i].CreateFromPoints(out.children[i], sizeOfVertices, temp, offset);
		delete temp;
		delete foundVertices;
	}

}

void Collision::CreateAABBT(AABBT & out, const DirectX::XMFLOAT3 * vertices, unsigned int offset, unsigned int * indices, SubMeshInfo * submeshes, unsigned int nrOfMeshes)
{
	if (!vertices)
	{
		throw ErrorMsg(7000015, L"vertices not defined in function CreateBBT");
	}
	if (!indices)
	{
		throw ErrorMsg(7000016, L"indices not defined in function CreateBBT");
	}
	if (!submeshes)
	{
		throw ErrorMsg(7000017, L"submeshes not defined in function CreateBBT");
	}

	int* foundVertices;

	try { foundVertices = new int[submeshes[nrOfMeshes - 1].count + submeshes[nrOfMeshes - 1].indexStart]; }
	catch (std::exception& e)
	{
		e;
		throw ErrorMsg(7000018, L"failed to allocate memory for foundVertices in function TestBBTAgainstBBT");
	}

	int sizeOfVertices = _FindSizeOfVertices(indices, submeshes[nrOfMeshes - 1].count + submeshes[nrOfMeshes - 1].indexStart, 0, foundVertices);

	delete foundVertices;


	out.root.CreateFromPoints(out.root, sizeOfVertices, vertices, offset);

	if (nrOfMeshes > 1)
	{
		out.children = new DirectX::BoundingBox[nrOfMeshes];
		out.nrOfChildren = nrOfMeshes;
	}
	else
	{
		out.nrOfChildren = 0;
		out.children = nullptr;
		return;
	}

	for (unsigned int i = 0; i < out.nrOfChildren; i++)
	{
		try { foundVertices = new int[submeshes[i].count]; }
		catch (std::exception& e)
		{
			e;
			throw ErrorMsg(7000018, L"failed to allocate memory for foundVertices in function TestBBTAgainstBBT");
		}

		sizeOfVertices = _FindSizeOfVertices(indices, submeshes[i].count, submeshes[i].indexStart, foundVertices);
		DirectX::XMFLOAT3* temp;

		try { temp = new DirectX::XMFLOAT3[submeshes[i].count]; }
		catch (std::exception& e)
		{
			e;
			throw ErrorMsg(7000019, L"failed to allocate memory for temp in function TestBBTAgainstBBT");
		}

		for (int j = 0; j < sizeOfVertices; j++)
		{
			temp[j] = vertices[foundVertices[j]];
		}


		out.children[i].CreateFromPoints(out.children[i], sizeOfVertices, temp, offset);
		delete temp;
		delete foundVertices;
	}

}

int Collision::CheckSingleAgainstSingle(const DirectX::BoundingBox & box1, const DirectX::BoundingSphere & s, DirectX::XMVECTOR & outMTV)
{
	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&s.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMVectorReplicatePtr(&s.Radius);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&box1.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&box1.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return 0; // Too far away
	}



	//DirectX::XMVECTOR BoxMin = DirectX::XMVectorSubtract(BoxCenter, BoxExtents);
	//DirectX::XMVECTOR BoxMax = DirectX::XMVectorAdd(BoxCenter, BoxExtents);

	//// Find the distance to the nearest point on the box.
	//// for each i in (x, y, z)
	//// if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
	//// else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2

	//DirectX::XMVECTOR d = DirectX::XMVectorZero();

	//// Compute d for each dimension.
	//DirectX::XMVECTOR LessThanMin = DirectX::XMVectorLess(SphereCenter, BoxMin);
	//DirectX::XMVECTOR GreaterThanMax = DirectX::XMVectorGreater(SphereCenter, BoxMax);

	//DirectX::XMVECTOR MinDelta = DirectX::XMVectorSubtract(SphereCenter, BoxMin);
	//DirectX::XMVECTOR MaxDelta = DirectX::XMVectorSubtract(SphereCenter, BoxMax);

	//// Choose value for each dimension based on the comparison.
	//d = DirectX::XMVectorSelect(d, MinDelta, LessThanMin);
	//d = DirectX::XMVectorSelect(d, MaxDelta, GreaterThanMax);

	//// Use a dot-product to square them and sum them together.
	//DirectX::XMVECTOR d2 = DirectX::XMVector3Dot(d, d);

	//outMTV = DirectX::XMVectorMultiplyAdd(SphereRadius, DirectX::XMVector3Normalize(d), DirectX::XMVectorScale(d,-1.0f));// DirectX::XMVectorSet(s.Radius - DirectX::XMVectorGetX(d), 0.0f, 0.0f, 0.0f);
	//return (int)DirectX::XMVector3LessOrEqual(d2, DirectX::XMVectorMultiply(SphereRadius, SphereRadius));

	//DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&s.Center);
	//DirectX::XMVECTOR SphereRadius = DirectX::XMVectorReplicatePtr(&s.Radius);

	//DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&box1.Center);
	//DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&box1.Extents);

	DirectX::XMVECTOR BoxMin = DirectX::XMVectorSubtract(BoxCenter, BoxExtents);
	DirectX::XMVECTOR BoxMax = DirectX::XMVectorAdd(BoxCenter, BoxExtents);

	// Find the distance to the nearest point on the box.
	// for each i in (x, y, z)
	// if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
	// else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2

	DirectX::XMVECTOR d = DirectX::XMVectorZero();

	// Compute d for each dimension.
	DirectX::XMVECTOR LessThanMin = DirectX::XMVectorLess(SphereCenter, BoxMin);
	DirectX::XMVECTOR GreaterThanMax = DirectX::XMVectorGreater(SphereCenter, BoxMax);

	DirectX::XMVECTOR MinDelta = DirectX::XMVectorSubtract(SphereCenter, BoxMin);
	DirectX::XMVECTOR MaxDelta = DirectX::XMVectorSubtract(SphereCenter, BoxMax);

	// Choose value for each dimension based on the comparison.
	d = DirectX::XMVectorSelect(d, MinDelta, LessThanMin);
	d = DirectX::XMVectorSelect(d, MaxDelta, GreaterThanMax);


	// Use a dot-product to square them and sum them together.
	DirectX::XMVECTOR d2 = DirectX::XMVector3Dot(d, d);

	if (DirectX::XMVector3Greater(d2, DirectX::XMVectorMultiply(SphereRadius, SphereRadius)))
		return 0;

	DirectX::XMVECTOR InsideAll = DirectX::XMVectorLessOrEqual(DirectX::XMVectorAdd(BoxMin, SphereRadius), SphereCenter);
	InsideAll = DirectX::XMVectorAndInt(InsideAll, DirectX::XMVectorLessOrEqual(SphereCenter, DirectX::XMVectorSubtract(BoxMax,SphereRadius)));
	InsideAll = DirectX::XMVectorAndInt(InsideAll, DirectX::XMVectorGreater(DirectX::XMVectorSubtract(BoxMax, BoxMin), SphereRadius));

	outMTV = DirectX::XMVectorMultiplyAdd(SphereRadius, DirectX::XMVector3Normalize(d), DirectX::XMVectorScale(d, -1.0f));

	return (DirectX::XMVector3EqualInt(InsideAll, DirectX::XMVectorTrueInt())) ? 2 : 1;
}


int Collision::_FindSizeOfVertices(unsigned int* indices, int nrOfIndices, int startPoint, int* foundVertices)
{
	if (!indices)
	{
		throw ErrorMsg(7000020, L"indices not defined in function _FindSizeOfVertices");
	}
	if (!foundVertices)
	{
		throw ErrorMsg(7000021, L"foundVertices not defined in function _FindSizeOfVertices");
	}

	for (int i = 0; i < nrOfIndices; i++)
	{
		foundVertices[i] = -1;
	}
	int counter = 0;
	bool found = false;
	foundVertices[counter] = indices[startPoint];
	counter++;
	for (int i = startPoint + 1; i < nrOfIndices+startPoint; i++)
	{
		for (int j = 0; j < counter; j++)
		{
			if (foundVertices[j] == indices[i])
			{
				found = true;
				j = counter;
			}
		}
		if (!found)
		{
			foundVertices[counter] = indices[i];
			counter++;
		}
		found = false;
	}
	return counter;
}

QuadTree::QuadTree(const std::vector<Entity>& entities, const std::vector<DirectX::BoundingBox>& boxes)
{
	for (uint i = 0; i < entities.size(); i++)
	{
		DirectX::BoundingBox::CreateMerged(_root, _root, boxes[i]);
		
	}
	_entites = entities;
	_boxes = boxes;

	_CreateChildren();
}

QuadTree::QuadTree(const std::vector<Entity>& entities, const std::vector<DirectX::BoundingBox>& boxes, const  DirectX::BoundingBox& root)
{
	_root = root;
	_entites = entities;
	_boxes = boxes;

	_CreateChildren();
}

QuadTree::~QuadTree()
{
	for (uint i = 0; i < 4;i++)
	{
		SAFE_DELETE(_children[i]);
	}
}

const void QuadTree::GetEntitiesInFrustum(const DirectX::BoundingFrustum & frustum, std::vector<Entity>& entites)
{
	DirectX::BoundingSphere s;
	s.CreateFromFrustum(s, frustum);


	DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&s.Center);
	DirectX::XMVECTOR SphereRadius = DirectX::XMVectorReplicatePtr(&s.Radius);

	DirectX::XMVECTOR BoxCenter = DirectX::XMLoadFloat3(&_root.Center);
	DirectX::XMVECTOR BoxExtents = DirectX::XMLoadFloat3(&_root.Extents);


	DirectX::XMVECTOR radiusVec = DirectX::XMVector3Length(BoxExtents);
	float radius = DirectX::XMVectorGetByIndex(radiusVec, 0);
	radiusVec = DirectX::XMVector3Length(SphereRadius);
	radius += DirectX::XMVectorGetByIndex(radiusVec, 0);

	DirectX::XMVECTOR distanceVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(BoxCenter, SphereCenter));// DirectX::XMVectorSet(_root.Center.x - s.Center.x, _root.Center.y - s.Center.y, _root.Center.z - s.Center.z, 0));
	float distance = DirectX::XMVectorGetByIndex(distanceVec, 0);

	if (distance > radius)
	{
		return; // Too far away
	}


	int test = frustum.Contains(_root);

	switch (test)
	{
	case 0:
		// return nothing
		return;
	case 1:
		// Frustum Intersects root, check children.

		if (_children[0])
		{
			// Children found, test against children.
			for (uint i = 0; i < 4;i++)
			{
				_children[i]->GetEntitiesInFrustum(frustum, entites);
			}
		}
		else
		{
			// No children test against the single box.
			for (uint i = 0; i < _entites.size(); i++)
			{
				if (frustum.Intersects(_boxes[i]))
				{
					entites.push_back(_entites[i]);
				}
			}
		}

		return;
	case 2:
		// Frustum Contains root, return all entities.
		for (auto& e : _entites)
			entites.push_back(e);
		return;
	default:
		break;
	}
}

const bool QuadTree::GetMTV(const DirectX::BoundingSphere & s, std::function<void(DirectX::XMVECTOR& outMTV, const Entity& entity)> move)
{
	Collision col;
	int test = col.CheckSingleAgainstSingle(_root, s);
	//int test = s.Intersects(_root);

	switch (test)
	{
	case 0:
		// return nothing
		//outMTV = DirectX::XMVectorZero();
		return false;
	case 1:
	case 2:
		// Frustum Intersects root, check children.
		if (_children[0])
		{
		
			// Children found, test against children.
			for (uint i = 0; i < 4;i++)
			{
				if (_children[i]->GetMTV(s, move))
				{
				 //	return true;
				}
			}
		}
		else
		{
			DirectX::XMVECTOR vector;
			// No children test against the single box.
			for (uint i = 0; i < _entites.size(); i++)
			{
				if (col.CheckSingleAgainstSingle(_boxes[i], s, vector))
				{
					move(vector, _entites[i]);
					//return true;
				}
			}
		}
	}

	return false;
}

void QuadTree::_CreateChildren()
{
	if (_entites.size() > 24)
	{
		// Top left box
		DirectX::BoundingBox box;
		box.Center = DirectX::XMFLOAT3(_root.Center.x - _root.Extents.x / 2.0f, _root.Center.y, _root.Center.z + _root.Extents.z / 2.0f);
		box.Extents = DirectX::XMFLOAT3(_root.Extents.x / 2.0f, _root.Extents.y, _root.Extents.z / 2.0f);

		std::vector<Entity> ents;
		std::vector<DirectX::BoundingBox> boxes;

		for (uint i = 0; i < _entites.size(); i++)
		{
			if (box.Intersects(_boxes[i]))
			{
				ents.push_back(_entites[i]);
				boxes.push_back(_boxes[i]);
			}
		}

		_children[0] = new QuadTree(ents, boxes, box);


		// Top right box
		box.Center = DirectX::XMFLOAT3(_root.Center.x + _root.Extents.x / 2.0f, _root.Center.y, _root.Center.z + _root.Extents.z / 2.0f);
	
		ents.clear();
		boxes.clear();
		for (uint i = 0; i < _entites.size(); i++)
		{
			if (box.Intersects(_boxes[i]))
			{
				ents.push_back(_entites[i]);
				boxes.push_back(_boxes[i]);
			}
		}

		_children[1] = new QuadTree(ents, boxes, box);

		// Bottom left box
		box.Center = DirectX::XMFLOAT3(_root.Center.x - _root.Extents.x / 2.0f, _root.Center.y, _root.Center.z - _root.Extents.z / 2.0f);
	
		ents.clear();
		boxes.clear();
		for (uint i = 0; i < _entites.size(); i++)
		{
			if (box.Intersects(_boxes[i]))
			{
				ents.push_back(_entites[i]);
				boxes.push_back(_boxes[i]);
			}
		}

		_children[2] = new QuadTree(ents, boxes, box);

		// Bottom right box
		box.Center = DirectX::XMFLOAT3(_root.Center.x + _root.Extents.x / 2.0f, _root.Center.y, _root.Center.z - _root.Extents.z / 2.0f);
	
		ents.clear();
		boxes.clear();
		for (uint i = 0; i < _entites.size(); i++)
		{
			if (box.Intersects(_boxes[i]))
			{
				ents.push_back(_entites[i]);
				boxes.push_back(_boxes[i]);
			}
		}

		_children[3] = new QuadTree(ents, boxes, box);
	}

}
