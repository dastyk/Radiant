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
cbuffer Lightdata : register(b1)
{
	float3 DirectionVS;
	float CosOuter;
	float CosInner;
	float3 Color;
	float3 PositionVS;
	float RangeRcp;
	float4x4 world;
	int volumetrick;
	float Intensity;
	float2 pad;
};

Texture2D ldep : register(t0);
Texture2D gdep : register(t1);
SamplerState TriLinearSam : register(s0);

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float4 PosV : POSITION0;
	float4 PosL : POSITION1;
	float3 Normal : NORMAL;
};

static const float PI = 3.14159265f;
static const float PI_RCP = 0.318309886;

static const uint NUM_SAMPLES = 16;

static const float TAU = 0.3f;

float CalcScatteringFactor(float theta, float scattAmount, float scattCoeff)
{
	float t = pow(1 - scattCoeff, 2);
	float n = 4 * PI * pow(1 + scattCoeff*scattCoeff - 2 * scattCoeff*cos(theta), 1.5f);
	return (t / n)*scattAmount;
}

float rcp(float v)
{
	return 1 / v;
}

void Raymarch(inout float3 VLI, in float stepSize, inout float3 rayPos, in float3 rayDir, in float3 lightPos)
{
	rayPos += rayDir*stepSize;

	float3 rayToLight = normalize(lightPos - rayPos);
	float theta = dot(rayToLight ,-rayDir);

	// rayPositionLightSS.xyz ray from light to rayPos in the lights viewspace.
	//float3 shadowTerm = getShadowTerm(shadowMapSampler, shadowMapSamplerState, rayPositionLightSS.xyz).xxx;

	float d = length(rayToLight);
	float dRcp = rcp(d);
										//phi scatter probability? // scatter factor from mie scattering?
	//float3 intens = TAU * (shadowTerm * (phi * 0.25 * PI_RCP) * dRcp * dRcp) * exp(-d * TAU) * exp(-l * TAU) * stepSize;
	float phi = CalcScatteringFactor(theta, 25.0f, 0.6f);
	float3 intens = TAU * phi* ((0.25*PI_RCP)*dRcp*dRcp)* exp(-d * TAU)*exp(-1.0f*TAU)*stepSize;

	VLI += intens;
}

struct PS_OUT
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
};
float4 main(VS_OUT input) : SV_TARGET
{
	//float depth = gDepthTex.Sample(gTriLinearSam, uv).r;
	float d3 = ldep.Load(uint3(input.PosH.xy, 0)).r;
	float d2 = gdep.Load(uint3(input.PosH.xy, 0)).r;

	// Get the lenght of the line segment
	float d = max((input.PosL.z),0.0f);
	d = max(min(d3, d2) - d, 0.0f);

	// The the various data
	float3 lightPos = mul(float4(PositionVS,1.0f), View).xyz;
	float3 pos = input.PosV.xyz;
	float3 dir = normalize(pos);
	float stepSize = d / (float)NUM_SAMPLES;
	//float sampleTot = 0.0f;

	// Setup the output
	//PS_OUT output = (PS_OUT)0;
	//output.Color.xyz = Color;

	float3 VLI = float3(0.0f,0.0f,0.0f);

	// Ray march the line segment
	for (uint i = 0; i < NUM_SAMPLES; i++)
	{
	//	pos += dir*stepSize;
		//float3 ptol = normalize(lightPos - pos);
	//	float theta = dot(-dir, ptol);

	//	output.Normal.xyz += -dir*CalcScatteringFactor(theta, 0.01f, 0.1f);
		//sampleTot += CalcScatteringFactor(theta, 0.5f, 0.5f);

		Raymarch(VLI, stepSize, pos, dir, lightPos);


	}

	return float4(Color*VLI, 1.0f);
//	output.Normal.xyz = normalize(output.Normal.xyz);
//	output.Normal.xyz = (output.Normal.xyz + 1.0f) * 0.5f;

	//sampleTot =  (sampleTot/NUM_SAMPLES);
//	return output;
	// Calculate draw distance fog

	//float r = 5.0;
	//float fogFactor = max(DrawDistance - d2 - r, 0.0f) / (DrawDistance - r);

	//return float4(Color, 1.0f)*sampleTot*0.2f;
	//return float4(input.Normal, 1.0f);
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
//	return float4(sampleTot, sampleTot, sampleTot, sampleTot);


}