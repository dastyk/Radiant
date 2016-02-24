cbuffer Material : register( b0 )
{
	float Roughness = 0.7f;
	float Metallic = 0.0f;
	float ParallaxScaling = 0.0f;
	float ParallaxBias = 0.0f;
	float TexCoordScaleU = 1.0f;
	float TexCoordScaleV = 1.0f;
	float EmissiveIntensity = 1.0f;
	float BlurIntensity = 1.0f;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D DisplacementMap : register(t2);
Texture2D Emissive : register(t3);

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
	float4 Emissive : SV_TARGET2;
	float Light : SV_TARGET3;
};

PS_OUT PS( VS_OUT input )
{
	PS_OUT output = (PS_OUT)0;

	output.Light.r = input.PosH.z/input.PosH.w;

	input.TexC.x *= TexCoordScaleU;
	input.TexC.y *= TexCoordScaleV;

	output.Emissive = float4(Emissive.Sample(TriLinearSam, input.TexC).xyz, BlurIntensity)*EmissiveIntensity;

	input.ToEye = normalize(input.ToEye);
	float height = DisplacementMap.Sample(TriLinearSam, input.TexC).r;
	height = height * ParallaxScaling + ParallaxBias;
	input.TexC += (height * input.ToEye.xy);

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