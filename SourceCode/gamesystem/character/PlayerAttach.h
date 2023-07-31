#pragma once
#include "ObjCommon.h"
using namespace std;         //  ���O��Ԏw��

//�e�̊��N���X
class PlayerAttach :
	public ObjCommon {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	PlayerAttach();
	//������
	bool Initialize()override;
	//�X�V
	void Update()override;
	//�`��
	void Draw(DirectXCommon* dxCommon)override;
	//ImGui����
	void ImGuiDraw();
	//�p�[�e�B�N��
	void Particle();
	//�e�̏o��
	void BirthObj();
	//�o��V�[���̓���
	void AppearUpdate(int Timer);
	//�_�[�N�R�g�R�̓o��V�[��
	void DarkAppear(int  Timer);
	//���j�V�[���̓���
	void LastDeadUpdate(int Timer);
	//�G���h���[���̓���
	void EndRollUpdate(int Timer);
	void DeathUpdate();
	//�^�C�g��
	void TitleUpdate();
public:
	//gettersetter
	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetAngle(const XMFLOAT2& Angle) { m_Angle = Angle; }
	//void SetAfterRot(const float AfterRotY) { m_AfterRotY = AfterRotY; }
private:
	XMFLOAT2 m_Angle = {};//�e�̊p�x
	//float m_AfterScale = 0.0f;
	float m_AddSpeed = {};//�����x
	bool m_Alive = false;
	bool m_Particle = false;
	int m_ParticleTimer = 0;
	int m_AliveTimer = 0;
	float m_SinAngle = {};
	float m_SinAngle2 = {};
	float m_Frame = 0.0f;

	//�o��V�[��
	enum AppearState {
		APPEAR_SET,
		APPEAR_WALK,
		APPEAR_WALK2,
		APPEAR_STOP,
	}_AppearState;

	//���j�V�[���̕ϐ�
	enum DeathState {
		DEATH_SET,
		DEATH_MOVE,
		DEATH_TALK,
		DEATH_NO_TALK,
	}_DeathState;
	XMFLOAT3 m_AfterPos = {};


	enum EndState {
		END_SET,
		END_WALK,
		END_DIR_CAMERA,
	}_EndState = END_SET;

	bool m_Rot = false;
	float m_AfterRot = {};

	bool m_EndStop = false;
};