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
}

cbuffer b : register(b1)
{
	float3 Pos;
	float Range;
	float3 Color;
	float Intensity;
	int visible;
	int inFrustum;
	float blobRange;
	int volumetrick;
};


struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float4 PosV : POSITION0;
//	float4 PosL : POSITION1;
	float3 Normal : NORMAL;
};

float main(VS_OUT input) : SV_TARGET
{
	return input.PosH.z/input.PosH.w;// float4(Color, 1.0f);
}