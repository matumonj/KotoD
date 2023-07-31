#pragma once
#include "ObjCommon.h"
//�n�[�g�N���X
class Heart :
	public ObjCommon {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	Heart();

	bool Initialize() override;//������
	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(DirectXCommon* dxCommon) override;

	/// <summary>
	/// ImGui�`��
	/// </summary>
	void ImGuiDraw();

private:
	void HeartJump();

	void HeartSet();

	void HeartVanish();
private://�X�e�[�g
	static void (Heart::* stateTable[])();
private:
	//�p�[�e�B�N��
	void Particle();
	//�����蔻��(�v���C���[)
	bool PlayerCollision();
public://getter setter
	const bool& GetAlive() { return m_Alive; }
private:
	//�����t���O
	bool m_Alive = false;
	//��������
	int m_AliveTimer = {};
	//�������Ԃ̐���
	int m_LimitTimer = {};

	bool m_Vanish = false;
private:
	enum HeartState {
		HEART_JUMP,
		HEART_SET,
		HEART_VANISH,
	}_heartState;

	XMFLOAT3 m_AddPower = {};
	float m_Gravity = 0.05f;

	float m_Frame = 0.0f;
};
