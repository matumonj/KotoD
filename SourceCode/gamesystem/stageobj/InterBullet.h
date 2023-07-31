#pragma once
#include "ObjCommon.h"
using namespace std;         //  ���O��Ԏw��

enum Bullettype {
	BULLET_FORROW,//�Ǐ]
	BULLET_SEARCH,//�T��
	BULLET_ATTACK,//�U��
};

enum AttackState {
	POWER_NONE,
	POWER_MIDDLE,
	POWER_STRONG,
	POWER_UNLIMITED
};
//�e�̊��N���X
class InterBullet :
	public ObjCommon {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	//������
	virtual bool Initialize() = 0;
	//�X�V
	void Update();
	
	//�`��
	void Draw(DirectXCommon* dxCommon);

	virtual void Action() = 0;//�e���L�̏���
	//ImGui����
	void ImGuiDraw();
	//���L��ImGui
	virtual void ImGui_Origin() = 0;

	virtual void Draw_Origin() = 0;
public:
	//gettersetter
	const bool& GetAlive() { return m_Alive; }
	const bool& GetInArea() { return m_InArea; }
	const bool& GetBossCatch() { return m_BossCatch; }
	const int& GetBulletType() { return m_BulletType; }
	const int& GetPowerState() { return m_PowerState; }
	const float& GetPower() { return m_DamagePower; }
	const float& GetRadius() { return m_Radius; }

	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetAngle(const XMFLOAT2& Angle) { m_Angle = Angle; }
	void SetBulletType(const int BulletType) { m_BulletType = BulletType; }
	void SetPowerState(const int PowerState) { m_PowerState = PowerState; }
	void SetCharge(const bool Charge) { m_Charge = Charge; }
	void SetInArea(const bool InArea) { m_InArea = InArea; }
	void SetBossCatch(const bool BossCatch) { m_BossCatch = BossCatch; }
	void SetTargetPos(const XMFLOAT3& TargetPos) { m_TargetPos = TargetPos; }
	void SetNumMag(const float NumMag) { m_NumMag = NumMag; }
public:
	XMFLOAT2 m_Angle = {};//�e�̊p�x
	float m_AddSpeed = {};//�����x
	int m_BulletType = {};//�e�̎��
	int m_Timer = {};//�o������
	int m_TargetTimer = {};//�o�����Ԃ̖ڕW
	bool m_Alive = true;//�����t���O
	bool m_Charge = false;//�`���[�W�������ǂ���

	int m_PowerState = POWER_NONE;
	float m_Power = {};//�_���[�W��
	float m_DamagePower = {};

	//�{�X���e���L���b�`������
	bool m_BossCatch = false;
	XMFLOAT3 m_TargetPos = {};

	bool m_InArea = false;

	float m_NumMag = 0.0f;

	//�e�̓����蔻��̔��a
	float m_Radius = {};
};