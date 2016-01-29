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
	float3 NormVS : NORMAL;
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
	//output.Color = float4(1.0f, 0.0f, 0.0f, Roughness); // remove this

	// Transform normal from [-1,1] to [0,1] because RT store in [0,1] domain.
	//output.Normal.rgb = 0.5f * (normalize( input.NormVS ) + 1.0f);
	output.Normal.rgb = NormalMap.Sample( TriLinearSam, input.TexC ).xyz;
	output.Normal.a = Metallic;
	output.Normal.a = 1.0f;

	return output;
}