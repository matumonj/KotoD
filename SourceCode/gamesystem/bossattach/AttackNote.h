#pragma once
#include "ObjCommon.h"
#include "InterBullet.h"
enum SetType {
	SET_FOLLOW,
	SET_DIFF,
};

//�{�X�����ލU�������N���X
class AttackNote :
	public ObjCommon {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	AttackNote();

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
	static void (AttackNote::* stateTable[])();
private:
	void Particle();
	void Follow();
	void Diffusion();
public:

public://getter setter
public:
	//gettersetter
	const bool& GetAlive() { return m_Alive; }
	void SetType(const int SetType) { m_SetType = SetType; }
	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetChange(const bool Change) { m_Change = Change; }
	void SetAngle(const XMFLOAT2& Angle) { m_Angle = Angle; }
private:
	bool m_Alive = true;//�����t���O
private:
	//�L�����̏��
	enum CharaState
	{
		STATE_FOLLOW,
		STATE_DIFF,
	}_charaState = CharaState::STATE_FOLLOW;

private:
	IKEModel* m_Model2;
	XMFLOAT2 m_Angle = {};//�e�̊p�x
	float m_AddSpeed = {};//�����x


	int m_SetType = SET_FOLLOW;

	int m_TargetTimer = {};//�o�����Ԃ̖ڕW

	int m_Timer = 0;

	float m_Damage = {};

	bool m_Change = false;
};
