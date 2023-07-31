#include "Gaussian.hlsli"

Texture2D<float4> tex0 : register(t0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex1 : register(t1);
SamplerState smp : register(s0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
	float d = distance(drawUV, pickUV);
	return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(VSOutput input) : SV_TARGET
{
	////�ڂ�����
	//float totalWeight = 0, _Sigma = 0.0025, _StepWidth = 0.001;
	//float4 col = float4(0, 0, 0, 0);

	//for (float py = -_Sigma * 2; py <= _Sigma * 2; py += _StepWidth) //xy��2a�̕��ŐF���擾
	//{
	//	for (float px = -_Sigma * 2; px <= _Sigma * 2; px += _StepWidth)
	//	{
	//		float2 pickUV = input.uv + float2(px, py);
	//		float weight = Gaussian(input.uv, pickUV, _Sigma);
	//		col += tex0.Sample(smp, pickUV) * weight;//Gaussian�Ŏ擾�����d�݂�F�ɂ�����

	//		totalWeight += weight;//�������d�݂̍��v�l�����߂�
	//	}
	//}

	//col.rgb = col.rgb / totalWeight; //�������d�ݕ�����
	//return float4(col.rgb, 1);
	float4 texcolor = tex0.Sample(smp, input.uv);
	//// //default
	////// return float4(texcolor.rgb, 1);
	//�ڂ���
	// 2.5�e�N�Z�������炷���߂�UV�l�����߂�
	float offsetU = 3.5f / 1280.0f;
	float offsetV = 3.5f / 720.0f;

	// ��e�N�Z������E�̃e�N�Z���̃J���[���T���v�����O����
	texcolor += tex0.Sample(smp, input.uv + float2(offsetU, 0.0f));

	// ��e�N�Z�����獶�̃e�N�Z���̃J���[���T���v�����O����
	texcolor += tex0.Sample(smp, input.uv + float2(-offsetU, 0.0f));

	// ��e�N�Z�����牺�̃e�N�Z���̃J���[���T���v�����O����
	texcolor += tex0.Sample(smp, input.uv + float2(0.0f, offsetV));

	// ��e�N�Z�������̃e�N�Z���̃J���[���T���v�����O����
	texcolor += tex0.Sample(smp, input.uv + float2(0.0f, -offsetV));

	// ��e�N�Z������E���̃e�N�Z���̃J���[���T���v�����O����
	texcolor += tex0.Sample(smp, input.uv + float2(offsetU, offsetV));

	// ��e�N�Z������E��̃e�N�Z���̃J���[���T���v�����O����
	texcolor += tex0.Sample(smp, input.uv + float2(offsetU, -offsetV));

	// ��e�N�Z�����獶���̃e�N�Z���̃J���[���T���v�����O����
	texcolor += tex0.Sample(smp, input.uv + float2(-offsetU, offsetV));

	// ��e�N�Z�����獶��̃e�N�Z���̃J���[���T���v�����O����
	texcolor += tex0.Sample(smp, input.uv + float2(-offsetU, -offsetV));

	// ��e�N�Z���ƋߖT8�e�N�Z���̕��ςȂ̂�9�ŏ��Z����
	texcolor /= 9.0f;

	return float4(texcolor.rgb, 1);
}
