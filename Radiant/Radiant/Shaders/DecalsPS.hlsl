
cbuffer Material : register(b0)
{
	float Roughness;
	float Metallic;
	float ParallaxBias;
	float ParallaxScaling;
};

cbuffer DecalsPSConstantBuffer : register(b1)
{
	float4x4 gInvViewProj;
	float2 gHalfPixelOffset;
	float pad;
	float pad2;
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
	float3 Normal : NORMAL;
};

struct PS_OUT
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Emissive : SV_TARGET2;
};

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

	output.Color = gColor.Sample(gTriLinearSam, decalUV);
	output.Color.a = Metallic;
	output.Normal = gNormal.Sample(gTriLinearSam, decalUV);
	output.Normal.a = Roughness;
	output.Emissive = gEmissive.Sample(gTriLinearSam, decalUV);
	
	//output.Color = float4(decalUV.x, decalUV.y, 0.0f, 1.0f);
	//output.Color = gColor.Sample(gTriLinearSam, decalUV);
	//output.Color = float4(uv.x, 0.0f, uv.y, 1.0f);
	//output.Color = float4(depth, depth, depth, 1.0f);
	//output.Color = float4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
	//output.Color = float4(localPosition.xyz, 1.0f);
	//output.Color.rgb = input.Normal;

	return output;
}