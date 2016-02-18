cbuffer a : register(b0)
{
	float4x4 gWVP;
	float4x4 gWorldViewInvTrp;
	float4x4 gWorld;
	float4 gCamPos; //Used for parallax occlusion mapping
	float4x4 gWorldView;
};

struct VS_IN
{
	float3 PosL : POSITION;
	float3 NormL : NORMAL;
};

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float4 PosV : POSITION0;
//	float4 PosL : POSITION1;
	float3 Normal : NORMAL;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.PosH = mul(float4(input.PosL, 1.0f), gWVP);
//	output.PosL = mul(float4(input.PosL, 1.0f), gWVP);
	output.PosV = mul(float4(input.PosL, 1.0f), gWorldView);
	output.Normal = mul(float4(input.NormL, 0.0f), gWorldViewInvTrp).xyz;

	return output;
}
