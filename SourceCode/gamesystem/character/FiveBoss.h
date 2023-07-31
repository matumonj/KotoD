#pragma once
#include "InterBoss.h"
#include "JoyStamp.h"
#include "AngerStamp.h"
#include "ShockWave.h"
#include "Predict.h"
#include "Collision.h"
#include "Shake.h"
#include "ConfuEffect.h"
#include "NormalAttack.h"
#include "NoteEffect.h"
#include "ShotAttack.h"
#include"SmashShotAttack.h"
#include"ShadowSlashAttack.h"
#include"GuardAction.h"
#include"SingleShot.h"
#include "KnockAttack.h"
#include"DarkSutoponShot.h"
class Spline;

class FiveBoss :
	public InterBoss
{
public:
	FiveBoss();

	bool Initialize() override;//������

	void SkipInitialize() override;//�X�L�b�v���̏�����

	void Pause() override;//�|�[�Y

	void Action() override;//�s��

	void EndRollAction() override;

	void AppearAction() override;//�{�X�o��̌ŗL�̏���

	void DeadAction() override;//�{�X���j�̌ŗL�̏���

	void DeadAction_Throw() override;//�{�X���j�̌ŗL�̏��� �X���[

	void ImGui_Origin() override;//�{�X���ꂼ���ImGui

	void InitAwake() override;//�{�X�̊o��

	void EffecttexDraw(DirectXCommon* dxCommon) override;

	void Draw(DirectXCommon* dxCommon) override;//�`��

	void AfterAction() override;//��b
private:
	void SetEasePos();
	void BirthParticle();
private:
	//CSV�ǂݍ��݌n
	void CSVLoad();
	//���񂾂Ƃ��̃p�[�e�B�N��
	void DeathParticle();
	//�o��V�[���̃p�[�e�B�N��
	void AppParticle();
private:
	static const int CD_NUM = 4;
private:
	//�e�N���X
	unique_ptr<ConfuEffect> confueffect;
	unique_ptr<NoteEffect> noteeffect;
	//�L�����̏��
	enum CharaState
	{
		STATE_INTER,
		STATE_CHOICE,
		STATE_ROCKON,
		STATE_RAND,
		STATE_HIT,
		STATE_END
	};

	//��~����
	int m_StopTimer = 0;
	//�ǂ̍s���ɂ��邩
	int m_MoveState = {};
	int m_ParticleTimer = {};

	int _charaState = STATE_INTER;

	//CSV�n
	int m_ChoiceInterval = {};

	//�C�[�W���O��̈ʒu
	XMFLOAT3 m_AfterPos = {};
	//X�����̉�]
	XMFLOAT3 m_AfterRot = { 0.0f,0.0f,0.0f };
	float m_Frame = {};

	int m_RotCount = 0;
	int m_RotTimer = 0;
	
	enum AreaState {
		AREA_SET,
		AREA_STOP,
		AREA_END,
	};

	int m_AreaState = AREA_SET;
	float SplineSpeed = false;

	//�����̃C���^�[�o��
	int m_MoveInterVal = {};
	//�s���I���̐�
	int m_EndCount = {};
	//�L���b�`����CD�̐�
	int m_CatchCount = {};
	//�{�X���v���C���[���瓦���鎞��
	int m_EndTimer = {};

	//���̓I�Ɏg��
	float m_Angle = 0.0f;
	float m_Angle2 = 0.0f;
	//��_�̋���
	float m_Length = {};

	//�~�^��
	float m_CircleScale = 30.0f;
	float m_CircleSpeed = {};

	//�e���̎��
	int m_BarraRand = {};

	int m_AttackRand = {};

	//CSV�n
	//�e�C���^�[�o���⃊�~�b�g����
	vector<int>m_Limit;

	enum LimitState {
		LIMIT_BASE,
		LIMIT_CONFU,
		LIMIT_STRONG_CONFU,
		LIMIT_BARRA,
	};

	enum EndState2 {
		END_SET2,
		END_RIGHT,
		END_LEFT,
		END_TOP,
		END_MOVE2,
	}_EndState2;

	enum ViewType {
		VIEW_MOVE,
		VIEW_RETURN,
	}_ViewType = VIEW_MOVE;

	int m_ViewTimer = {};

	bool m_View = false;
	//�ړ���
	float m_FollowSpeed = {};
	ShotAttack* shot;
	NormalAttack* normal;
	SmashShotAttack* smash;
	ShadowSlashAttack* slash;
	SingleShot* single;
	GuardAction* guard;
	KnockAttack* knock;
	DarkSutoponShot* darkshot;
	int ThreeGhostActionRand;
	bool JudgDShot;
	bool JudgSlash;
	int CoolDShot;
	float WalkSpeed;

		//�K�[�h��������
	float GuardTime;
		//�m�b�N�o�b�N�p�x
	int KnockInter;
	int noAction;
	float KnockDam, MeteoDam, ShotDam, UltDam,DarkShotDam;
	int KnockTimer=1;
	int GuardCount;
	static void (FiveBoss::* attackTable[])();
	enum ActionPhase
	{
		MOVE,
		ATTACK_SHOT,
		ATTACK_NORMAL,
		ATTACK_IMPACT,
		ATTACK_SINGLESHOT
	}_aPhase = ATTACK_SHOT;

	enum DeathState {
		DEATH_SET,
		DEATH_KNOCK,
		DEATH_STOP
	}_DeathState = DEATH_SET;

	enum AppState {
		APP_SET,
		APP_BIRTH,
		APP_END,
	}_AppState = APP_SET;
public:

private:
	void ActionSet(ActionPhase phase, InterAttack* attack);
	int RandAction;
	int ActionTimer;
	size_t bonesize;
	std::vector<XMFLOAT3> bonepos;;
	std::vector<XMMATRIX> bonemat;;
	std::vector<XMFLOAT4> s_color;
	std::vector<XMFLOAT4>e_color;
	std::vector<float> s_scale;
	std::vector<float> e_scale;
	std::vector<int> m_Life;
	int JudgAttack;

	
	inline void Shot() { shot->Upda(); }
	inline void Normal() { normal->Upda(); }
	inline void Smash() { smash->Upda(); }
	inline void Slash() { slash->Upda(); }
	inline void Single() { single->Upda(); }
	void MatTranstoPos(XMMATRIX trans, XMFLOAT3& m_Pos);
};


