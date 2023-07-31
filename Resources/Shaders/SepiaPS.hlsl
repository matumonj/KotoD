#include"PostEffectTest.hlsli"
Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	float4 col = tex.Sample(smp, input.uv);
	float sepia = addsepia;
	float grayScale = col.r * 0.299 + col.g * 0.587 + col.b * 0.114;
	float4 color = float4(grayScale + sepia, grayScale, grayScale - sepia, 1);
	return color;
}
