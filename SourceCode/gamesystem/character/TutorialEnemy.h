#pragma once
#include"InterEnemy.h"
using namespace std;         //  ���O��Ԏw��
//���ʂ̓G
class TutorialEnemy :public InterEnemy {
public:
	TutorialEnemy();
	bool Initialize() override;//������
	void Finalize() override;//�J��
	void Action()override;//�X�V
	void ImGuiDraw()override;
	void Draw(DirectXCommon* dxCommon) override;//�`��


	void Born();
	bool GetIsActive() {return isActive; }

	void SetIsActive(bool flag) { isActive = flag; }
private:
	void Particle();//�p�[�e�B�N��

	enum class CommandState : int {
		SpawnState = 0,
		WaitState,
		LockOnState,
		JumpState,
	}commandState = CommandState::SpawnState;

	//�֐��|�C���^
	static void(TutorialEnemy::* commandTable[])();
	//�����o�֐�
	void SpawnUpdate();
	void WaitUpdate();
	void LockOnUpdate();
	void JumpUpdate();

	bool Death();
private:
	void GetRotation2Player();

	const float scale_ = 0.6f;

	float RottoPlayer = 0.0f;
	float moveTimer = 0.6f;
	float scaleCount = 0.05f;
	enum {
		addNum = 1,
		subNum = -1,
	};
	float move = addNum;

	float waitTimer = 0.0f;
	float commandTimer = 0.0f;
	float rot = 0.0f;
	int jumpCount = 1;
	const int kJumpCountMax = 3;

	float kWaitTimeMax = 150.0f;
	const float kSpawnTimeMax = 30.0f;
	float kLockOnTimeMax = 50.0f;
	float kJumpTimeMax = 60.0f;

	XMFLOAT3 s_pos = {}, e_pos = {};
};