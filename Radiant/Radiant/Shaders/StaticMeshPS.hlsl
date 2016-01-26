
struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
	float3 NormVS : NORMAL;
};

float4 PS( VS_OUT input ) : SV_TARGET
{
	return float4(1,0,0,1);
}