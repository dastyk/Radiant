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
		pos[0] = XMLoadFloat3( (XMFLOAT3*)_AttributeStreams[positionStream].Attributes.data() + triangles[t].AttributeIndices[0][positionStream] );
		pos[1] = XMLoadFloat3( (XMFLOAT3*)_AttributeStreams[positionStream].Attributes.data() + triangles[t].AttributeIndices[1][positionStream] );
		pos[2] = XMLoadFloat3( (XMFLOAT3*)_AttributeStreams[positionStream].Attributes.data() + triangles[t].AttributeIndices[2][positionStream] );

		for ( unsigned e = 0; e < 3; ++e )
		{
			XMVECTOR edgeVec = pos[(e + 1) % 3] - pos[e];

			// Check every other vertex position against the edges of this triangle.
			for ( unsigned v = 0; v < _AttributeStreams[positionStream].Attributes.size() / 3; ++v )
			{
				XMFLOAT3 testVertex = ((XMFLOAT3*)_AttributeStreams[positionStream].Attributes.data())[v];
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

const void Mesh::CalcNTB()
{
	std::vector<float> posf = AttributeData(FindStream(AttributeType::Position));
	XMFLOAT3* pos = (XMFLOAT3*)posf.data();

	std::vector<float> uvsf = AttributeData(FindStream(AttributeType::TexCoord));
	XMFLOAT2* uvs = (XMFLOAT2*)uvsf.data();

	const unsigned int* indices = AttributeIndices(FindStream(AttributeType::Position));

	std::vector<XMFLOAT3> normals(posf.size(), XMFLOAT3(0.0f, 0.0f, 0.0f));

	
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
	//FlipNormals();
	//InvertV();

	AddAttributeStream(Mesh::AttributeType::Normal, normals.size(), (float*)&normals[0], _indexCount, (unsigned int*)indices);

	std::vector<DirectX::XMFLOAT3> tangents(normals.size(), XMFLOAT3(0.0f, 0.0f, 0.0f));
	std::vector<DirectX::XMFLOAT3> binormals(normals.size(), XMFLOAT3(0.0f, 0.0f, 0.0f));

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
		XMFLOAT3 sdir = XMFLOAT3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 *y2) * r, (t2 * z1 - t1 * z2) * r);
		XMFLOAT3 tdir = XMFLOAT3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

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



	AddAttributeStream(Mesh::AttributeType::Tangent, tangents.size(), (float*)&tangents[0], _indexCount, (unsigned int*)indices);
	AddAttributeStream(Mesh::AttributeType::Binormal, binormals.size(), (float*)&binormals[0], _indexCount, (unsigned int*)indices);

}
