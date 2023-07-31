#pragma once
#include "IKETexture.h"
#include <array>
using namespace std;         //  ���O��Ԏw��
//�������̃G�t�F�N�g
class BossStunEffect {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	BossStunEffect();
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw(DirectXCommon* dxCommon);

	void ImGuiDraw();
private:
	//��̂҂�҂�̂��
	void StunEffect();

private:
	static const int STUN_MAX = 4;
public:
	//gettersetter
	bool GetAlive() { return m_Alive; }

	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetBasePos(const XMFLOAT3& BasePos) { m_BasePos = BasePos; }
private:
	array<unique_ptr<IKETexture>, STUN_MAX> stuntex;
	bool m_Alive = false;

	//�҂�҂�̈ʒu
	array<float, STUN_MAX> m_Stunradius;
	array<float, STUN_MAX> m_StunSpeed;
	array<float, STUN_MAX> m_Stunscale;
	array<float, STUN_MAX> m_StunCircleX;
	array<float, STUN_MAX> m_StunCircleZ;
	array<XMFLOAT3, STUN_MAX> m_StunPos;
	XMFLOAT3 m_BasePos = {};
};