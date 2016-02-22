cbuffer a : register(b0)
{
	float4x4 gWVP;
	float4x4 gWorldViewInvTrp;
	float4x4 gWorld;
	float4 gCamPos; //Used for parallax occlusion mapping
	float4x4 gWorldView;
};
cbuffer a : register(b1)
{
	float3 Pos;
	float Range;
	float3 Color;
	float Intensity;
};

//Texture2D ldep : register(t0);
//Texture2D gdep : register(t1);
//SamplerState TriLinearSam : register(s0);

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float4 PosV : POSITION0;
	//float4 PosL : POSITION1;
	float3 Normal : NORMAL;
};

float4 main(VS_OUT input) : SV_TARGET
{
	//input.PosV /= input.PosV.w;
	//float2 tex = (input.PosV.xy + float2(1.0f,1.0f))*0.5;
	//tex.y = -tex.y;
	//float ld = ldep.Sample(TriLinearSam, tex);
	//float gd = gdep.Sample(TriLinearSam, tex);
	//ld = (gd < ld) ? gd : ld;

	//float a = pow(input.PosV.z-ld),10);

	float3 pos = input.PosV.xyz;// / input.PosV.w;
	pos = normalize(-pos);
	float3 N = normalize(input.Normal);
	float a = max(0,dot(N, pos));//pow(1 - max(dot(normalize(input.Normal), pos),0), 10);

	a = pow(a, 50);
	//input.Normal = normalize(input.Normal);

	//float a = max(0, dot(-cpv, input.Normal));
	//input.PosL.xy /= input.PosL.w;
	//float2 smTex = float2(0.5*input.PosL.x + 0.5f, -0.5f*input.PosL.y + 0.5f);
	//float d3 = ldep.Sample(TriLinearSam, smTex);
	//float d2 = gdep.Sample(TriLinearSam, smTex);
	//float d = input.PosH.z / input.PosH.w;
	//d = d-  max(d3,d2);
	//a = pow(d, 2);
	return float4(Color,0.5f)*a*Intensity;
}