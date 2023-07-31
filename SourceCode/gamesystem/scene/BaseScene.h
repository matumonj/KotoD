#pragma once
#include "DebugCamera.h"
#include "LightGroup.h"
#include "DirectXCommon.h"
#include "PostEffect.h"
#include "BaseActor.h"
#include <vector>
#include <memory>
using namespace std;         //  ���O��Ԏw��
//�O���錾
class SceneManager;

//�V�[���C���^�[�t�F�[�X
class BaseScene {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;


public:
	//���z�f�X�g���N�^
	virtual ~BaseScene() = default;

	//������
	virtual void Initialize(DirectXCommon* dxCommon) = 0;
	//�J��
	virtual void Finalize() = 0;
	//�X�V
	virtual void Update(DirectXCommon* dxCommon) = 0;
	//�`��
	virtual void Draw(DirectXCommon* dxCommon) = 0;
	//���ʏ�����
	void BaseInitialize(DirectXCommon* dxCommon);

public:
	//�A�N�^�[�N���X
	unique_ptr<BaseActor> actor;
	///�Q�[���V�[���p
	DebugCamera* camera = nullptr;
	//�|�X�g�G�t�F�N�g�������邩
	bool m_PlayPostEffect = false;
	//���C�g
	LightGroup* lightGroup = nullptr;
};