#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "DirectXCommon.h"

/// �X�v���C�g

class IKESprite
{
protected: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X

	/// ���_�f�[�^�\����
	struct VertexPosUv
	{
		XMFLOAT3 pos; // xyz���W
		XMFLOAT2 uv; // uv���W
	};


	/// �萔�o�b�t�@�p�f�[�^�\����

	struct ConstBufferData
	{
		XMFLOAT4 color; // �F (RGBA)
		XMMATRIX mat; // �R�c�ϊ��s��
		XMFLOAT2 offset;//�I�t�Z�b�g�l
	};

public: // �ÓI�����o�֐�
	static bool StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, int window_width,
	                             int window_height);

	/// �e�N�X�`���ǂݍ���
	static bool LoadTexture(UINT texnumber, const wchar_t* filename);

	/// �`��O����
	static void PreDraw();

	/// �`��㏈��
	static void PostDraw();

	/// �X�v���C�g����
	static std::unique_ptr<IKESprite> Create(UINT texNumber, const XMFLOAT2& position, const XMFLOAT4& color = {1, 1, 1, 1},
	                         const XMFLOAT2& anchorpoint = {0.0f, 0.0f}, bool isFlipX = false, bool isFlipY = false);

protected: // �ÓI�����o�ϐ�
	// �e�N�X�`���̍ő喇��
	static const int srvCount = 512;
	// ���_��
	static const int vertNum = 4;
	// �f�o�C�X
	static ID3D12Device* device;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootSignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelineState;
	// �ˉe�s��
	static XMMATRIX matProjection;
	// �f�X�N���v�^�q�[�v
	static ComPtr<ID3D12DescriptorHeap> descHeap;
	// �e�N�X�`���o�b�t�@
	static ComPtr<ID3D12Resource> texBuff[srvCount];

	float closeRad;

public: // �����o�֐�

	/// �R���X�g���N�^
	IKESprite(UINT texNumber, const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color,
	          const XMFLOAT2& anchorpoint, bool isFlipX, bool isFlipY);


	/// ������
	bool Initialize();


	/// �p�x�̐ݒ�
	void SetRotation(float rotation);


	/// ���W�̐ݒ�
	void SetPosition(const XMFLOAT2& position);

	/// ���W�̎擾
	const XMFLOAT2& GetPosition() { return position; }

	/// �T�C�Y�̐ݒ�
	void SetSize(const XMFLOAT2& size);
	/// 
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(float scale);

	/// <summary>
	/// �T�C�Y�̐ݒ�
	/// </summary>
	/// <param name="size">�T�C�Y</param>
	const XMFLOAT2& GetSize() { return size; }


	/// �J���[�̐ݒ�
	void IKESprite::SetColor(const XMFLOAT4& color);


	/// �A���J�[�|�C���g�̐ݒ�
	void SetAnchorPoint(const XMFLOAT2& anchorpoint);


	/// ���E���]�̐ݒ�
	void SetIsFlipX(bool isFlipX);


	/// �㉺���]�̐ݒ�
	void SetIsFlipY(bool isFlipY);


	/// �e�N�X�`���͈͐ݒ�
	void SetTextureRect(const XMFLOAT2& texBase, const XMFLOAT2& texSize);


	/// �`��

	void Draw();

	//�J��
	static bool Finalize();
	bool CreateVertices();


	void SetRadCenter(XMFLOAT2 pos) { RadCenterPos = pos; }
	void SetRadPower(float p) { RadPowers = p; }
	//�I�t�Z�b�g�̐ݒ�
	void SetAddOffset(float addoffset) { this->addoffset = addoffset; }
	void SetCloseRad(float r) { closeRad = r; }

protected: // �����o�ϐ�
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// �e�N�X�`���ԍ�
	UINT texNumber = 0;
	// Z�����̉�]�p
	float rotation = 0.0f;
	// ���W
	XMFLOAT2 position{};
	// �X�v���C�g���A����
	XMFLOAT2 size = {100.0f, 100.0f};
	// �A���J�[�|�C���g
	XMFLOAT2 anchorpoint = {0, 0};
	// ���[���h�s��
	XMMATRIX matWorld{};
	//�I�t�Z�b�g
	float addoffset = 0.0f;
	//�I�t�Z�b�g�l
	XMFLOAT2 offset = { 0.0f,0.0f };
	// �F
	XMFLOAT4 color = {1, 1, 1, 1};
	// ���E���]
	bool isFlipX = false;
	// �㉺���]
	bool isFlipY = false;
	// �e�N�X�`���n�_
	XMFLOAT2 texBase = {0, 0};
	// �e�N�X�`�����A����
	XMFLOAT2 texSize = {100.0f, 100.0f};

	float RadPowers;
	XMFLOAT2 RadCenterPos;
private: // �����o�֐�
	/// ���_�f�[�^�]��
	void TransferVertices();
};
