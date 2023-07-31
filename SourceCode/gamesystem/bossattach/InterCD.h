#pragma once
#include "ObjCommon.h"
#include "InterBullet.h"
#include "BreakEffect.h"
enum CDState {
	CD_BIRTH,
	CD_STAY,
	CD_CATCH,
	CD_DEATH,
	CD_RESPORN,
};
using namespace std;         //  ���O��Ԏw��
//�����N���X
class InterCD :
public ObjCommon {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	//CDV���[�h
	void CsvLoad();
	//������
	virtual bool Initialize() = 0;
	//�X�V
	void Update();
	//�`��
	void Draw(DirectXCommon* dxCommon);

	//�`��(�ŗL��)
	virtual void Origin_Draw(DirectXCommon* dxCommon) = 0;

	void ImGuiDraw();//ImGui�̕`��

	void DeathMove(const int Timer,const int TargetTimer);

	void EndMove(const int TargetTimer);
protected:

	virtual void Action() = 0;//�{�X���L�̏���

	virtual void AudioAction() = 0;//�{�X���L�̏���(���y)

	virtual void ImGui_Origin() = 0;//�{�X���ꂼ���ImGui

	void CollideBul(vector<InterBullet*>bullet);

	//�U���O��CD�̃Z�b�g
	void SetCD();
	//�G�t�F�N�g����
	void BirthEffect();
protected:
	//�����o�֐�
	virtual void BirthCD() {};
	virtual void StayCD() {};
	virtual void CatchCD() {};
	virtual void DeathCD() {};
	virtual void ResPornCD() {};
public:
	//gettersetter
	const int& GetCDState() { return m_CDState; }
	const bool& GetBreakCD() { return m_BreakCD; }

	void SetCDState(const int CDState) { m_CDState = CDState; }

	void SetBreakCD(const bool BreakCD) { m_BreakCD = BreakCD; }

	void SetAttackSetCD(const bool AttackSetCD) { m_AttackSetCD = AttackSetCD; }

	void SetCatchPos(const XMFLOAT3 CatchPos) { m_CatchPos = CatchPos; }
protected:
	static void (InterCD::* stateTable[])();
private:
protected:
	vector<InterEffect*> effects;
	//CD�̏��
	int m_CDState = CD_BIRTH;

	//�㏸�x
	float m_AddPower = 0.0f;
	//�d�͉����x
	float m_Gravity = 0.03f;
	//�L���b�`������̃|�W�V����
	XMFLOAT3 m_CatchPos = {};
	
	//���X�|�[������
	int m_ResPornTimer = {};

	//��ꂽ���h�E��
	bool m_BreakCD = false;

	//CD���L���b�`����
	enum CatchState {
		CATCH_SET,
		CATCH_MOVE,
		CATCH_END,
	};

	bool m_AttackSetCD = false;
	int m_CatchState = CATCH_SET;

	float m_HP = {};

	bool m_DeathMove = false;
	XMFLOAT3 m_BoundPower = {};

	int m_BoundCount = {};

	int m_DeathTimer = {};

	enum AudioState {
		AUDIO_SET,
		AUDIO_END,
	}_AudioState = AUDIO_SET;

	bool m_AudioPlay = false;

	enum EndState {
		END_SET,
		END_MOVE,
		END_FINISH
	}_EndState;

	int m_EndTimer = {};
};