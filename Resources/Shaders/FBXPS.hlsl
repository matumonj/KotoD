#include "FBX.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

struct PSOutput
{
	float4 target0 : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
	PSOutput output;
	// テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);
	// Lambert反射
	float3 light = normalize(float3(1, -1, 1)); // 右下奥　向きのライト
	float diffuse = saturate(dot(-light, input.normal));
	float brightness = diffuse + 0.5f;
	float4 shadecolor = float4(brightness, brightness, brightness, 1.0f);

	// 陰影とテクスチャの色を合成
	if (ShadeSet)
	{
		output.target0 = shadecolor * texcolor * color;
	}
	else
	{
		output.target0 = texcolor * color;
	}
	return output;
}
