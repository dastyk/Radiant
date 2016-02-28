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
cbuffer Lightdata : register(b1)
{
	float3 DirectionVS;
	float CosOuter;
	float CosInner;
	float3 Color;
	float3 PositionVS;
	float RangeRcp;
	float4x4 world;
	int volumetrick;
	float Intensity;
	float2 pad;
};

Texture2D ldep : register(t0);
Texture2D gdep : register(t1);
SamplerState TriLinearSam : register(s0);

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float4 PosV : POSITION0;
	float4 PosL : POSITION1;
	float3 Normal : NORMAL;
};

float4 main(VS_OUT input) : SV_TARGET
{
	input.PosL.xy /= input.PosL.w;
	float2 smTex = float2(0.5*input.PosL.x + 0.5f, -0.5f*input.PosL.y + 0.5f);
	float d3 = ldep.Sample(TriLinearSam, smTex);
	float d2 = gdep.Sample(TriLinearSam, smTex);
	float d = input.PosH.z / input.PosH.w;
	d = d - max(d3,d2);

	float a = pow(0.1f, 2);
	
	
	// Calculate draw distance fog

	float r = 5.0;
	float fogFactor = max(DrawDistance - input.PosV.z - r, 0.0f) / (DrawDistance - r);

	return float4(Color*d*0.7, 1.0f);
	//return float4(input.Normal, 1.0f);
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
	//return float4(a, a, a, 1.0f);
}