#pragma once
#include <DirectXMath.h>
//�ړ������p�̃p�[�e�B�N���N���X(��)
class UpLimit {
private:
	//template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public:
	static UpLimit* GetInstance();
	void LimitParticle();

private:

	XMFLOAT3 m_ParticlePos = {};
};