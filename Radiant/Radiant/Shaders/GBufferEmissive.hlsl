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
	float gamma;
}

cbuffer Material : register( b1 )
{
//	float Roughness = 0.7f;
	//float Metallic = 0.0f;
	float ParallaxScaling = 0.0f;
	float ParallaxBias = 0.0f;
	float TexCoordScaleU = 1.0f;

	float TexCoordScaleV = 1.0f;
	float EmissiveIntensity = 1.0f;
	float BlurIntensity = 1.0f;
	float3 EmissiveColor = float3(1.0f, 1.0f, 1.0f);
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D DisplacementMap : register(t2);
Texture2D Roughness : register(t3);
Texture2D Glossiness : register(t4);
Texture2D Emissive : register(t5);

SamplerState TriLinearSam : register(s0);

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float4 PosV : POSITION;
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

	output.Light.r = input.PosV.z ;

	input.TexC.x *= TexCoordScaleU;
	input.TexC.y *= TexCoordScaleV;

	float r = 5.0;
	float fogFactor = max(max(ViewDistance - input.PosV.z - r, 0.0f) / (ViewDistance - r), 0.1);

	float r2 = 5.0f;
	float fogFactor2 = max(DrawDistance - input.PosV.z - r2, 0.0f) / (DrawDistance - r2);

	input.ToEye = normalize(input.ToEye);
	float height = DisplacementMap.Sample(TriLinearSam, input.TexC).r;
	height = height * ParallaxScaling + ParallaxBias;
	input.TexC -= (height * input.ToEye.xy);

	output.Emissive = float4(Emissive.Sample(TriLinearSam, input.TexC).xyz*EmissiveColor, BlurIntensity)*EmissiveIntensity*fogFactor*fogFactor2;


	float4 diffuse = DiffuseMap.Sample( TriLinearSam, input.TexC );
	output.Color.rgb = pow( abs( diffuse.rgb ), gamma );
	output.Color.a = Roughness.Sample(TriLinearSam, input.TexC).r;

	input.tbnMatrix[0] = normalize( input.tbnMatrix[0] );
	input.tbnMatrix[1] = normalize( input.tbnMatrix[1] );
	input.tbnMatrix[2] = normalize( input.tbnMatrix[2] );

	// First convert from [0,1] to [-1,1] for normal mapping, and then back to
	// [0,1] when storing in GBuffer.
	float3 normal = NormalMap.Sample(TriLinearSam, input.TexC).xyz;
	normal = normal * 2.0f - 1.0f;
	normal = normalize( mul( normal, input.tbnMatrix ) );
	normal = (normal + 1.0f) * 0.5f;

	output.Normal.rgb = normal;
	output.Normal.a = Glossiness.Sample(TriLinearSam, input.TexC).r;

	return output;
}