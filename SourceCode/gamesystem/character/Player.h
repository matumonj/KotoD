#pragma once
#include "ObjCommon.h"
#include "ViewBullet.h"
#include "GhostBullet.h"
#include "AttackBullet.h"
#include "PlayerAttach.h"
#include "BreakEffect.h"
#include "AbsorptionEffect.h"
#include <any>
using namespace DirectX;
class Player :public ObjCommon
{
public:
	static Player* GetInstance();

private:
	int index;
	int index2;
	static void (Player::* stateTable[])();
public:
	void InitState(const XMFLOAT3& pos);
	void LoadResource();
	//初期化
	bool Initialize()override;
	//更新
	void Update()override;
	//描画
	void Draw(DirectXCommon* dxCommon)override;
	//ImGui
	void ImGuiDraw();
	//ボス登場シーンの動き
	void AppearUpdate();
	//ボス撃破シーンの動き(フェード前)
	void DeathUpdate();
	//フェードあと
	void DeathUpdateAfter(int Timer);
	//ダークコトコ登場シーンの動き
	void DarkAppearUpdate(int Timer);
	//ラスボス登場シーンの動き
	void LastAppearUpdate(int Timer);
	//ラスボス撃破シーンの動き
	void LastDeadUpdate(int Timer);
	//エンドロールの更新
	void EndRollUpdate(int Timer);
	//タイトルの更新
	void TitleUpdate();
	//キャラの状態
	enum CharaState
	{
		STATE_IDLE,
		STATE_RUN,
	}_charaState;
	unique_ptr<IKEObject3d>skirtobj;
	unique_ptr<IKEObject3d>sutoobj;
	XMMATRIX skirtmat;
	XMMATRIX sutomat;

	std::unique_ptr<IKEFBXObject3d>fbxmodels;
private:
	void LoadCSV();
private:
	//歩きまたは走り状態
	float velocity;
	//移動方向指定用
	float angle;
	//移動加算値
	float m_AddSpeed;
	//↑の値をCSVから読み込むときの格納用
	std::any sp;

	//あるき処理
	void Walk();
	XMFLOAT3 MoveVECTOR(XMVECTOR v, float angle);

private:
	//弾の生成
	void BirthShot(const std::string& bulletName, bool Super);
private:
	void Idle();
	//インターバル管理
	void InterVal();
	//弾のリセット
	void ResetBullet();
	//弾の管理
	void Bullet_Management();
	//弾の更新
	void BulletUpdate(std::vector<InterBullet*> bullets);
	//弾の描画
	void BulletDraw(std::vector<InterBullet*> bullets, DirectXCommon* dxCommon);
	//銃の処理
	void SutoponUpdate();
	//ダメージパーティクル
	void BirthParticle();
	//パラメーターセット
	void SetParam();
	//吸収エフェクトの生成
	void BirthAbs(const XMFLOAT4& color);
private:
	//各アニメーション
	enum class AnimeName
	{
		IDLE=0,
		ATTACK=1,
		WALK=2,
	}_animeName;

	void AnimationControl(AnimeName name, const bool& loop, int speed);
public:
	//当たり判定系
	bool PlayerCollide(const XMFLOAT3& pos);//プレイヤーとの当たり判定
	//プレイヤーの当たった瞬間
	void PlayerHit(const XMFLOAT3& pos);
	//弾かれる処理
	void ReBound();
	//
	float GetPercentage();
	//覚醒シーンの初期化
	void AwakeInit();
public:
	//gettersetter
	const int& GetBulletType() { return m_BulletType; }
	const int& GetChargeType() { return m_ChargeType; }
	const int& GetDamageInterVal() { return m_DamageInterVal; }
	const bool& GetIsShotNow() { return isShotNow; }
	const bool& GetSkip() { return m_Skip; }
	const bool& GetConfu() { return m_Confu; }

	void SetHP(float hp) { m_HP = hp; };
	void SetCanSearch(bool CanSearch) { m_CanSearch = CanSearch; };
	float GetHP() { return m_HP; }
	float GetMaxHP() { return m_MaxHP; }

	void SetHitPlayer(bool HitPlayer) { m_HitPlayer = HitPlayer; };
	void SetCanShot(const bool m_canShot) {this->m_canShot = m_canShot;}
	void SetConfu(const bool Confu) { this->m_Confu = Confu; }
	void SetConfuTimer(const int ConfuTimer) { this->m_ConfuTimer = ConfuTimer; }
	void SetSkip(const bool Skip) { this->m_Skip = Skip; }

private://各クラス
	vector<InterBullet*> ghostbullets;//言霊
	vector<InterBullet*> attackbullets;//攻撃
	unique_ptr<InterBullet> viewbullet;//可視化の弾
	unique_ptr<PlayerAttach> playerattach;//プレイヤーの装備
	vector<InterEffect*> effects;//エフェクト
	vector<AbsorptionEffect*> abseffect;//吸収
	//弾関係の変数
	int m_BulletType = {};//弾の種類
	int m_InterVal = {};//弾の発射のインターバル
	int m_RigidityTime = {};//硬直時間
	int m_TargetInterVal = {};//インターバルの目標時間
	int m_TargetRigidityTime = {};//硬直時間の目標時間
	//体力
	float m_HP = 0.0f;
	float m_MaxHP = 0.0f;

	XMFLOAT3 m_FollowPos = {};

	float m_ChargePower = {};//弾のチャージ
	int m_ChargeType = {};
	bool m_canShot = true;
	//ダメージ
	bool m_Damage = false;
	int m_DamageInterVal = {};

	XMFLOAT2 m_BoundPower = {};

	bool TriggerAttack;
	//弾の数
	int m_BulletNum = 0;

	bool m_Confu = false;
	int m_ConfuTimer = 0;

	bool m_Skip = false;

	//飢餓ゲージを減らす
	bool m_SubHunger = false;
	float m_Frame = 0.0f;

	float m_LimitHunger = {};

	bool m_CanSearch = true;

	int m_ChangeLimit = 0;

	//CSV系
	//弾の強さのリミット
	vector<float>m_PowerLimit;

	bool m_HitPlayer = false;

	//ラスボスのときの動き
	enum DarkState {
		DARK_SET,
		DARK_WALK,
		DARK_SECOND_WALK,
		DARK_STOP,
	}_DarkState;

	//ラスボスのときの動き
	enum LastState {
		LAST_SET,
		LAST_WALK,
		LAST_SECOND_WALK,
		LAST_STOP,
	}_LastState;

	enum EndState {
		END_SET,
		END_WALK,
		END_DIR_CAMERA,
		END_DIR_RIGHT,
		END_DIR_LEFT,
		END_DIR_BACK,
		END_DIR_FRONT,
	}_EndState = END_SET;

	enum LastEndState {
		LAST_END_SET,
		LAST_END_WALK,
		LAST_END_DIR,
	}_LastEndState = LAST_END_SET;

	bool m_viewBull = false;
public:
	vector<InterBullet*>GetBulllet_ghost() { return ghostbullets; }
	vector<InterBullet*>GetBulllet_attack() { return attackbullets; }

private:
	bool isStop;
	bool isShotNow=false;
	XMFLOAT3 OldPos;
public:
	void MoveStop(bool f) { isStop = f; }
	void isOldPos();

	void RecvDamage(float Damage);
	//弾の全削除
	void BulletDelete();

private:
	static const int ABS_NUM = 3;
private:
	bool m_Birthabs[ABS_NUM];
};