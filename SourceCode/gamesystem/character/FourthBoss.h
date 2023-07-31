#pragma once
#include "IKESprite.h"
#include "InterBoss.h"
#include "Shake.h"
#include "IKETexture.h"
#include "TutorialEnemy.h"
#include "BossStunEffect.h"

class FourthBoss :
	public InterBoss {
public:
	FourthBoss();

	bool Initialize() override;//初期化

	void SkipInitialize() override;//スキップ時の初期化

	void Pause() override;//ポーズ

	void Action() override;//行動

	void AppearAction() override;//ボス登場の固有の処理

	void DeadAction() override;//ボス撃破の固有の処理

	void DeadAction_Throw() override;//ボス撃破の固有の処理 スロー

	void ImGui_Origin() override;//ボスそれぞれのImGui

	void InitAwake() override;//ボスの覚醒

	void EffecttexDraw(DirectXCommon* dxCommon) override;

	void Draw(DirectXCommon* dxCommon) override;//描画

	void EndRollAction() override;//エンドロールの更新

	void AfterAction() override;//会話
private:
	enum class commandState : int {
		WaitCommand = 0,
		MoveCommand,
		ControlCommand,
		EnemySpawn,
		SubGauge,
		Ultimate,
		Explosion,
		COMMANDMAX
	};
	void SelectAction();

	//関数ポインタ
	static void(FourthBoss::* stateTable[])();
	//メンバ関数
	void WaitUpdate();
	void MoveUpdate();
	void ControlUpdate();
	void EnemySpawnUpdate();
	void SubGaugeUpdate();
	void UltimateUpdate();
	void ExplosionUpdate();

	bool ShutterEffect();
	bool ShutterFeed();
	void ShutterReset();

	bool EnemysIsActiveCheck();
	void ChangePos2Random();
	void ChangePos2Rand();

	bool IsPinch();
private:
	static const int kPhotoSpotMax = 5;
	array<unique_ptr<IKETexture>, kPhotoSpotMax> photoSpot = {};

	static const int FourthEnemyMax = 5;
	array<unique_ptr<TutorialEnemy>, FourthEnemyMax>Fourthenemys = {};

	array<XMFLOAT3, kPhotoSpotMax> spotPos = {
		XMFLOAT3({-48,0,-55}),
		XMFLOAT3({58,0,-55}),
		XMFLOAT3({-48,0,55}),
		XMFLOAT3({58,0,55}),
		XMFLOAT3({0,0,0})
	};
	enum {
		Photo_In,
		Photo_In_Change,
		Photo_In_Control,
		Photo_In_Sub,
		Photo_In_Ult,
		Photo_Out_Top,
		Photo_Out_Under,
		SpriteMax,
	};
	array<unique_ptr<IKESprite>, SpriteMax> photo = {};
	//unique_ptr<BossStunEffect> bossstuneffect;

private:
	int moveSpawn = 0;
	int nowSpawn = 4;
	commandState phase = commandState::WaitCommand;

	int cases = 0;
	float limitHp = 0.0f;

	int stage_move = 0;
	int stage_move_count = 1;
	int stage_move_max = 4;

	bool isShutter = false;
	float shutterTime = 0.0f;
	float feedTimer = 0.0f;
	float shutterHight[2] = { 0,0 };
	//enum class commandState : int {
	//	WaitCommand = 0,
	//	MoveCommand,
	//	ControlCommand,
	//	EnemySpawn,
	//	SubGauge,
	//	Ultimate,
	//	COMMANDMAX
	//};
	float add = 5.0f;
	float subtimer = 0.4f;


	array<int, (size_t)commandState::COMMANDMAX> ActionTimerMax =
	{ 180,
		120,
		100,
		100,
		60,
		700,
		500
	};

	float shutterTimeMax = 40.0f;
	float feedTimeMax = 15.0f;

	//上昇度
	float m_AddPower = 0.0f;
	//重力加速度
	float m_Gravity = 0.03f;
};
