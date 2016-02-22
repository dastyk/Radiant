
cbuffer Material : register(b0)
{
	float Roughness;
	float Metallic;
	float ParallaxBias = 0.0f;
	float ParallaxScaling = 0.0f;
	float TexCoordScaleU = 1.0f;
	float TexCoordScaleV = 1.0f;
	float EmissiveIntensity = 1.0f;
};

cbuffer DecalsPSConstantBuffer : register(b1)
{
	float4x4 gInvViewProj;
	float4x4 gView;//Used for constructing viewspace normal
};

cbuffer DecalsPSPerObjectBuffer : register(b2)
{
	float4x4 gInvWorld;
}



SamplerState gTriLinearSam : register(s0);

Texture2D<float4> gDepthTex : register(t0);//from gbuffer
Texture2D<float4> gColor : register(t1); //Not the one in the gbuffer, this is the texture of the decal
Texture2D<float4> gNormal : register(t2); //Likewise
Texture2D<float4> gEmissive : register(t3);//Likewise

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 PosT : POSITION;
};

struct PS_OUT
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Emissive : SV_TARGET2;
};

float3x3 cotangentFrame(float3 pixelNormal, float3 worldPos, float2 decalUV)
{
	float3 dp1 = ddx(worldPos);
	float3 dp2 = ddy(worldPos);
	float2 duv1 = ddx(decalUV);
	float2 duv2 = ddy(decalUV);

	float3 dp2perp = cross(dp2, pixelNormal);
	float3 dp1perp = cross(pixelNormal, dp1);
	float3 tangent = dp2perp * duv1.x + dp1perp * duv2.x;
	float3 bitangent = dp2perp * duv1.y + dp1perp * duv2.y;

	float invmax = rsqrt(max(dot(tangent, tangent), dot(bitangent, bitangent)));
	return float3x3(tangent * invmax, bitangent * invmax, pixelNormal);
}

PS_OUT PS(VS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	//Get UVs to sample depth
	//input.PosT.xyz /= input.PosT.w;
	input.PosT.xyz /= input.PosT.w;
	//float depth = gDepthTex.Sample(gTriLinearSam, uv).r;
	float depth = gDepthTex.Load(uint3(input.Pos.xy, 0)).r;
	//Get world pos by multiplying with invViewproj
	float4 worldPos = mul(float4(input.PosT.x, input.PosT.y, depth, 1.0f), gInvViewProj);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1.0f;


	//Transform worldPos into Decals local space
	float4 localPosition = mul(worldPos, gInvWorld);
	clip(0.5f - abs(localPosition.xyz)); //If it is outside the box's local space we do nothing
	float2 decalUV = localPosition.xy + 0.5f;
	decalUV.y = 1.0f - decalUV.y;

	float3 pixelNormal = normalize(cross(ddy(worldPos), ddx(worldPos))).xyz;
	float3x3 tbnMatrix = cotangentFrame(pixelNormal, worldPos.xyz, decalUV);
	

	output.Color = gColor.Sample(gTriLinearSam, decalUV);
	clip(output.Color.a - 0.05f);
	output.Color.a = Roughness;
	float3 normal = gNormal.Sample(gTriLinearSam, decalUV).rgb;
	normal = normal * 2.0f - 1.0f;
	normal = -normal;
	normal = mul(tbnMatrix, normal);
	normal = mul(float4(normal, 0.0f), gView).xyz;
	normal = 0.5f * (normal + 1.0f);
	
	output.Normal.rgb = normal;
	//output.Normal.rgb = gNormal.Sample(gTriLinearSam, decalUV).rgb;
	output.Normal.a = Metallic;
	output.Emissive = gEmissive.Sample(gTriLinearSam, decalUV);
	output.Emissive *= EmissiveIntensity;
	
	//output.Color = float4(decalUV.x, decalUV.y, 0.0f, 1.0f);
	//output.Color = gColor.Sample(gTriLinearSam, decalUV);
	//output.Color = float4(uv.x, 0.0f, uv.y, 1.0f);
	//output.Color = float4(depth, depth, depth, 1.0f);
	//output.Color = float4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
	//output.Color = float4(localPosition.xyz, 1.0f);
	//output.Color.rgb = input.Normal;

	return output;
}