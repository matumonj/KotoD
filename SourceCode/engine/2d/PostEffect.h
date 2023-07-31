#pragma once
#include "IKESprite.h"

class PostEffect : public IKESprite
{
private:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct CONST_BUFFER_DATA_POST
	{
		XMFLOAT3 Color; //�F
		float sepia; //�Z�s�A
		bool isTone; //�g�[���̗L��
		float frame; //���ʕ\��
		XMFLOAT2 P1;
		XMFLOAT2 P2;
		XMFLOAT2 P3;
		float RadPower;
		XMFLOAT2 CenterPos;
		float closeRad;
	};

public:
	//�R���X�g���N�^
	PostEffect();
	/// <summary>
  /// �p�C�v���C������
  /// </summary>
  /// <param name="cmdList"></param>
	void CreateGraphicsPipeline(const wchar_t* vsShaderName, const wchar_t* psShaderName);
	//������
	void Initialize();

	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//ImGui
	void ImGuiDraw();

	/// <summary>
	/// �V�[���`��O����
	/// </summary>
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �V�[���`��㏈��
	/// </summary>
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);
private: //�ÓI�����o�ϐ�
	static const float clearColor[4];
public:
	void SetSepia(float addsepia) { this->addsepia = addsepia; }
private: //�����o�ϐ�
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff[2];
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//�O���t�B�b�N�X�p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;
	//�Z�s�A
	static float addsepia;
	int tex = 0;

	XMFLOAT2 P1 = {0.10f, 0.05f};
	XMFLOAT2 P2 = {0.50f, 0.5f};
	XMFLOAT2 P3 = {2.00f, 1.00f};
	
};
