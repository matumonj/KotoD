#pragma once
#include "Framework.h"
#include"SceneManager.h"

/// �Q�[���V�[��
class IkeGame : public Framework
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private: // �ÓI�����o�ϐ�
	static const int debugTextTexNumber = 0;

public: // �����o�֐�

	//�R���X�g�N���^
	IkeGame();

	//�f�X�g���N�^
	~IkeGame();

	//������
	void Initialize(DirectXCommon* dxCommon) override;

	//���t���[������
	void Update(DirectXCommon* dxCommon) override;

	//�`��
	void Draw(DirectXCommon* dxCommon) override;

	//�I��
	void Finalize() override;


private://�����o�ϐ�

};