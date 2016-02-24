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
	float gBackbufferWidth;
	float gBackbufferHeight;
}
cbuffer DecalsVSPerObjectBuffer : register(b1)
{
	float4x4 gWorldViewProj;
}

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


VS_OUT VS(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.position = mul(float4(input.position, 1.0f), gWorldViewProj);
	output.PosT = mul(float4(input.position, 1.0f), gWorldViewProj);
	output.Normal = input.normal;
	return output;
}