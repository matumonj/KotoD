#pragma once
#include "InterBoss.h"
#include"IKEFBXObject3d.h"
#include "BarrangeCD.h"
#include "ConfuCD.h"
#include "LineCD.h"
#include "DebuffCD.h"
#include "AttackNote.h"
#include "DamageArea.h"
#include "ConfuEffect.h"
#include "NoteEffect.h"
class SixBoss :
	public InterBoss {
public:
	SixBoss();

	bool Initialize() override;//初期化

	void SkipInitialize() override;//スキップ時の初期化

	void Pause() override;//ポーズ

	void Action() override;//行動

	void AppearAction() override;//ボス登場の固有の処理

	void DeadAction() override;//ボス撃破の固有の処理

	void DeadAction_Throw() override;//ボス撃破の固有の処理 スロー

	void ImGui_Origin() override;//ボスそれぞれのImGui

	void EffecttexDraw(DirectXCommon* dxCommon) override;

	void Draw(DirectXCommon* dxCommon) override;//描画

	void InitAwake() override;//ボスの覚醒

	void EndRollAction() override;//エンドロールの更新

	void AfterAction() override;//会話
private:
	//インターバル
	void InterValMove();
	//動きの選択
	void Choice();
	//ダメージエリアのセット
	void LineSet();
	//プレイヤーのデバフ
	void Debuff();
	//混乱
	void Confu();
	//弾幕
	void Barrage();
	//行動終わり
	void EndMove();
	//CSV読み込み系
	void CSVLoad();
	//ノーツの生成
	void BirthNote(const std::string& BarrageName);
	//死んだときのパーティクル
	void DeathParticle();
private:
	static const int BULLET_NUM = 2;
	static const int CD_NUM = 4;
private:
	//各クラス
	array<unique_ptr<InterCD>, CD_NUM> cd;
	vector<AttackNote*> attacknotes;//弾幕
	unique_ptr<DamageArea> damagearea;//ダメージエリア
	unique_ptr<ConfuEffect> confueffect;
	unique_ptr<NoteEffect> noteeffect;
	//キャラの状態
	enum CharaState
	{
		STATE_INTER,
		STATE_CHOICE,
		STATE_LINE,
		STATE_DEBUFF,
		STATE_CONFU,
		STATE_BARRA,
		STATE_END
	};

	//停止時間
	int m_StopTimer = 0;
	//どの行動にするか
	int m_MoveState = {};

	//関数ポインタ
	static void(SixBoss::* stateTable[])();

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

	enum CDType {
		CD_LINE,
		CD_DEBUFF,
		CD_CONFU,
		CD_BARRA,
	};
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
};
