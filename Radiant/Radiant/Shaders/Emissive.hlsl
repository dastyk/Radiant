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
cbuffer Material : register(b1)
{
	float Roughness = 0.7f;
	float Metallic = 0.0f;
	float ParallaxScaling = 0.0f;
	float ParallaxBias = 0.0f;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D DisplacementMap : register(t2);

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
float3 NormalSampleToWorldSpace(float3 nSample,
	float3 normal,
	float3 tangent)
{
	float3 normalT = 2.0f*nSample - float3(1.0f, 1.0f, 1.0f);

	float3 N = normalize(normal);
	float3 T = normalize(tangent - dot(tangent, N)*N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	return mul(normalT, TBN);
}

PS_OUT PS( VS_OUT input )
{
	PS_OUT output = (PS_OUT)0;

	output.Light.r = input.PosV.z;

	//input.ToEye = normalize( input.ToEye );
	float height = DisplacementMap.Sample( TriLinearSam, input.TexC ).r;
	height = height * ParallaxScaling + ParallaxBias;
	input.TexC -= (height * input.ToEye.xy);

	float r = 5.0;
	float fogFactor = max(max(ViewDistance - input.PosV.z - r, 0.0f) / (ViewDistance - r), 0.5f);

	float r2 = 5.0f;
	float fogFactor2 = max(DrawDistance - input.PosV.z - r2, 0.0f) / (DrawDistance - r2);

	float4 diffuse = DiffuseMap.Sample( TriLinearSam, input.TexC );
	output.Color.rgb = pow( abs( diffuse.rgb ), gamma )*fogFactor*fogFactor2;
	output.Color.a = Roughness;

	input.tbnMatrix[0] = normalize(input.tbnMatrix[0]);
	input.tbnMatrix[1] = normalize(input.tbnMatrix[1]);
	input.tbnMatrix[2] = normalize(input.tbnMatrix[2]);



	// First convert from [0,1] to [-1,1] for normal mapping, and then back to
	// [0,1] when storing in GBuffer.
	float3 normal = NormalMap.Sample(TriLinearSam, input.TexC);
	normal = normal * 2.0f - 1.0f;
	normal = normalize(mul(normal, input.tbnMatrix));
	normal = (normal + 1.0f) * 0.5f;

	output.Normal.rgb = normal;
	output.Normal.a = Metallic;

	//output.Emissive = float4(0.1f, 0.0f, 0.0f, 0.0f) * (height < 0.001f);
	output.Emissive = float4(output.Color.rgb, 0.0f) * (height < 0.001f)*fogFactor*fogFactor2;

	return output;
}
