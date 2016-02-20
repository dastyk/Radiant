struct VS_IN
{
	float3 position : POSITION;
	float3 normal : NORMAL;//Not used but cant be arsed to make a new input layout
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 PosT : POSITION;
	float3 Normal : NORMAL;
};

cbuffer DecalsVSPerObjectBuffer : register(b0)
{
	float4x4 gWorldViewProj;
}

VS_OUT VS(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.position = mul(float4(input.position, 1.0f), gWorldViewProj);
	output.PosT = mul(float4(input.position, 1.0f), gWorldViewProj);
	output.Normal = input.normal;
	return output;
}