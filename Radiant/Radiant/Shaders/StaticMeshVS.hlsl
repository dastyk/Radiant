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
	float3 BinoL : BINORMAL;
	float2 TexC : TEXCOORD;
};

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
	float3x3 tbnMatrix : TBNMATRIX;
};

VS_OUT VS( VS_IN input )
{
	VS_OUT output = (VS_OUT)0;

	output.PosH = mul( float4(input.PosL, 1.0f), gWVP );
	output.TexC = input.TexC;
	//output.NormVS = mul( float4(input.NormL, 0.0f), gWorldViewInvTrp ).xyz;
	//float nor = mul(float4(input.NormL, 0.0f), gWorldViewInvTrp ).xyz;
	//float tan = mul(float4(input.TangL, 0.0f), gWorldViewInvTrp).xyz;
	//float bin = mul(float4(input.BinoL, 0.0f), gWorldViewInvTrp).xyz;

	output.tbnMatrix[0] = input.TangL;
	output.tbnMatrix[1] = input.BinoL;
	output.tbnMatrix[2] = input.NormL;

	//output.tbnMatrix = transpose(output.tbnMatrix);
	
	//output.tbnMatrix = mul(gWorldViewInvTrp, output.tbnMatrix);
	output.tbnMatrix = mul(output.tbnMatrix, gWorldViewInvTrp );
	//output.tbnMatrix = transpose(output.tbnMatrix);
	return output;
}