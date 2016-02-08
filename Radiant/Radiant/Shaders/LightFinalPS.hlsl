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

Texture2D ldep : register(t0);
Texture2D gdep : register(t1);
SamplerState TriLinearSam : register(s0);

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float4 PosV : POSITION;
	float3 Normal : NORMAL;
};

float4 main(VS_OUT input) : SV_TARGET
{
	input.PosV /= input.PosV.w;
	float2 tex = (input.PosV.xy + float2(1.0f,1.0f))*0.5;
	tex.y = -tex.y;
	float ld = ldep.Sample(TriLinearSam, tex);
	float gd = gdep.Sample(TriLinearSam, tex);
	ld = (gd < ld) ? gd : ld;

	float a = pow((input.PosV.z- ld), Range);
	return float4(Color, 1.0f)*a;
}