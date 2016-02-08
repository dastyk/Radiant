cbuffer a : register(b0)
{
	float4x4 gWVP;
	float4x4 gWorldViewInvTrp;
};

cbuffer a : register(b1)
{
	float3 Pos;
	float Range;
	float3 Color;
	float Intensity;
};


struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

float main(VS_OUT input) : SV_TARGET
{
	return input.PosH.z/input.PosH.w;// float4(Color, 1.0f);
}