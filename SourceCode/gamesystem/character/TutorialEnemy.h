#pragma once
#include"InterEnemy.h"
using namespace std;         //  名前空間指定
//普通の敵
class TutorialEnemy :public InterEnemy {
public:
	TutorialEnemy();
	bool Initialize() override;//初期化
	void Finalize() override;//開放
	void Action()override;//更新
	void ImGuiDraw()override;
	void Draw(DirectXCommon* dxCommon) override;//描画


	void Born();
	bool GetIsActive() {return isActive; }

	void SetIsActive(bool flag) { isActive = flag; }
private:
	void Particle();//パーティクル

	enum class CommandState : int {
		SpawnState = 0,
		WaitState,
		LockOnState,
		JumpState,
	}commandState = CommandState::SpawnState;

	//関数ポインタ
	static void(TutorialEnemy::* commandTable[])();
	//メンバ関数
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