// b0 is once per frame something buffer

cbuffer a : register(b1)
{
	float4x4 gWVP;
	float4x4 gWorldViewInvTrp;
	float4x4 gWorld;
	float4x4 gWorldView;
};

void VS( in float3 PosW : POSITION, in uint VertexID : SV_VertexID, out float4 PosH : SV_POSITION, out uint Segment : SEGMENT )
{
	PosH = mul( float4(PosW, 1.0f), gWVP );
	Segment = VertexID / 2; // intentional dropping of fractions
}