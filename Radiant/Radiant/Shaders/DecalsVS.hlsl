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
}
cbuffer DecalsVSPerObjectBuffer : register(b1)
{
	float4x4 gWorldViewProj[256];
	int multBy; //Because drawindexedinstanced is fucking retarded and SV_InstanceID does not start with the last argument of the drawindexedinstanced call
	int pad1;
	int pad2;
	int pad3;
}

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


VS_OUT VS(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.position = mul(float4(input.position, 1.0f), gWorldViewProj[input.instanceID * multBy]);
	output.PosT = mul(float4(input.position, 1.0f), gWorldViewProj[input.instanceID * multBy]);
	output.instanceID = input.instanceID * multBy;
	return output;
}