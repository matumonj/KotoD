cbuffer cbuff0 : register(b0)
{
float3 Color; //�F
float addsepia; //�Z�s�A
bool isTone; //�g�[���̗L��
float frame; //���ʕ\��
float2 P_1;
float2 P_2;
float2 P_3;
float RadPower;
float2 RadCenter;
float CloseIconRad;
};

//���_�V�F�[�_�[����̏o�͍\����
struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv :TEXCOORD;
};
