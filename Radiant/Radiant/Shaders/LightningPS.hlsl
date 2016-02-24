struct Segment
{
	uint Intensity;
};

//StructuredBuffer<Segment> gSegments : register(t0);

cbuffer Material : register(b0)
{
	float3 BoltColor;
}

float4 PS( float4 PosH : SV_POSITION, uint Segment : SEGMENT ) : SV_TARGET
{
	//Segment seg = gSegments[Segment];

	//return float4(BoltColor * seg.Intensity, 0.0f);
	return float4(BoltColor, 0.0f);
}