struct Segment
{
	float Intensity;
	float3 Color;
};

StructuredBuffer<Segment> Segments : register(t0);

cbuffer Material : register(b1)
{
	float3 BoltColor;
}

float4 PS( float4 PosH : SV_POSITION, uint SegmentIndex : SEGMENT ) : SV_TARGET
{
	Segment seg = Segments[SegmentIndex];
	
	return float4(seg.Color * seg.Intensity + 0.000001f * BoltColor, 0.0f);
	return float4(BoltColor * seg.Intensity, 0.0f);
}