cbuffer a : register(b0)
{
	float4x4 gWVP;
	float4x4 gWorldViewInvTrp;
};

struct VS_IN
{
	float3 PosL : POSITION;
	float3 NormL : NORMAL;
};

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.PosH = mul(float4(input.PosL, 1.0f), gWVP);
	output.Normal = mul(float4(input.NormL, 1.0f), gWorldViewInvTrp).xyz;

	return output;
}
