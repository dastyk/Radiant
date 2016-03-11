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

cbuffer a : register(b1)
{
	float4x4 gWVP;
	float4x4 gWorldViewInvTrp;
	float4x4 gWorld;
	float4x4 gWorldView;
	float4x4 gWorldInvTrp;
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
	float4 PosV : POSITION;
	float3 ToEye : NORMAL0;
	float2 TexC : TEXCOORD;
	float3 Normal : NORMAL1;
	float3 Tangent : TANGENT;
};

VS_OUT VS( VS_IN input )
{
	VS_OUT output = (VS_OUT)0;

	output.PosH = mul( float4(input.PosL, 1.0f), gWVP );
	output.PosV = mul(float4(input.PosL, 1.0f), gWVP);
	output.TexC = input.TexC;

	float3 posW = mul(float4(input.PosL, 1.0f), gWorld).xyz;
	float3 toEye = normalize(posW - CameraPosition);

	float3x3 tbnMatrix;
	tbnMatrix[0] = output.Tangent = mul(float4(input.TangL, 0.0f), gWorld);
	tbnMatrix[1]  = mul(float4(input.BinoL, 0.0f), gWorld);
	tbnMatrix[2] = output.Normal = mul(float4(input.NormL, 0.0f), gWorldInvTrp);
	tbnMatrix = transpose(tbnMatrix);
	output.ToEye = mul(toEye, tbnMatrix);
	
	//output.Tangent = mul(float4(output.Tangent, 0.0f), gWorld);
	//output.Normal = mul(float4(output.Normal, 0.0f), gWorldInvTrp);
	
	//output.tbnMatrix = mul(output.tbnMatrix, gWorldViewInvTrp );//since the normal read from the normalmap will be multiplied with both the tbnMatrix and the wordviewinvtrp we can do it here instead.
	
	return output;
}