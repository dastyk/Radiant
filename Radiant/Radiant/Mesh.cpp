#include "Mesh.h"

#include <DirectXMath.h>
#include <algorithm>
#include "Utils.h"

using namespace std;
using namespace DirectX;

Mesh::Mesh( void )
{
}

// OBS! Vid tilldelning kommer pekarna för dessa objekt att kopieras över, men
// inte datan de pekar på, och när den temporära Modelvariabeln destrueras
// rensas datan som pekarna pekar på ut och det blir problem. För att lösa det
// kan man göra egna copy constructor, copy assignment operator, och destructor.
// Kom ihåg Rule of Three: Behövs en av dessa, behövs antagligen alla. I detta
// fall används en Initialize, men destructorn körs vid tilldelning icke desto
// mindre eftersom först: Model mModel; och senare mModel = Model(); Konstruktor
// körs två gånger och en tilldelning sker > destruktorn anropas på tempvariabel.
Mesh::~Mesh()
{
	for ( unsigned i = 0; i < _AttributeStreams.size(); ++i )
	{
		SAFE_DELETE_ARRAY( _AttributeStreams[i].Indices );
	}
}

void Mesh::AddAttributeStream( AttributeType type, unsigned attributeCount, float *attributes, unsigned indexCount, unsigned *indices )
{
	if ( _indexCount && (_indexCount != indexCount) )
	{
		TraceDebug( "The stream currently being added does not have the same number of indices as the previous ones.\nPrevious number of indices: %d\nThis number of indices: %d", _indexCount, indexCount );
		
		return;
	}

	_indexCount = indexCount;

	AttributeStream stream = AttributeStream();
	stream.Type = type;
	unsigned components = Components( type );
	stream.Attributes.resize( attributeCount * components );
	memcpy( stream.Attributes.data(), attributes, ByteSize( type ) * attributeCount );
	stream.Indices = new unsigned[indexCount];
	memcpy( stream.Indices, indices, sizeof( unsigned ) * indexCount );

	_AttributeStreams.push_back( std::move( stream ) );
}

void Mesh::AddBatch( unsigned startIndex, unsigned indexCount )
{
	Batch batch;
	batch.StartIndex = startIndex;
	batch.IndexCount = indexCount;

	_Batches.push_back( batch );
}

int Mesh::FindStream( AttributeType type ) const
{
	for ( unsigned i = 0; i < _AttributeStreams.size(); ++i )
	{
		if ( _AttributeStreams[i].Type == type )
			return signed( i );
	}

	return -1;
}

// Returns number of T-junctions found.
unsigned Mesh::FixTJunctions( void )
{
	int positionStream = FindStream( AttributeType::Position );
	if ( positionStream == -1 )
	{
		TraceDebug( "Could not find a position stream!" );
		return 0;
	}

	struct Triangle
	{
		unsigned Batch;
		unsigned *AttributeIndices[3]; // Attribute indices for each vertex.
	};

	vector<Triangle> triangles;

	// Simplification step: organize vertices into triangles.
	for ( unsigned batch = 0; batch < _Batches.size(); ++batch )
	{
		unsigned endIndex = _Batches[batch].StartIndex + _Batches[batch].IndexCount;

		for ( unsigned i = _Batches[batch].StartIndex; i < endIndex; i += 3 )
		{
			Triangle t;
			t.Batch = batch;

			for ( unsigned vertex = 0; vertex < 3; ++vertex )
			{
				t.AttributeIndices[vertex] = new unsigned[_AttributeStreams.size()];

				for ( unsigned attribute = 0; attribute < _AttributeStreams.size(); ++attribute )
				{
					t.AttributeIndices[vertex][attribute] = _AttributeStreams[attribute].Indices[i + vertex];
				}
			}

			triangles.push_back( t );
		}
	}

	// The idea here is to check every vertex position against every triangle
	// edge. If the vertex lies approximately on the edge, it's a T-junction.
	// When a T-junction is found, the other attributes are interpolated from
	// the edge vertices and the triangle is split into two on this vertex.
	//        _             _
	//       / \           /|\
		//      /   \   ==>   / | \
	//     /__.__\       /__!__\
	//        ^-- T-junction

	unsigned numTJunctions = 0;

	for ( unsigned t = 0; t < triangles.size(); ++t )
	{
	restart:

		// Get the positions of the vertices of this triangle.
		XMVECTOR pos[3];
		pos[0] = XMLoadFloat3( (DirectX::XMFLOAT3*)_AttributeStreams[positionStream].Attributes.data() + triangles[t].AttributeIndices[0][positionStream] );
		pos[1] = XMLoadFloat3( (DirectX::XMFLOAT3*)_AttributeStreams[positionStream].Attributes.data() + triangles[t].AttributeIndices[1][positionStream] );
		pos[2] = XMLoadFloat3( (DirectX::XMFLOAT3*)_AttributeStreams[positionStream].Attributes.data() + triangles[t].AttributeIndices[2][positionStream] );

		for ( unsigned e = 0; e < 3; ++e )
		{
			XMVECTOR edgeVec = pos[(e + 1) % 3] - pos[e];

			// Check every other vertex position against the edges of this triangle.
			for ( unsigned v = 0; v < _AttributeStreams[positionStream].Attributes.size() / 3; ++v )
			{
				DirectX::XMFLOAT3 testVertex = ((DirectX::XMFLOAT3*)_AttributeStreams[positionStream].Attributes.data())[v];
				XMVECTOR testVec = XMLoadFloat3( &testVertex ) - pos[e];

				float edgeDotEdge = XMVectorGetX( XMVector3Dot( edgeVec, edgeVec ) );
				float testDotEdge = XMVectorGetX( XMVector3Dot( testVec, edgeVec ) );
				float c = testDotEdge / edgeDotEdge;

				// Test vertex to lie on the interior, excluding the edge
				// vertices (avoid self checks).
				if ( c > 0.00001f && c < 0.99999f )
				{
					// Test squared distance from test point to edge.
					XMVECTOR testToEdge = c * edgeVec - testVec;
					if ( XMVectorGetX( XMVector3Dot( testToEdge, testToEdge ) ) < 0.0001f )
					{
						// If we reached here, we found a T-junction!
						numTJunctions++;

						unsigned edgeVert1 = e;
						unsigned edgeVert2 = (e + 1) % 3;
						unsigned oddVert = (e + 2) % 3;

						Triangle newTri;
						newTri.Batch = triangles[t].Batch;
						for ( unsigned i = 0; i < 3; ++i )
						{
							newTri.AttributeIndices[i] = new unsigned[_AttributeStreams.size()];
						}

						// Set up new and old triangle, one attribute at a time.
						for ( unsigned i = 0; i < _AttributeStreams.size(); ++i )
						{
							// Position stream is special in that it does not interpolate,
							// rather it uses the exact same position.
							if ( signed( i ) == positionStream )
							{
								// Vertex positions of new triangle
								newTri.AttributeIndices[0][positionStream] = v; // T-junction vertex
								newTri.AttributeIndices[1][positionStream] = triangles[t].AttributeIndices[oddVert][positionStream]; // Reuse index of oddVert
								newTri.AttributeIndices[2][positionStream] = triangles[t].AttributeIndices[edgeVert1][positionStream]; // Reuse index of edgeVert1

																																	   // Vertex positions of old triangle. One vertex is changed to T-junction.
								triangles[t].AttributeIndices[edgeVert1][0] = v;

								continue;
							}
							// The other attributes are interpolated and create new vertices.
							else
							{
								unsigned compCount = Components( _AttributeStreams[i].Type );
								unsigned interpolatedIndex = static_cast<unsigned int>(_AttributeStreams[i].Attributes.size()) / compCount;

								newTri.AttributeIndices[0][i] = interpolatedIndex;
								newTri.AttributeIndices[1][i] = triangles[t].AttributeIndices[oddVert][i]; // Reuse index of oddVert
								newTri.AttributeIndices[2][i] = triangles[t].AttributeIndices[edgeVert1][i]; // Reuse index of edgeVert1

																											 // Interpolate the components of the attribute
								for ( unsigned comp = 0; comp < compCount; ++comp )
								{
									unsigned index = triangles[t].AttributeIndices[edgeVert1][i];
									float vert1Val = _AttributeStreams[i].Attributes[compCount * index + comp];

									index = triangles[t].AttributeIndices[edgeVert2][i];
									float vert2Val = _AttributeStreams[i].Attributes[compCount * index + comp];

									float interpolated = vert1Val + c * (vert2Val - vert1Val);
									_AttributeStreams[i].Attributes.push_back( interpolated );
								}

								// Update the current triangle when it's data is no longer needed.
								// Change edgeVert1 to the T-junction.
								triangles[t].AttributeIndices[edgeVert1][i] = interpolatedIndex;
							}
						}

						triangles.push_back( newTri );

						// When a T-junction has been fixed we want to restart checking
						// vertices, but continue from the current triangle (the
						// previous ones have already been checked!).
						goto restart;
					}
				}
			}
		}
	}

	// Sort triangles on batches and set the new values up.
	// Recreate indices of the attributes according to the triangles.
	sort( triangles.begin(), triangles.end(), []( const Triangle &a, const Triangle &b ) { return a.Batch < b.Batch; } );

	_indexCount = 3 * static_cast<unsigned int>(triangles.size());

	for ( unsigned i = 0; i < _AttributeStreams.size(); ++i )
	{
		delete[] _AttributeStreams[i].Indices;
		_AttributeStreams[i].Indices = new unsigned[_indexCount];
	}

	unsigned batch = 0;
	unsigned startIndex = 0;
	unsigned indexCount = 0;
	for ( unsigned t = 0; t < triangles.size(); ++t )
	{
		if ( triangles[t].Batch != batch )
		{
			_Batches[batch].StartIndex = startIndex;
			_Batches[batch].IndexCount = indexCount;
			startIndex += indexCount;
			indexCount = 0;
			batch = triangles[t].Batch;
		}

		for ( unsigned a = 0; a < _AttributeStreams.size(); ++a )
		{
			// Update the stream index buffer with new indices from triangle vertices.
			_AttributeStreams[a].Indices[3 * t + 0] = triangles[t].AttributeIndices[0][a];
			_AttributeStreams[a].Indices[3 * t + 1] = triangles[t].AttributeIndices[1][a];
			_AttributeStreams[a].Indices[3 * t + 2] = triangles[t].AttributeIndices[2][a];
		}

		// Triangle will not be used any more, time to free memory allocated
		// when setting up the triangles (and splitting when T-junction found).
		delete[] triangles[t].AttributeIndices[0];
		delete[] triangles[t].AttributeIndices[1];
		delete[] triangles[t].AttributeIndices[2];

		indexCount += 3;
	}

	_Batches[batch].StartIndex = startIndex;
	_Batches[batch].IndexCount = indexCount;

	return numTJunctions;
}

void Mesh::FlipPositionZ( void )
{
	int positionStream = FindStream( AttributeType::Position );

	if ( positionStream == -1 )
	{
		TraceDebug( "Position stream not found" );
		return;
	}

	float *attributeComps = _AttributeStreams[positionStream].Attributes.data();
	unsigned componentCount = static_cast<unsigned int>(_AttributeStreams[positionStream].Attributes.size());

	for ( unsigned i = 2; i < componentCount; i += 3 )
	{
		attributeComps[i] *= -1.0f;
	}
}

void Mesh::FlipNormals( void )
{
	int normalStream = FindStream( AttributeType::Normal );

	if ( normalStream == -1 )
	{
		TraceDebug( "Normal stream not found" );
		return;
	}

	float *attributeComps = _AttributeStreams[normalStream].Attributes.data();
	unsigned componentCount = static_cast<unsigned int>(_AttributeStreams[normalStream].Attributes.size());

	for ( unsigned i = 2; i < componentCount; i += 3 )
	{
		attributeComps[i] *= -1.0f;
	}
}

void Mesh::FlipTangents(void)
{
	int tangStream = FindStream(AttributeType::Tangent);
	if (tangStream == -1)
	{
		TraceDebug("Tangent stream not found");
		return;
	}
	float* attComps = _AttributeStreams[tangStream].Attributes.data();
	unsigned count = static_cast<unsigned int>(_AttributeStreams[tangStream].Attributes.size());

	for (unsigned i = 0; i < count; ++i)
	{
		attComps[i] *= -1.0f;
	}
}
void Mesh::FlipBitangents(void)
{
	int bitangStream = FindStream(AttributeType::Binormal);
	if (bitangStream == -1)
	{
		TraceDebug("Tangent stream not found");
		return;
	}
	float* attComps = _AttributeStreams[bitangStream].Attributes.data();
	unsigned count = static_cast<unsigned int>(_AttributeStreams[bitangStream].Attributes.size());

	for (unsigned i = 0; i < count; ++i)
	{
		attComps[i] *= -1.0f;
	}
}

void Mesh::InvertV( void )
{
	int texStream = FindStream( AttributeType::TexCoord );

	if ( texStream == -1 )
	{
		TraceDebug( "Tex stream not found" );
		return;
	}

	float *attributeComps = _AttributeStreams[texStream].Attributes.data();
	unsigned componentCount = static_cast<unsigned int>(_AttributeStreams[texStream].Attributes.size());

	for ( unsigned i = 1; i < componentCount; i += 2 )
	{
		attributeComps[i] = 1.0f - attributeComps[i];
	}
}

void Mesh::CalcNormals()
{
	std::vector<float> posf = AttributeData(FindStream(AttributeType::Position));
	XMFLOAT3* pos = (XMFLOAT3*)posf.data();

	const unsigned int* indices = AttributeIndices(FindStream(AttributeType::Position));

	std::vector<DirectX::XMFLOAT3> normals(posf.size(), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	for (unsigned int i = 0; i < _indexCount; i += 3)
	{
		XMVECTOR pos1 = XMLoadFloat3(&pos[indices[i]]);
		XMVECTOR pos2 = XMLoadFloat3(&pos[indices[i + 1]]);
		XMVECTOR pos3 = XMLoadFloat3(&pos[indices[i + 2]]);

		XMVECTOR v1 = pos1 - pos2;
		XMVECTOR v2 = pos3 - pos2;

		XMVECTOR normal = XMVector3Normalize(XMVector3Cross(v1, v2));
		XMStoreFloat3(&normals[indices[i]], normal);
		XMStoreFloat3(&normals[indices[i + 1]], normal);
		XMStoreFloat3(&normals[indices[i + 2]], normal);
	}
	AddAttributeStream(Mesh::AttributeType::Normal, static_cast<unsigned int>(normals.size()), (float*)&normals[0], _indexCount, (unsigned int*)indices);
}

const void Mesh::CalcNTB()
{
	std::vector<float> posf = AttributeData(FindStream(AttributeType::Position));
	DirectX::XMFLOAT3* pos = (DirectX::XMFLOAT3*)posf.data();

	std::vector<float> uvsf = AttributeData(FindStream(AttributeType::TexCoord));
	XMFLOAT2* uvs = (XMFLOAT2*)uvsf.data();

	const unsigned int* indices = AttributeIndices(FindStream(AttributeType::Position));

	std::vector<DirectX::XMFLOAT3> normals(posf.size(), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	for (unsigned int i = 0; i < _indexCount; i += 3)
	{
		XMVECTOR pos1 = XMLoadFloat3(&pos[indices[i]]);
		XMVECTOR pos2 = XMLoadFloat3(&pos[indices[i + 1]]);
		XMVECTOR pos3 = XMLoadFloat3(&pos[indices[i + 2]]);

		XMVECTOR v1 = pos1 - pos2;
		XMVECTOR v2 = pos3 - pos2;

		XMVECTOR normal = XMVector3Normalize(XMVector3Cross(v1, v2));
		XMStoreFloat3(&normals[indices[i]], normal);
		XMStoreFloat3(&normals[indices[i + 1]], normal);
		XMStoreFloat3(&normals[indices[i + 2]], normal);
	}
	

	AddAttributeStream(Mesh::AttributeType::Normal, static_cast<unsigned int>(normals.size()), (float*)&normals[0], _indexCount, (unsigned int*)indices);



	std::vector<DirectX::XMFLOAT3> tangents(normals.size(), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	std::vector<DirectX::XMFLOAT3> binormals(normals.size(), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	for (unsigned int i = 0; i < _indexCount; i += 3)
	{
		DirectX::XMFLOAT3 tp1;
		DirectX::XMFLOAT3 tp2;
		DirectX::XMFLOAT3 tp3;
		DirectX::XMFLOAT3 tn1;
		DirectX::XMFLOAT3 tn2;
		DirectX::XMFLOAT3 tn3;
		DirectX::XMFLOAT2 uv1;
		DirectX::XMFLOAT2 uv2;
		DirectX::XMFLOAT2 uv3;

		tp1 = pos[indices[i]];
		tp2 = pos[indices[i + 1]];
		tp3 = pos[indices[i + 2]];

		tn1 = normals[indices[i]];
		tn2 = normals[indices[i + 1]];
		tn3 = normals[indices[i + 2]];

		uv1 = uvs[indices[i]];
		uv2 = uvs[indices[i + 1]];
		uv3 = uvs[indices[i + 2]];

		float x1 = tp2.x - tp1.x;
		float x2 = tp3.x - tp1.x;
		float y1 = tp2.y - tp1.y;
		float y2 = tp3.y - tp1.y;
		float z1 = tp2.z - tp1.z;
		float z2 = tp3.z - tp1.z;

		float s1 = uv2.x - uv1.x;
		float s2 = uv3.x - uv1.x;
		float t1 = uv2.y - uv1.y;
		float t2 = uv3.y - uv1.y;

		float r = 1.0f / (s1 * t2 - s2 * t1);
		DirectX::XMFLOAT3 sdir = DirectX::XMFLOAT3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 *y2) * r, (t2 * z1 - t1 * z2) * r);
		DirectX::XMFLOAT3 tdir = DirectX::XMFLOAT3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

		/*printf("%f, %f, %f\n", tn1.x, tn1.y, tn1.z);
		printf("%f, %f, %f\n", tdir.x, tdir.y, tdir.z);
		printf("%f, %f, %f\n", sdir.x, sdir.y, sdir.z);*/


		XMVECTOR tan = XMLoadFloat3(&sdir);
		XMVECTOR nor = XMLoadFloat3(&tn1);

		tan = XMVector3Normalize(XMVectorSubtract(tan, XMVectorScale(nor, XMVectorGetX(XMVector3Dot(nor, tan)))));
		float m = XMVectorGetX(XMVector3Dot(XMVector3Cross(nor, tan), XMLoadFloat3(&sdir))) < 0.0f ? -1.0f : 1.0f;
		XMVECTOR bit = XMVectorScale(XMVector3Cross(nor, tan), m);

		XMStoreFloat3(&tangents[indices[i]], tan);
		XMStoreFloat3(&tangents[indices[i + 1]], tan);
		XMStoreFloat3(&tangents[indices[i + 2]], tan);

		XMStoreFloat3(&binormals[indices[i]], bit);
		XMStoreFloat3(&binormals[indices[i + 1]], bit);
		XMStoreFloat3(&binormals[indices[i + 2]], bit);
	}



	AddAttributeStream(Mesh::AttributeType::Tangent, static_cast<unsigned int>(tangents.size()), (float*)&tangents[0], _indexCount, (unsigned int*)indices);
	AddAttributeStream(Mesh::AttributeType::Binormal, static_cast<unsigned int>(binormals.size()), (float*)&binormals[0], _indexCount, (unsigned int*)indices);

}

const void Mesh::GenerateSphere(unsigned detail)
{
	_points.clear();
	// create 12 vertices of a icosahedron
	float t = sqrt(0.5f);


	std::vector<Vertex> v;
	v.push_back(Vertex(DirectX::XMFLOAT3(-t, t, 0.0f), DirectX::XMFLOAT3(-t, t, 0.0f)));
	v.push_back(Vertex(DirectX::XMFLOAT3(t, t, 0.0f), DirectX::XMFLOAT3(t, t, 0.0f)));
	v.push_back(Vertex(DirectX::XMFLOAT3(-t, -t, 0.0f), DirectX::XMFLOAT3(-t, -t, 0.0f)));
	v.push_back(Vertex(DirectX::XMFLOAT3(t, -t, 0.0f), DirectX::XMFLOAT3(t, -t, 0.0f)));

	v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, -t, -t), DirectX::XMFLOAT3(0.0f, -t, -t)));//4
	v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, t, -t), DirectX::XMFLOAT3(0.0f, t, -t)));//5
	v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, -t, t), DirectX::XMFLOAT3(0.0f, -t, t)));
	v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, t, t), DirectX::XMFLOAT3(0.0f, t, t)));

	v.push_back(Vertex(DirectX::XMFLOAT3(t, 0.0f, t), DirectX::XMFLOAT3(t, 0.0f, t)));
	v.push_back(Vertex(DirectX::XMFLOAT3(t, 0.0f, -t), DirectX::XMFLOAT3(t, 0.0f, -t)));//9
	v.push_back(Vertex(DirectX::XMFLOAT3(-t, 0.0f, t), DirectX::XMFLOAT3(-t, 0.0f, t)));
	v.push_back(Vertex(DirectX::XMFLOAT3(-t, 0.0f, -t), DirectX::XMFLOAT3(-t, 0.0f, -t)));

	// create 20 triangles of the icosahedron

	std::vector<Face> faces;

	// 5 faces around point 0
	faces.push_back(Face(0, 5, 11));
	faces.push_back(Face(0, 1, 5));
	faces.push_back(Face(0, 7, 1));
	faces.push_back(Face(0, 10, 7));
	faces.push_back(Face(0, 11, 10));

	// 5 adjacent faces
	faces.push_back(Face(1, 9, 5));
	faces.push_back(Face(5, 4, 11));
	faces.push_back(Face(11, 2, 10));
	faces.push_back(Face(10, 6, 7));
	faces.push_back(Face(7, 8, 1));

	//// 5 faces around point 3
	faces.push_back(Face(3, 4, 9));
	faces.push_back(Face(3, 2, 4));
	faces.push_back(Face(3, 6, 2));
	faces.push_back(Face(3, 8, 6));
	faces.push_back(Face(3, 9, 8));
	/////////
	// 5 adjacent faces
	faces.push_back(Face(4, 5, 9));
	faces.push_back(Face(4, 2, 11));
	faces.push_back(Face(2, 6, 10));
	faces.push_back(Face(6, 8, 7));
	faces.push_back(Face(8, 9, 1));


	// refine triangles
	for (unsigned int i = 0; i < detail; i++)
	{
		std::vector<Face> faces2 = faces;
		for (std::vector<Face>::iterator it = faces.begin(); it != faces.end(); it++)
		{
			// replace triangle by 4 triangles
			int a = GetMiddlePoint(it->x, it->y, v);
			int b = GetMiddlePoint(it->y, it->z, v);
			int c = GetMiddlePoint(it->z, it->x, v);

			faces2.push_back(Face(it->x, a, c));
			faces2.push_back(Face(it->y, b, a));
			faces2.push_back(Face(it->z, c, b));
			faces2.push_back(Face(a, b, c));
		}
		faces = faces2;
	}
	std::vector<DirectX::XMFLOAT3> pos;
	pos.resize(v.size());

	for (unsigned i = 0; i < pos.size(); i++)
	{
		pos[i] = std::move(v[i].Pos);
	}

	for (unsigned i = 0; i < pos.size(); i++)
	{
		pos[i] = std::move(v[i].normal);
	}
	AddAttributeStream(Mesh::AttributeType::Position, static_cast<unsigned int>(pos.size()), (float*)&pos[0], static_cast<unsigned int>(faces.size()) * 3, (unsigned int*)&faces[0]);
	AddAttributeStream(Mesh::AttributeType::Normal, static_cast<unsigned int>(pos.size()), (float*)&pos[0], static_cast<unsigned int>(faces.size()) * 3, (unsigned int*)&faces[0]);

	AddBatch(0, static_cast<unsigned int>(faces.size()) * 3);


}

const void Mesh::GenerateCube(float sidelen)
{
	float h = sidelen / 2.0f;
	
	float positions[8 * 3] = {
		-h, -h, h,
		h, -h, h,
		-h, h, h,
		h, h, h,
		-h, h, -h,
		h, h, -h,
		-h, -h, -h,
		h, -h, -h,
	};
	float normals[6 * 3] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, -1.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
	};

	uint32_t posIndices[6 * 2 * 3] = {
		0, 1, 2,
		2, 1, 3,

		2, 3, 4,
		4, 3, 5,

		4, 5, 6,
		6, 5, 7,

		6, 7, 0,
		0, 7, 1,

		1, 7, 3,
		3, 7, 5,

		6, 0, 4,
		4, 0, 2,
	};

	uint32_t normalIndices[6 * 2 * 3] = {
		3, 3, 3,
		3, 3, 3,

		1, 1, 1,
		1, 1, 1,

		0, 0, 0,
		0, 0, 0,

		4, 4, 4,
		4, 4, 4,

		2, 2, 2,
		2, 2, 2,

		5, 5, 5,
		5, 5, 5
	};

	AddAttributeStream(AttributeType::Position, 8, &positions[0], 36, &posIndices[0]);
	AddAttributeStream(AttributeType::Normal, 4, &normals[0], 36, &normalIndices[0]);
	AddBatch(0, 36);

	//FlipPositionZ();

	return void();
}

const void Mesh::GenerateCone(unsigned detail)
{
	float t = sqrt(0.5f);
	float t2 = sqrt(0.125f);
	float a = std::tanf(0.5);
	float r = 0.5f;

	//std::vector<Vertex> v;

	//// Point
	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -r), DirectX::XMFLOAT3(t, t, 0.0f))); // tr
	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -r), DirectX::XMFLOAT3(0.0f, 1.0, 0.0f))); // tm
	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -r), DirectX::XMFLOAT3(-t, t, 0.0f))); // tl

	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -r), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f))); // ml

	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -r), DirectX::XMFLOAT3(-t, -t, 0.0f))); // bl
	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -r), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f))); // bm
	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -r), DirectX::XMFLOAT3(t, -t, 0.0f))); // br

	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -r), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f))); // mr


	//// End
	//v.push_back(Vertex(DirectX::XMFLOAT3(t, t, r), DirectX::XMFLOAT3(t, t, 0.0f))); // tr
	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, r, r), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f))); // tm
	//v.push_back(Vertex(DirectX::XMFLOAT3(-t, t, r), DirectX::XMFLOAT3(-t, t, 0.0f))); // tl

	//v.push_back(Vertex(DirectX::XMFLOAT3(-r, 0.0f, r), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f))); // ml

	//v.push_back(Vertex(DirectX::XMFLOAT3(-t, -t, r), DirectX::XMFLOAT3(-t, -t, 0.0f))); // bl
	//v.push_back(Vertex(DirectX::XMFLOAT3(0.0f, -r, r), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f))); // bm
	//v.push_back(Vertex(DirectX::XMFLOAT3(t, -t, r), DirectX::XMFLOAT3(t, -t, 0.0f))); // br

	//v.push_back(Vertex(DirectX::XMFLOAT3(r, 0.0f, r), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f))); // mr


	std::vector<XMFLOAT3> pos;

	// Point
	pos.push_back(DirectX::XMFLOAT3(0.0f, 0.0f, -r)); // p 0

	//end
	pos.push_back(DirectX::XMFLOAT3(t2, t2, r)); // tr 1
	pos.push_back(DirectX::XMFLOAT3(0.0f, r, r)); // tm 2
	pos.push_back(DirectX::XMFLOAT3(-t2, t2, r)); // tl 3

	pos.push_back(DirectX::XMFLOAT3(-r, 0.0f, r)); // ml 4

	pos.push_back(DirectX::XMFLOAT3(-t2, -t2, r)); // bl 5
	pos.push_back(DirectX::XMFLOAT3(0.0f, -r, r)); // bm 6
	pos.push_back(DirectX::XMFLOAT3(t2, -t2, r)); // br 7

	pos.push_back(DirectX::XMFLOAT3(r, 0.0f, r)); // mr 8


	std::vector<XMFLOAT3> normals;
	normals.push_back(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)); // tr 0
	normals.push_back(DirectX::XMFLOAT3(t, t, 0.0f)); // tr 1
	normals.push_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)); // tm 2
	normals.push_back(DirectX::XMFLOAT3(-t, t, 0.0f)); // tl 3

	normals.push_back(DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f)); // ml 4

	normals.push_back(DirectX::XMFLOAT3(-t, -t, 0.0f)); // bl 5
	normals.push_back(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f)); // bm 6
	normals.push_back(DirectX::XMFLOAT3(t, -t, 0.0f)); // br 7

	normals.push_back(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f)); // mr 8

	for (auto& n : normals)
	{
		DirectX::XMVECTOR norm = DirectX::XMLoadFloat3(&n);
		norm = DirectX::XMVector3Rotate(norm, DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), -a));
		DirectX::XMStoreFloat3(&n, norm);
	}
	std::vector<Face> posIndices;

	// bottom left
	posIndices.push_back(Face(0, 6, 5));

	posIndices.push_back(Face(0, 5, 4));

	// top left
	posIndices.push_back(Face(0, 4, 3));

	posIndices.push_back(Face(0, 3, 2));
	
	// top right

	posIndices.push_back(Face(0, 2, 1));

	posIndices.push_back(Face(0, 1, 8));

	// bottom right

	posIndices.push_back(Face(0, 8, 7));

	posIndices.push_back(Face(0, 7, 6));


	std::vector<Face> normalIndices;

	// bottom left
	normalIndices.push_back(Face(6, 6, 5));

	normalIndices.push_back(Face(5, 5, 4));

	// top left
	normalIndices.push_back(Face(4, 4, 3));

	normalIndices.push_back(Face(3, 3, 2));

	// top right

	normalIndices.push_back(Face(2, 2, 1));

	normalIndices.push_back(Face(1, 1, 8));

	// bottom right

	normalIndices.push_back(Face(8, 8, 7));

	normalIndices.push_back(Face(7, 7, 6));

	std::vector<PointPair> posP;
	std::vector<PointPair> normalP;

	// refine triangles
	for (unsigned int i = 0; i < detail; i++)
	{
		std::vector<Face> posIndices2 = posIndices;
		std::vector<Face> normalIndices2 = normalIndices;

		for (unsigned int i = 0; i < posIndices.size(); i++)
		{
			Face& f = posIndices[i];
			Face& n = normalIndices[i];

			// split triangle in two
			std::pair<unsigned int, unsigned int> a = GetMiddlePoint(f.y, f.z, pos, normals);

			posIndices2.push_back(Face(0, f.y, a.first));
			posIndices2.push_back(Face(0, a.first, f.z));

			normalIndices2.push_back(Face(n.x, n.y, a.second));
			normalIndices2.push_back(Face(a.second, a.second, n.z));
		}
		posIndices = posIndices2;
		normalIndices = normalIndices2;
	}

	AddAttributeStream(Mesh::AttributeType::Position, static_cast<unsigned int>(pos.size()), (float*)&pos[0], static_cast<unsigned int>(posIndices.size()) * 3, (unsigned int*)&posIndices[0]);
	AddAttributeStream(Mesh::AttributeType::Normal, static_cast<unsigned int>(pos.size()), (float*)&pos[0], static_cast<unsigned int>(normalIndices.size()) * 3, (unsigned int*)&normalIndices[0]);

	AddBatch(0, static_cast<unsigned int>(posIndices.size()) * 3);
}

unsigned long Mesh::GetMiddlePoint(unsigned long p1, unsigned long p2, std::vector<Vertex>& v)
{
	PointPair p(p1, p2, static_cast<unsigned long>(v.size()));
	for (std::vector<PointPair>::iterator it = _points.begin(); it != _points.end(); it++)
	{
		if (p == (*it))
			return (*it).point;
	}

	Vertex v1 = v[p1];
	Vertex v2 = v[p2];

	// Get middle point
	DirectX::XMFLOAT3 pos;
	pos.x = (v1.Pos.x + v2.Pos.x) *0.5f;
	pos.y = (v1.Pos.y + v2.Pos.y) *0.5f;
	pos.z = (v1.Pos.z + v2.Pos.z) *0.5f;

	// Get length and direction from origo
	float len = sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
	DirectX::XMFLOAT3 norm;
	norm.x = pos.x / len;
	norm.y = pos.y / len;
	norm.z = pos.z / len;

	// Add the missing distance to pos
	pos.x += norm.x*(1 - len);
	pos.y += norm.y*(1 - len);
	pos.z += norm.z*(1 - len);

	_points.push_back(p);
	v.push_back(Vertex(pos, norm));

	return p.point;
}

std::pair<unsigned int, unsigned int> Mesh::GetMiddlePoint(unsigned long p1, unsigned long p2, std::vector<DirectX::XMFLOAT3>& pos, std::vector<DirectX::XMFLOAT3>& normals)
{
	float a = std::tanf(0.5);
	std::pair<unsigned int, unsigned int> out;

	DirectX::XMFLOAT3 pos1 = pos[p1];
	DirectX::XMFLOAT3 pos2 = pos[p2];
	
	DirectX::XMFLOAT3 npos;
	npos.x = (pos1.x + pos2.x) *0.5f;
	npos.y = (pos1.y + pos2.y) *0.5f;
	npos.z = 0.0f;

	// Get length and direction
	float len = sqrt(npos.x*npos.x + npos.y*npos.y);
	DirectX::XMFLOAT3 norm;
	norm.x = npos.x / len;
	norm.y = npos.y / len;
	norm.z = 0.0f;

	// Add the missing distance to pos
	npos.x += norm.x*(0.5f - len);
	npos.y += norm.y*(0.5f - len);
	npos.z = 0.5f;


	DirectX::XMVECTOR normv = DirectX::XMLoadFloat3(&norm);
	normv = DirectX::XMVector3Rotate(normv, DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), -a));
	DirectX::XMStoreFloat3(&norm, normv);

	pos.push_back(npos);
	normals.push_back(norm);

	out.first = pos.size()-1;
	out.second = normals.size()-1;

	return out;
}
