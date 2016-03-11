// ############################################################################
// Simply outputs data to clear G-Buffer render targets to default values.
// ############################################################################
cbuffer OncePerFrameConstantsBuffer : register(b0)
{
	float4x4 View;
	float4x4 Proj;
	float4x4 ViewProj;
	float4x4 InvView;
	float4x4 InvProj;
	float4x4 InvViewProj;
	float4x4 Ortho;
	float3 CameraPosition;
	float DrawDistance;
	float ViewDistance;
	float gBackbufferWidth;
	float gBackbufferHeight;
	float gamma;
}
struct VS_OUT
{
	float4 PosH : SV_POSITION;
};

VS_OUT VS( uint VertexID : SV_VertexID )
{
	VS_OUT output = (VS_OUT)0;

	output.PosH.x = (VertexID == 2) ? 3.0f : -1.0f;
	output.PosH.y = (VertexID == 0) ? -3.0f : 1.0f;
	output.PosH.zw = 1.0f;

	return output;
}

struct PS_OUT
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Emissive : SV_TARGET2;
};

PS_OUT PS( VS_OUT input )
{
	PS_OUT output = (PS_OUT)0;

	// Black color
	output.Color = 0.0f;

	// When transforming 0.5f into [-1,1], we will get 0.0f
	output.Normal.rgb = 0.5f;

	// Dielectric
	output.Normal.a = 0.0f;

	output.Emissive = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return output;
}