#pragma once
#include "IKETexture.h"
#include <array>
using namespace std;         //  ���O��Ԏw��
//�������̃G�t�F�N�g
class AbsorptionEffect {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	AbsorptionEffect();
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw(DirectXCommon* dxCommon);

	void ImGuiDraw();
private:
	void MoveEffect();
private:
public:
	//gettersetter
	bool GetAlive() { return m_Alive; }

	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetAddFrame(const float AddFrame) { m_AddFrame = AddFrame; }
	void SetBasePos(const XMFLOAT3& BasePos) { m_BasePos = BasePos; }
	void SetColor(const XMFLOAT4& Color) { m_Color = Color; }

private:
	unique_ptr<IKETexture> tex;

	float m_CircleRadius;
	float m_CircleSpeed;
	float m_CircleScale;
	float m_CircleX;
	float m_CircleZ;
	float m_AddFrame = {};

	bool m_Alive = false;
	XMFLOAT3 m_Position = {};
	XMFLOAT3 m_BasePos = {};

	float m_Frame = {};

	XMFLOAT3 m_Scale = { 0.3f,0.3f,0.3f };
	XMFLOAT4 m_Color = {};
};