Texture2D gTexture : register(t0);
SamplerState gTriLinearSam : register(s0);

float4 PS( float4 posH : SV_POSITION, float2 texC : TEXCOORD ) : SV_TARGET
{
	float4 color;

	[unroll]
	for ( int i = -HKERNEL_HALF; i < HKERNEL_HALF; ++i )
	{
		for ( int j = -VKERNEL_HALF; j < VKERNEL_HALF; ++j )
		{
			color += gTexture.Sample( gTriLinearSam, texC + float2(i * TEXEL_WIDTH, j * TEXEL_HEIGHT) );
		}
	}

	color /= (2.0f * HKERNEL_HALF * 2.0f * VKERNEL_HALF);
	return color;
}