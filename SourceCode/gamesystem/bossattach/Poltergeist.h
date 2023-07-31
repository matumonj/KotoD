#pragma once
#include "ObjCommon.h"
enum PolterType {
	TYPE_FOLLOW,
	TYPE_BOUND,
};
//�{�X�����ލU�������N���X
class Poltergeist :
	public ObjCommon {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	Poltergeist();

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

	bool Collide();	//�����蔻��

private://�X�e�[�g
	static void (Poltergeist::* stateTable[])();
private:
	void Particle();
	void Follow();//�Ǐ]
	void Bound();//���˒e
public:

public://getter setter
public:
	//gettersetter
	const bool& GetAlive() { return m_Alive; }

	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetPolterType(const int PolterType) { m_PolterType = PolterType; }

	void SetTargetTimer(const int TargetTimer) { m_TargetTimer = TargetTimer; }
	void SetCircleSpeed(const float CircleSpeed) { m_CircleSpeed = CircleSpeed; }

	void SetBasePos(const XMFLOAT3& BasePos) { m_BasePos = BasePos; }
private:
	bool m_Alive = true;//�����t���O
	int m_PolterType = TYPE_FOLLOW;

	int m_TargetTimer = {};//�o�����Ԃ̖ڕW

	int m_ThrowTimer = {};

	enum ThrowType {
		THROW_SET,
		THROW_INTER,
		THROW_PLAY,
	};

	int m_ThrowType = THROW_SET;
	XMFLOAT3 m_AfterPos = {};
	float m_Frame = {};

	double m_SpeedX = {};
	double m_SpeedZ = {};

	//�~�^��
	XMFLOAT3 m_BasePos = {};
	float m_CircleScale = 10.0f;
	float m_CircleSpeed = {};

	XMFLOAT2 m_Angle = {};

	float m_AddSpeed = {};//�����x

	int m_AliveTimer = {};

	XMFLOAT2 m_Power = {};
};
