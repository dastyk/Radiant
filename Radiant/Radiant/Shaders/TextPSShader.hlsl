cbuffer Buffers : register(b0)
{
	float4 Color;
};



Texture2D Texture : register(t0);
SamplerState TriLinearSam : register(s0);

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{

	float4 text = Texture.Sample(TriLinearSam, input.TexC);
	text *= Color;
	return text;
}