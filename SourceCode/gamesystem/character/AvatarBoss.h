#pragma once
#include "InterBoss.h"
#include "Shake.h"
#include "Poltergeist.h"
#include "DamageBlock.h"
#include "IKETexture.h"
class AvatarBoss :
	public InterBoss {
public:
	AvatarBoss();

	bool Initialize() override;//������

	void SkipInitialize() override;//�X�L�b�v���̏�����

	void Pause() override;//�|�[�Y

	void Action() override;//�s��

	void AppearAction() override;//�{�X�o��̌ŗL�̏���

	void DeadAction() override;//�{�X���j�̌ŗL�̏���

	void DeadAction_Throw() override;//�{�X���j�̌ŗL�̏��� �X���[

	void ImGui_Origin() override;//�{�X���ꂼ���ImGui

	void EffecttexDraw(DirectXCommon* dxCommon) override;

	void Draw(DirectXCommon* dxCommon) override;//�`��

	void InitAwake() override;//�{�X�̊o��

	void EndRollAction() override;//�G���h���[���̍X�V

	void AfterAction() override;//��b
private:
	void CSVLoad();

	//�e�{�X�̍s��
	void InterValMove();//�C���^�[�o��
	void Polter();//�|���^�[�K�C�X�g
	void ThrowBound();//�o�E���h�e
	void BlockAttack();//�_���[�W�u���b�N�̐���
	void BirthBlock();
	void BirthPolter(const std::string& PolterName);//�|���^�[�K�C�X�g�̐���
	void Vanish();
	void BirthExplosion();
	//�A�o�^�[�̃^�C�v
	void AvatarNormal();
	void AvatarAround();
	void AvatarRight();
	void AvatarLeft();
public:
private:
	//�L�����̏��
	enum CharaState
	{
		STATE_INTER,
		STATE_POLTER,
		STATE_BOUND,
		STATE_BLOCK,
		STATE_VANISH,
	}_charaState;

	//�֐��|�C���^
	static void(AvatarBoss::* stateTable[])();
	static void(AvatarBoss::* avatarTable[])();
private:
	static const int POLTER_NUM = 2;
	static const int BLOCK_NUM = 3;
private:
	unique_ptr<IKETexture> tex;
	vector<Poltergeist*> poltergeist;//�|���^�[�K�C�X�g
	vector<DamageBlock*> damageblock;//�_���[�W�u���b�N
	int m_InterVal = {};

	int m_MoveTimer = {};

	//�U���̗���
	int m_AttackRand = {};

	//�C�[�W���O��̈ʒu
	XMFLOAT3 m_AfterPos = {};

	bool m_Return = false;

	bool m_Tackle = false;

	enum ReturnState {
		RETURN_SET,
		RETURN_PLAY,
		RETURN_END,
	}_ReturnState;
	//���������鎞��
	float m_VanishFrame = {};

	float m_AddSpeed = {};
	enum AvatarType {
		AVATAR_ONE,
		AVATAR_SECOND,
		AVATAR_THIRD,
		AVATAR_FOURTH
	};

	enum VanishType {
		VANISH_SET,
		VANISH_ATTACK,
		VANISH_EXPLO,
		VANISH_END,
	}_VanishType;

	float m_Frame = {};

	int m_AttackCount = {};

	//�e�N�X�`���֘A
	XMFLOAT3 m_TexPos = {};
	XMFLOAT3 m_TexRot = {};
	XMFLOAT3 m_TexScale = {};
	XMFLOAT4 m_TexColor = { 1.0f,1.0f,1.0f,0.0f };
	float m_Alpha = 0.0f;
	float m_AfterAlpha = 0.0f;

	bool m_TexAlive = false;
};
