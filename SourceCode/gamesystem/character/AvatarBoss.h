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
	void CSVLoad();

	//各ボスの行動
	void InterValMove();//インターバル
	void Polter();//ポルターガイスト
	void ThrowBound();//バウンド弾
	void BlockAttack();//ダメージブロックの生成
	void BirthBlock();
	void BirthPolter(const std::string& PolterName);//ポルターガイストの生成
	void Vanish();
	void BirthExplosion();
	//アバターのタイプ
	void AvatarNormal();
	void AvatarAround();
	void AvatarRight();
	void AvatarLeft();
public:
private:
	//キャラの状態
	enum CharaState
	{
		STATE_INTER,
		STATE_POLTER,
		STATE_BOUND,
		STATE_BLOCK,
		STATE_VANISH,
	}_charaState;

	//関数ポインタ
	static void(AvatarBoss::* stateTable[])();
	static void(AvatarBoss::* avatarTable[])();
private:
	static const int POLTER_NUM = 2;
	static const int BLOCK_NUM = 3;
private:
	unique_ptr<IKETexture> tex;
	vector<Poltergeist*> poltergeist;//ポルターガイスト
	vector<DamageBlock*> damageblock;//ダメージブロック
	int m_InterVal = {};

	int m_MoveTimer = {};

	//攻撃の乱数
	int m_AttackRand = {};

	//イージング後の位置
	XMFLOAT3 m_AfterPos = {};

	bool m_Return = false;

	bool m_Tackle = false;

	enum ReturnState {
		RETURN_SET,
		RETURN_PLAY,
		RETURN_END,
	}_ReturnState;
	//透明化する時間
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

	//テクスチャ関連
	XMFLOAT3 m_TexPos = {};
	XMFLOAT3 m_TexRot = {};
	XMFLOAT3 m_TexScale = {};
	XMFLOAT4 m_TexColor = { 1.0f,1.0f,1.0f,0.0f };
	float m_Alpha = 0.0f;
	float m_AfterAlpha = 0.0f;

	bool m_TexAlive = false;
};
