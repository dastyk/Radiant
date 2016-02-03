cbuffer Material : register( b0 )
{
	float Roughness = 0.7f;
	float Metallic = 0.0f;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
SamplerState TriLinearSam : register(s0);

struct VS_OUT
{
	float4 PosH : SV_POSITION;
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

	// First convert from [0,1] to [-1,1] for normal mapping, and then back to
	// [0,1] when storing in GBuffer.
	float3 normal = NormalMap.Sample(TriLinearSam, input.TexC).xyz;
	normal = normal * 2.0f - 1.0f;
	normal = normalize( mul( normal, input.tbnMatrix ) );
	normal = (normal + 1.0f) * 0.5f;

	output.Normal.rgb = normal;
	output.Normal.a = Metallic;	

	return output;
}