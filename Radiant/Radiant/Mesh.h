#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include <DirectXMath.h>
class Mesh
{
public:
	//  unused  unique  components  byte size
	// |--------|--------|--------|--------|
	enum class AttributeType : unsigned
	{
		Position = (1u << 16) | (3u << 8) | 12u,
		TexCoord = (2u << 16) | (2u << 8) | 8u,
		Normal = (3u << 16) | (3u << 8) | 12u,
		Binormal = (4u << 16) | (3u << 8) | 12u,
		Tangent = (5u << 16) | (3u << 8) | 12u
	};

	struct AttributeStream
	{
		AttributeType Type;
		std::vector<float> Attributes;
		unsigned *Indices;
	};

	struct Batch
	{
		unsigned StartIndex;
		unsigned IndexCount;
	};
	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 normal;

		Vertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 n) : Pos(p), normal(n)
		{

		}
	};
	struct Face
	{
		unsigned long x, y, z;
		Face(unsigned long x, unsigned long y, unsigned long z) : x(x), y(y), z(z)
		{
		}
	};
	struct PointPair
	{
		unsigned long p1, p2, point;
		PointPair(unsigned long pp1, unsigned long pp2, unsigned long ppoint) : p1(pp1), p2(pp2), point(ppoint)
		{
		}
		bool operator==(PointPair& other)
		{
			if (p1 == other.p1 || p1 == other.p2)
				if (p2 == other.p2 || p2 == other.p1)
					return true;
			return false;
		}
	};
public:
	Mesh( void );
	~Mesh();

	unsigned IndexCount() const { return _indexCount; }
	unsigned BatchCount() const { return static_cast<unsigned int>(_Batches.size()); }

	// Assumes that the indices are arranged to match batches.
	void AddAttributeStream( AttributeType type, unsigned attributeCount, float *attributes, unsigned indexCount, unsigned *indices );
	void AddBatch( unsigned startIndex, unsigned indexCount );

	int FindStream( AttributeType type ) const;
	const std::vector<float>& AttributeData( int stream ) const { return _AttributeStreams[stream].Attributes; }
	const unsigned* AttributeIndices( int stream ) const { return _AttributeStreams[stream].Indices; }
	const std::vector<Batch>& Batches( void ) const { return _Batches; }

	unsigned FixTJunctions( void );
	void FlipPositionZ( void );
	void FlipNormals( void );
	void FlipTangents(void);
	void FlipBitangents(void);
	void InvertV( void );
	// Calculate tangent space (should be able to do using positions and uv) http://www.terathon.com/code/tangent.html

	inline static unsigned Components( AttributeType type ) { return ((static_cast<unsigned>(type) >> 8) & 0xFF); }
	inline static unsigned ByteSize( AttributeType type ) { return (static_cast<unsigned>(type) & 0xFF); }


	const void CalcNTB();
	void CalcNormals();

	const void GenerateSphere(unsigned detail = 3);
	const void GenerateCube(float sidelen = 1);
	unsigned long GetMiddlePoint(unsigned long p1, unsigned long p2, std::vector<Vertex>& v);
private:
	Mesh( const Mesh& rhs );
	Mesh& operator=( const Mesh& rhs );

private:
	unsigned _indexCount = 0;
	std::vector<AttributeStream> _AttributeStreams;
	std::vector<Batch> _Batches;
	std::vector<PointPair> _points;
};

#endif // _MESH_H_