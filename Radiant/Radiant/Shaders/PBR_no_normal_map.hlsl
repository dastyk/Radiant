cbuffer Material : register( b0 )
{
	float Roughness = 0.7f;
	float Metallic = 0.0f;
};

Texture2D DiffuseMap : register(t0);
SamplerState TriLinearSam : register(s0);

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float3 ToEye : NORMAL;
	float2 TexC : TEXCOORD;
	float3x3 tbnMatrix : TBNMATRIX;
};

struct PS_OUT
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
};

PS_OUT PS( VS_OUT input )
{
	PS_OUT output = (PS_OUT)0;

	float4 diffuse = DiffuseMap.Sample( TriLinearSam, input.TexC );
	float gamma = 2.2f;
	output.Color.rgb = pow( abs( diffuse.rgb ), gamma );
	output.Color.a = Roughness;

	// Convert from [-1,1] to [0,1] when storing in GBuffer.
	output.Normal.rgb = (normalize( input.tbnMatrix[2] ) + 1.0f) * 0.5f;
	output.Normal.a = Metallic;	

	return output;
}