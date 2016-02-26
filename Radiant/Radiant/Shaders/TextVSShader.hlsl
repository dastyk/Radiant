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
struct VS_IN
{
	float3 PosL : POSITION;
	float2 TexC : TEXCOORD;
};

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.PosH = mul(float4(input.PosL, 1.0f), Ortho);
	output.PosH = float4(output.PosH.x - 1.0f, output.PosH.y + 1.0f, output.PosH.zw);
	output.TexC = input.TexC;
	return output;
}