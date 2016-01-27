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

float4 PSSingleChannel( float4 posH : SV_POSITION, float2 texC : TEXCOORD ) : SV_TARGET
{
	return gTexture.Sample( gTriLinearSam, texC ).rrrr;
}