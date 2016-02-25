struct VS_IN
{
	float3 position : POSITION;
	uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 PosT : POSITION;
	uint instanceID : INSTANCEID;
};

cbuffer DecalsVSPerObjectBuffer : register(b0)
{
	float4x4 gWorldViewProj[256];
}

VS_OUT VS(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.position = mul(float4(input.position, 1.0f), gWorldViewProj[input.instanceID]);
	output.PosT = mul(float4(input.position, 1.0f), gWorldViewProj[input.instanceID]);
	output.instanceID = input.instanceID;
	return output;
}