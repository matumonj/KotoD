#pragma once
#include "InterBoss.h"
#include "JoyStamp.h"
#include "AngerStamp.h"
#include "ShockWave.h"
#include "Predict.h"
#include "Collision.h"
#include "Shake.h"
#include "ConfuEffect.h"
#include "NoteEffect.h"
#include"Fraction.h"


class Spline;

class FirstBoss :
	public InterBoss
{
private:
	enum AttackFase {
		AttackBefore=0,
		AttackAfter,
	}fase_;

	enum ActionFase {
		Before=0,
		After,
	}act_;

	enum MoveFase {
		Move=0,
		Stop,
	}move_;

	enum BottleState {
		NORMAL,
		FRACTION,
	}bottlestate_;

	enum Bounce {
		UP=0,
		DOWN,
		SOURCE,
	}bounce_;
public:
	FirstBoss();

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
	//インターバル
	void InterValMove();
	//動きの選択
	void Choice();
	//ロックオン突進
	void RockOnAttack();
	//ランダム
	void DropAttack();
	//当たり屋?
	void Hit();

	void RockOn();

	void Attack();

	void FaceToOrientation();

	void CreateFraction(const XMFLOAT3& DropPos,const XMFLOAT3& BossPos);

	void Bounce();

	void Areia();

	void FractionRockOn();

	void Big();
private:

	//死んだときのパーティクル
	void DeathParticle();
public:

	bool  InterChange() { if (_charstate == CharaState::STATE_INTER) { true; } }

	float GetHp() { return m_HP; }

private:
	static const int BULLET_NUM = 4;
	static const int CD_NUM = 4;
private:

	//各クラス
	unique_ptr<ConfuEffect> confueffect;
	unique_ptr<NoteEffect> noteeffect;
	std::list<std::unique_ptr<Fraction>> fraction_;
	vector<XMFLOAT3> m_Area;
	unique_ptr<IKETexture> tex;//エリア

	//キャラの状態
	enum CharaState
	{
		STATE_INTER = 0,
		STATE_CHOICE,
		STATE_ROCKON,
		STATE_RAND,
		STATE_HIT,
		STATE_BIG,
		STATE_END
	}_charstate;

	enum RockonState {
		STATE_AIM=0,
		STATE_ATTACK,
	}_rockonstate;

	//停止時間
	int m_StopTimer = 0;
	//どの行動にするか
	int m_MoveState = {};

	//関数ポインタ
	static void(FirstBoss::* stateTable[])();

	//CSV系
	int m_ChoiceInterval = {};

	//イージング後の位置
	XMFLOAT3 m_AfterPos = {};
	//X方向の回転
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

	//動きのインターバル
	int m_MoveInterVal = {};
	//行動終了の数
	int m_EndCount = {};
	//キャッチしたCDの数
	int m_CatchCount = {};
	//ボスがプレイヤーから逃げる時間
	int m_EndTimer = {};

	//棘の的に使う
	float m_Angle = 0.0f;
	float m_Angle2 = 0.0f;
	//二点の距離
	float m_Length = {};

	//円運動
	float m_CircleScale = 30.0f;
	float m_CircleSpeed = {};

	//弾幕の種類
	int m_BarraRand = {};

	int m_AttackRand = {};

	//CSV系
	//各インターバルやリミット時間
	vector<int>m_Limit;

	enum LimitState {
		LIMIT_BASE,
		LIMIT_CONFU,
		LIMIT_STRONG_CONFU,
		LIMIT_BARRA,
	};

	//移動力
	float m_FollowSpeed = {};

	float timer_ = 0.f;
	float RottoPlayer = 0.0f;
	float commandTimer = 0.0f;
	float bounceTimer = 0.f;
	float kLockOnTimeMax = 50.0f;
	float rot = 0.0f;
	int jumpCount = 0;
	int waitCount = 0;
	XMFLOAT3 s_pos = {}, e_pos = {};
	float kJumpTimeMax = 60.0f;
	float kMoveTimeMax = 180.f;
	const int kJumpCountMax = 3;

	float fraction_timer_ = 0.f;

	int attack_count_ = 0;

	int jump_count_ = 0;
	float m_SinAngle = {};
	float m_SinAngle2 = {};

	float s_posY{};
	float e_posY{};

	//テクスチャ関連
	XMFLOAT3 m_TexPos = {};
	XMFLOAT3 m_TexRot = {};
	XMFLOAT3 m_TexScale = {};
	XMFLOAT4 m_TexColor = { 1.0f,1.0f,1.0f,0.0f };
	float m_Alpha = 0.0f;
	float m_AfterAlpha = 1.0f;
	float m_ActionTimer = 0.f;
	bool Display = false;

	float beforeTimer = 0.f;
	float half_hp_{};
	float DecisionCount = 0.f;

	bool movie = false;


	float returntimer_ = 0.f;
	float returnrot_ = 0.f;

	float bigtimer_ = 0.f;

	XMFLOAT3 e_scl{ 15.3f,15.3f,15.3f };
};


