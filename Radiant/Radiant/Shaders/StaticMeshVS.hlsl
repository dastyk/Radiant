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
	float3 TangL : TANGENT;
	float3 BinoL : BINORMAL;
	float2 TexC : TEXCOORD;
};

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float3 ToEye : NORMAL;
	float2 TexC : TEXCOORD;
	float3x3 tbnMatrix : TBNMATRIX;
};

VS_OUT VS( VS_IN input )
{
	VS_OUT output = (VS_OUT)0;

	output.PosH = mul( float4(input.PosL, 1.0f), gWVP );
	output.TexC = input.TexC;

	float3 posW = mul(float4(input.PosL, 1.0f), gWorld).xyz;
	float3 toEye = normalize(gCamPos.xyz - posW);

	output.tbnMatrix[0] = input.TangL;
	output.tbnMatrix[1] = input.BinoL;
	output.tbnMatrix[2] = input.NormL;
	
	output.ToEye = mul(toEye, transpose(output.tbnMatrix));
	output.tbnMatrix = mul(output.tbnMatrix, gWorldViewInvTrp );
	
	return output;
}