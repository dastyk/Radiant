cbuffer a : register(b0)
{
	float4x4 gWVP;
	float4x4 gWorldViewInvTrp;
};

struct VS_IN
{
	float3 PosL : POSITION;
	
	float3 NormL : NORMAL;
	float3 TangL : TANGENT;
	float3 binoL : BINORMAL;
	float2 TexC : TEXCOORD;
};

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
	float3 NormVS : NORMAL;
};

VS_OUT VS( VS_IN input )
{
	VS_OUT output = (VS_OUT)0;

	output.PosH = mul( float4(input.PosL, 1.0f), gWVP );
	output.TexC = input.TexC;
	output.NormVS = mul( float4(input.NormL, 0.0f), gWorldViewInvTrp ).xyz;

	return output;
}