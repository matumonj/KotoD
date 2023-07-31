#pragma once
#include <DirectXMath.h>
//移動制限用のパーティクルクラス(下)
class LeftLimit {
private:
	//template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public:
	static LeftLimit* GetInstance();
	void LimitParticle();

private:

	XMFLOAT3 m_ParticlePos = {};
};