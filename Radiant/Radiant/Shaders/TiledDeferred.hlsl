struct PointLight
{
	float3 PositionVS;
	float Range;
	float3 Color;
	float Intensity;
};

// Inputs
Texture2D<float4> gColorMap : register(t0);
Texture2D<float4> gNormalMap : register(t1);
Texture2D<float4> gDepthMap : register(t2);
StructuredBuffer<PointLight> gPointLights : register(t4);

// Output
RWTexture2D<float4> gOutputTexture : register(u0); // Fully composited and lit HDR texture (actually not HDR in this project)

// Constants
cbuffer Constants : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gInvView;
	float4x4 gInvProj;
	float gBackbufferWidth;
	float gBackbufferHeight;
	int gPointLightCount;
	int gSpotLightCount;
	int gCapsuleLightCount;
};

SamplerState gSamTriLinear : register(s0);

struct GBuffer
{
	float3 Diffuse;
	float3 Normal;
	float3 PosVS;
	float Roughness;
	float Metallic;
};

groupshared uint minDepth;
groupshared uint maxDepth;

groupshared uint visiblePointLightCount;
groupshared uint visiblePointLightIndices[1024];

void EvaluatePointLight( PointLight light, GBuffer gbuffer, out float3 radiance, out float3 l )
{
	// Light data is in world space; transform it to view space.
	light.PositionVS = mul( float4(light.PositionVS, 1.0f), gView ).xyz;

	// Surface-to-light vector
	l = light.PositionVS - gbuffer.PosVS;

	// Compute attenuation based on distance - linear attenuation
	float distToLight = length( l );
	float attenuation = saturate( 1.0f - distToLight / light.Range );

	l /= distToLight; // Normalize

	radiance = light.Color * light.Intensity * attenuation;
}

bool IntersectPointLightTile( PointLight light, float4 frustumPlanes[6] )
{
	bool inFrustum = true;
	[unroll]
	for ( uint i = 0; i < 6; ++i )
	{
		// Light data is in world space; transform it to view space.
		float dist = dot( frustumPlanes[i], mul( float4(light.PositionVS, 1.0f), gView ) );
		inFrustum = inFrustum && (-light.Range <= dist);
	}

	return inFrustum;
}

float3 Fr_Schlick( float3 F0, float F90, float cosIncident )
{
	return F0 + (F90 - F0) * pow( 1.0f - cosIncident, 5.0f );
}

// Se "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
// av Eric Heitz. Den höjdkorrigerade Smith beskrivs i avsnitt 6 (ekv. 99)
// (icke-korrigerad Smith är ekv. 43)
// OmegaM är microfacet normal (halvvektorn), omegaO är utgående (view) och
// omegaI är ingående (ljus). Halvvektorn är mellan dessa, så skalärprodukten
// är positiv (eftersom vinkeln mellan l och v är < 90) och därmed ger chiPlus
// värdet 1 båda gångerna.
// Lambda är en funktion som beror på normalfördelningen. Ekv. 72 ger Lambda
// för GGX (ser lite underlig ut, men notera på raden under vad a är (det är a,
// inte alpha som jag trodde först), så ser man
// att den beror på theta0, som i sin tur beror på inparametern. Sätt in det
// uttrycket, kör tan2 = sin2/cos2 på resulterande, följt av trigettan på sin2
// så får man resultatet nedan :)
float Vis_SmithGGXHeightCorrelated( float NdL, float NdV, float alpha )
{
	//float alphaSq = alpha * alpha;
	//float NdVSq = NdV * NdV;
	//float NdLSq = NdL * NdL;
	//float lambdaV = 0.5f * (-1.0f + sqrt( 1.0f + alphaSq * (1.0f - NdVSq) / NdVSq ));
	//float lambdaL = 0.5f * (-1.0f + sqrt( 1.0f + alphaSq * (1.0f - NdLSq) / NdLSq ));
	//return 1.0f / (1.0f + lambdaV + lambdaL) / (4.0f * NdL * NdV + 1e-3f);

	// Optimized.
	// För min egen skull: härledning (tror division med 4NdLNdV ingår)
	float alphaSq = alpha * alpha;
	float lambdaV = NdL * sqrt( (-NdV * alphaSq + NdV) * NdV + alphaSq );
	float lambdaL = NdV * sqrt( (-NdL * alphaSq + NdL) * NdL + alphaSq );
	return 0.5f / (lambdaV + lambdaL + 1e-5f);
}

// Anisotropic variant is available in Heitz (SIGGRAPH'14) course notes eq. 85
// (with lambda for Smith in eq. 86)
// Normaliserad GGX kan ses i "Physics and Math of Shading" av Naty Hoffman
// (ekv. 18) i en förenklad form. Det är den som används nedan.
// För min egen skull: Kolla så jag kan gå från ekv. 71 i Heitz till ekv. 18
// i Hoffman genom att ändra tan2 till sin2/cos2 och sedan köra trigettan på sin2.
float NDF_GGX( float NdH, float m )
{
	float mSq = m * m;
	float x = (NdH * mSq - NdH) * NdH + 1.0f;
	return mSq / (3.141592f * x * x + 1e-5f);
}

float3 BRDF( float3 l, float3 v, GBuffer gbuffer )
{
	// Half vector
	float3 h = normalize( l + v );

	float NdL = saturate( dot( gbuffer.Normal, l ) );
	float NdH = saturate( dot( gbuffer.Normal, h ) );
	float NdV = abs( dot( gbuffer.Normal, v ) ) + 1e-5f; // Epsilon is to avoid artifact. By why abs? Is it faster than saturate?
	float LdH = saturate( dot( l, h ) );

	float roughness = gbuffer.Roughness;
	float linearRoughness = roughness * roughness;
	float3 albedo = gbuffer.Diffuse * (1.0f - gbuffer.Metallic); // No diffuse if metal

	/*
	Note: Reflectance at normal incidence hardcoded for dielectrics (4%). This
	works for most materials, but not correct in the physical sense	for gemstones
	and diamond. Since metals don't have subsurface scattering they don't have
	any diffuse color, but they do have colored specular tint. For that reason
	the diffuse channel of the gbuffer doubles at Fresnel reflectance at normal
	incidence for metals.
	*/
	float3 F0 = lerp( 0.04f, gbuffer.Diffuse, gbuffer.Metallic );

	// Specular microfacet model
	float3 F = Fr_Schlick( F0, 1.0f, LdH );
	float Vis = Vis_SmithGGXHeightCorrelated( NdL, NdV, roughness );
	float D = NDF_GGX( NdH, roughness );
	float3 spec = F * Vis * D;

	// Diffuse (Disney)
	// The Disney diffuse model is an empirical model based on real world surface
	// observations. It takes into account roughness of the material and provides
	// some retro-reflection at grazing angles. The Disney model is not energy
	// conserving (by design, actually). Therefore, it needs to be renormalized.
	//float energyBias = lerp( 0.0f, 0.5f, linearRoughness );
	//float energyFactor = lerp( 1.0f, 1.0f / 1.51f, linearRoughness );
	//float F90 = energyBias + 2.0f * LdH * LdH * linearRoughness;
	//F0 = float3(1.0f, 1.0f, 1.0f);
	//float3 diff = albedo / 3.141592f * Fr_Schlick( F0, F90, NdL ).r * Fr_Schlick( F0, F90, NdV ).r * energyFactor;

	/*
	Qualitative Oren-Nayar, simplified as per "Beyond a Simple Physically
	Based Blinn-Phong Model in Real-Time" from the SIGGRAPH'12 PBR course.
	Oren-Nayar is a diffuse only model based on micro-facet theory, and
	introduces inter-reflection. The roughness is actually a standard
	deviation of angle in radians, and not in [0,1]. According to O-N, a
	roughness value of 0.52 (30 deg) is a very rough surface, so there's
	really no need to go all the way to PI/2. Therefore I use the roughness
	in [0,1] right away. It's possible to remap [0,1] on [0,0.60] or
	something, if need be.
	*/
	float A = 1.0f - 0.5f * linearRoughness / (linearRoughness + 0.57f);
	float B = 0.45f * linearRoughness / (linearRoughness + 0.09f);
	float LdV = saturate( dot( l, v ) );
	float3 diff = (albedo / 3.141592f) * (A + B * saturate( LdV - NdV * NdL ) / max( NdL, NdV ));

	return diff + spec;
}

// One pixel per thread, 16x16 thread groups (= 1 tile).
#define BLOCK_SIZE 16
[numthreads( BLOCK_SIZE, BLOCK_SIZE, 1 )]
void CS( uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID,
	uint groupIndex : SV_GroupIndex, uint3 dispatchThreadID : SV_DispatchThreadID )
{
	// Step 1 - Load gbuffers and depth
	GBuffer gbuffer;

	float depth = gDepthMap.Load( uint3(dispatchThreadID.xy, 0) ).r;
	float4 diffuse_roughness = gColorMap.Load( uint3(dispatchThreadID.xy, 0) );
	float4 normal_metallic = gNormalMap.Load( uint3(dispatchThreadID.xy, 0) );

	// Reconstruct view space position from depth
	float x = (dispatchThreadID.x / gBackbufferWidth) * 2 - 1;
	float y = (1 - dispatchThreadID.y / gBackbufferHeight) * 2 - 1;
	float4 posVS = mul( float4(x, y, depth, 1), gInvProj );
	posVS /= posVS.w;

	gbuffer.PosVS = posVS.xyz;
	gbuffer.Diffuse = diffuse_roughness.rgb;
	gbuffer.Normal = normalize( 2.0f * normal_metallic.xyz - 1.0f );
	gbuffer.Roughness = diffuse_roughness.a;
	gbuffer.Metallic = normal_metallic.a;

	// Initialize group shared memory
	if ( groupIndex == 0 )
	{
		visiblePointLightCount = 0;
		minDepth = 0xFFFFFFFF;
		maxDepth = 0;
	}

	GroupMemoryBarrierWithGroupSync();

	// Step 2 - Calculate min and max z in threadgroup / tile
	float linearDepth = gProj[3][2] / (depth - gProj[2][2]);
	uint depthInt = asuint( linearDepth );

	// Only works on ints, but we can cast to int because z is always positive
	// Set group minimum
	InterlockedMin( minDepth, depthInt );
	InterlockedMax( maxDepth, depthInt );

	GroupMemoryBarrierWithGroupSync();

	// When group min/max have been calculated, we use those values
	float minGroupDepth = asfloat( minDepth );
	float maxGroupDepth = asfloat( maxDepth );

	// Step 3 - Cull light sources against tile frustum

	// Calculate tile frustum. Tile min and max z is used as a "depth bounds" test.
	// Extrahering av plan kan ses i RTR (s.774), där skillnaden är att kolumner
	// används istället för rader eftersom DX är vänsterorienterat. Här bryr vi
	// oss inte om att negera planen eftersom vi vill att de ska peka in i frustum.
	// Enbart projektionsmatrisen används eftersom vi då kommer arbeta i view space.
	// Vidare är de använda kolumnerna modifierade för rutans frustum.
	// TODO: Jag har inte hittat härledning till varför man kan modifiera projektions-
	// matris så här. Also: Varför 2 * BLOCK_SIZE? Det borde framstå ur härledning...
	float4 frustumPlanes[6];
	float2 tileScale = float2(gBackbufferWidth, gBackbufferHeight) * rcp( float( 2 * BLOCK_SIZE ) );
	float2 tileBias = tileScale - float2(groupID.xy);
	float4 col1 = float4(gProj._11 * tileScale.x, 0.0f, tileBias.x, 0.0f);
	float4 col2 = float4(0.0f, -gProj._22 * tileScale.y, tileBias.y, 0.0f);
	float4 col4 = float4(0.0f, 0.0f, 1.0f, 0.0f);
	frustumPlanes[0] = col4 + col1; // Left plane
	frustumPlanes[1] = col4 - col1; // Right plane
	frustumPlanes[2] = col4 - col2; // Top plane
	frustumPlanes[3] = col4 + col2; // Bottom plane
									// Remember: First three components are the plane normal (a,b,c). The last
									// component w = -(ax + by + cz) where (x,y,z) is a point on the plane.
	frustumPlanes[4] = float4(0.0f, 0.0f, 1.0f, -minGroupDepth); // Near plane
	frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f, maxGroupDepth); // Far plane
	[unroll]
	for ( int i = 0; i < 4; ++i )
	{
		// Normalize planes (near and far already normalized)
		frustumPlanes[i] *= rcp( length( frustumPlanes[i].xyz ) );
	}

	// Cull lights against computed frustum. We use multiple passes when we have
	// more light than threads. The indices of the visible lights are appended to
	// a list. An atomic add keeps track of the how many lights are visible.
	uint threadCount = BLOCK_SIZE * BLOCK_SIZE;
	uint passCount;
	uint passIt;

	// Point lights
	passCount = (gPointLightCount + threadCount - 1) / threadCount;
	for ( passIt = 0; passIt < passCount; ++passIt )
	{
		uint lightIndex = passIt * threadCount + groupIndex;

		// Prevent overrun by clamping to a last "null" light
		lightIndex = min( lightIndex, gPointLightCount );

		PointLight light = gPointLights[lightIndex];

		if ( IntersectPointLightTile( light, frustumPlanes ) )
		{
			uint offset;
			InterlockedAdd( visiblePointLightCount, 1, offset );
			visiblePointLightIndices[offset] = lightIndex;
		}
	}

	GroupMemoryBarrierWithGroupSync();

	// Step 4 - Switching back to processing pixels. Accumulate lighting from the
	// visible lights calculated earlier and combine with color data.

	float2 uv = dispatchThreadID.xy / float2(gBackbufferWidth, gBackbufferHeight);
	float3 color;

	color = 0.1f * gbuffer.Diffuse; // Ambient

	// View vector (camera position is origin because of view space :) )
	float3 v = normalize( -gbuffer.PosVS );

	float3 radiance;
	float3 l;
	float NdL;
	uint lightIt;

	// Point lights
	for ( lightIt = 0; lightIt < visiblePointLightCount; ++lightIt )
	{
		uint lightIndex = visiblePointLightIndices[lightIt];
		PointLight light = gPointLights[lightIndex];

		EvaluatePointLight( light, gbuffer, radiance, l );
		NdL = saturate( dot( gbuffer.Normal, l ) );

		color += BRDF( l, v, gbuffer ) * radiance * NdL;
	}

	gOutputTexture[dispatchThreadID.xy] = float4(color, 1);
}