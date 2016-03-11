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

cbuffer cbTexelSize : register(b1)
{
	float gTexelWidth;
	float gTexelHeight;
};

float GaussDist( float offset, float stdDev )
{
	// mean = 0
	return (1.0f / (stdDev * sqrt( 2 * 3.141592f ))) * exp( -offset * offset / (2 * stdDev * stdDev) );
}

static const int gBlurRadius = 9;
static const float gBroadStdDev = 6.0f;
static const float gSharpStdDev = 3.0f;
static const float gBroadGauss[gBlurRadius] = { GaussDist( 1.0f, gBroadStdDev ), GaussDist( 2.0f, gBroadStdDev ), GaussDist( 3.0f, gBroadStdDev ), GaussDist( 4.0f, gBroadStdDev ), GaussDist( 5.0f, gBroadStdDev ), GaussDist( 6.0f, gBroadStdDev ), GaussDist( 7.0f, gBroadStdDev ), GaussDist( 8.0f, gBroadStdDev ), GaussDist( 9.0f, gBroadStdDev ) };
static const float gSharpGauss[gBlurRadius] = { GaussDist( 1.0f, gSharpStdDev ), GaussDist( 2.0f, gSharpStdDev ), GaussDist( 3.0f, gSharpStdDev ), GaussDist( 4.0f, gSharpStdDev ), GaussDist( 5.0f, gSharpStdDev ), GaussDist( 6.0f, gSharpStdDev ), GaussDist( 7.0f, gSharpStdDev ), GaussDist( 8.0f, gSharpStdDev ), GaussDist( 9.0f, gSharpStdDev ) };
static const float gBroadGaussMean = GaussDist( 0.0f, gBroadStdDev );
static const float gSharpGaussMean = GaussDist( 0.0f, gSharpStdDev );
static const float gBroadGaussNormalizer = 1.0f / (gBroadGaussMean + 2.0f * (gBroadGauss[0] + gBroadGauss[1] + gBroadGauss[2] + gBroadGauss[3] + gBroadGauss[4] + gBroadGauss[5] + gBroadGauss[6] + gBroadGauss[7] + gBroadGauss[8]));
static const float gSharpGaussNormalizer = 1.0f / (gSharpGaussMean + 2.0f * (gSharpGauss[0] + gSharpGauss[1] + gSharpGauss[2] + gSharpGauss[3] + gSharpGauss[4] + gSharpGauss[5] + gSharpGauss[6] + gSharpGauss[7] + gSharpGauss[8]));

Texture2D gImageToBlur : register(t0);

SamplerState gTrilinearSam : register(s0);

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

float4 PS(VS_OUT input) : SV_TARGET
{
#if HORIZONTAL_BLUR
	float2 texOffset = float2(gTexelWidth, 0.0f);
#else
	float2 texOffset = float2(0.0f, gTexelHeight);
#endif

	// Center value always contributes to the sum
	float4 center = float4(gImageToBlur.Sample( gTrilinearSam, input.TexC ).xyz, 0.0f);
	float4 color = gBroadGaussNormalizer * gBroadGaussMean * center;
	color += gSharpGaussNormalizer * gSharpGaussMean * center;

	// Loop through pixels in one end of the line to the other.
	for ( float i = 0; i < gBlurRadius; ++i )
	{
		// Add neighbor pixels to blur.
		float4 left = float4(gImageToBlur.Sample(gTrilinearSam, input.TexC + i * texOffset).xyz, 0.0f);
		float4 right = float4(gImageToBlur.Sample( gTrilinearSam, input.TexC - i * texOffset ).xyz, 0.0f);
		color += gBroadGaussNormalizer * gBroadGauss[i] * (left + right);
		color += gSharpGaussNormalizer * gSharpGauss[i] * left + gSharpGaussNormalizer * gSharpGauss[i] * right;
	}

	return color;
}