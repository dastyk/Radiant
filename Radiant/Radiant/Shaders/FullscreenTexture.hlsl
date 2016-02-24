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

void VS( uint VertexID : SV_VertexID, out float4 oPosH : SV_POSITION, out float2 oTexC : TEXCOORD )
{
	oPosH.x = (VertexID == 2) ? 3.0f : -1.0f;
	oPosH.y = (VertexID == 0) ? -3.0f : 1.0f;
	oPosH.zw = 1.0f;

	oTexC = oPosH.xy * float2(0.5f, -0.5f) + 0.5f;
}

Texture2D gTexture : register(t0);
SamplerState gTriLinearSam : register(s0);

float4 PSMultiChannel( float4 posH : SV_POSITION, float2 texC : TEXCOORD ) : SV_TARGET
{
	return gTexture.Sample( gTriLinearSam, texC ).rgba;
}

float4 PSMultiChannelGamma(float4 posH : SV_POSITION, float2 texC : TEXCOORD) : SV_TARGET
{
	const float gamma = 2.2f;
	float4 color = gTexture.Sample( gTriLinearSam, texC );

	return float4(pow( abs( color.rgb ), gamma ), 1.0f);
}

float4 PSSingleChannel( float4 posH : SV_POSITION, float2 texC : TEXCOORD ) : SV_TARGET
{
	return gTexture.Sample( gTriLinearSam, texC ).rrrr;
}