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

	bool Initialize() override;//初期化

	void SkipInitialize() override;//スキップ時の初期化

	void Pause() override;//ポーズ

	void Action() override;//行動

	void EndRollAction() override;

	void AppearAction() override;//ボス登場の固有の処理

	void DeadAction() override;//ボス撃破の固有の処理

	void DeadAction_Throw() override;//ボス撃破の固有の処理 スロー

	void ImGui_Origin() override;//ボスそれぞれのImGui

	void InitAwake() override;//ボスの覚醒

	void EffecttexDraw(DirectXCommon* dxCommon) override;

	void Draw(DirectXCommon* dxCommon) override;//描画

	void AfterAction() override;//会話
private:
	void SetEasePos();
	void BirthParticle();
private:
	//CSV読み込み系
	void CSVLoad();
	//死んだときのパーティクル
	void DeathParticle();
	//登場シーンのパーティクル
	void AppParticle();
private:
	static const int CD_NUM = 4;
private:
	//各クラス
	unique_ptr<ConfuEffect> confueffect;
	unique_ptr<NoteEffect> noteeffect;
	//キャラの状態
	enum CharaState
	{
		STATE_INTER,
		STATE_CHOICE,
		STATE_ROCKON,
		STATE_RAND,
		STATE_HIT,
		STATE_END
	};

	//停止時間
	int m_StopTimer = 0;
	//どの行動にするか
	int m_MoveState = {};
	int m_ParticleTimer = {};

	int _charaState = STATE_INTER;

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
	//移動力
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

		//ガード続く時間
	float GuardTime;
		//ノックバック頻度
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


