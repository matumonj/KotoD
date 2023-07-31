#pragma once
#include "InterBoss.h"
#include "Shake.h"
#include "BossStunEffect.h"
#include "Poltergeist.h"
#include "AvatarBoss.h"
#include "FireBoll.h"
#include "AbsorptionEffect.h"
#include "ConfuEffect.h"
#include "DamageBlock.h"
class SevenBoss :
	public InterBoss {
public:
	SevenBoss();

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
	void BirthAvatar();//�U���̃{�X
	void Manipulate();//����
	void BulletCatch();//�e���z��
	void FireAttack();//�΂̋ʍU��
	void BirthFire();//������
	void Confu();//����
	void BlockAttack();//�_���[�W�u���b�N�̐���
	void BirthBlock();
	void Stun();//�X�^��
	void BirthExplosion();

	void BirthPolter(const std::string& PolterName);//�|���^�[�K�C�X�g�̐���

	void CatchBul(vector<InterBullet*>bullet);

	void BirthParticle();

	//��u�{�X��������
	void VanishBoss();

	//�����X�^�[��������͈͐ݒ�
	void VanishCollide(vector<InterBullet*>bullet);

	//�����_���ɓ���
	void RandMove();
	//�{�X���߂�
	void ReturnBoss();

	void Rebound();
public:

private:
	//�L�����̏��
	enum CharaState
	{
		STATE_INTER,
		STATE_POLTER,
		STATE_BOUND,
		STATE_AVATAR,
		STATE_MANIPULATE,
		STATE_FIRE,
		STATE_CONFU,
		STATE_BLOCK,
		STATE_CATCH,
		STATE_STUN,
	}_charaState;

	//�֐��|�C���^
	static void(SevenBoss::* stateTable[])();
private:
	static const int POLTER_NUM = 4;
	static const int FIRE_NUM = 8;
	static const int BLOCK_NUM = 6;
private:
	vector<Poltergeist*> poltergeist;//�|���^�[�K�C�X�g
	vector<FireBoll*> fireboll;//�΂̋�
	vector<InterBoss*> avatarboss;//�U���̃{�X
	vector<DamageBlock*> damageblock;//�_���[�W�u���b�N
	vector<AbsorptionEffect*> abseffect;//�z��
	unique_ptr<ConfuEffect> confueffect;
	unique_ptr<BossStunEffect> bossstuneffect;
	float m_SinAngle = {};
	float m_SinAngle2 = {};
	int m_InterVal = {};

	int m_MoveTimer = {};
	int m_AvatarCount = {};

	float m_Frame = {};

	//�U����
	int m_AttackCount = {};
	//�X�^���������ǂ���
	bool m_Stun = false;
	//�U���̗���
	int m_AttackRand = {};
	//��������̉~���̈ʒu
	float m_RandSpeed = {};
	//��_�̋���
	float m_Dir = {};
	//�G���e������邩�ǂ���
	bool m_Vanish = false;

	//�����̃X�e�[�g
	enum VanishState {
		VANISH_SET,
		VANISH_END,
	}_vanishState;
	//���������鎞��
	float m_VanishFrame = {};
	//���x
	float m_AfterAlpha = {};
	//����������m��
	int m_VanishTarget = {};
	XMFLOAT3 m_AfterPos = {};

	int m_RotTimer = {};
	bool m_StartMani = false;

	//CSV�n
	//�e�C���^�[�o���⃊�~�b�g����
	vector<int>m_RandAct;

	enum RandState {
		RAND_POLTER,
		RAND_BOUND,
		RAND_MANIPULATE,
		RAND_FIRE,
		RAND_CONFU,
		RAND_BLOCK,
	};

	int m_ChangeTimer = {};
	float m_AddSpeed = {};
	float m_AddScale = {};

	bool m_Return = false;

	enum ReturnState {
		RETURN_SET,
		RETURN_PLAY,
		RETURN_END,
	}_ReturnState;

	//���g�{�X�̐�
	int AVATAR_NUM = 2;

	//CSV�n
	//�e�C���^�[�o���⃊�~�b�g����
	//�����O
	vector<int>m_Limit;
	//������
	vector<int>m_StrongLimit;
	//�ړ����x
	

	//���j�V�[���֌W
	float m_BoundPower = {};
	bool m_Bound = false;

	enum DeathState {
		DEATH_SET,
		DEATH_ATTACK,
		DEATH_TALK,
		DEATH_NO_TALK,
	}_DeathState;

	int m_DeleteNumber = {};

	enum EndDir {
		DIR_NONE,
		DIR_PLAYER,
		DIR_RETURN,
	}_EndDir = DIR_NONE;

	XMFLOAT2 m_BullBoundPower = {};
};