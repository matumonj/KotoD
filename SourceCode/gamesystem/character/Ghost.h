#pragma once
#include "ObjCommon.h"
#include "InterBullet.h"
#include "AttackNote.h"
#include "CollisionPrimitive.h"
//ゴーストクラス
class Ghost :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	Ghost();

	bool Initialize() override;//初期化
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(DirectXCommon* dxCommon) override;

	/// <summary>
	/// ImGui描画
	/// </summary>
	void ImGuiDraw();
	//探索のはじめ
	void StartSearch(const XMFLOAT3& pos);
	//探索の終わり
	void EndSearch();

private:
	void Particle();
	//当たり判定(プレイヤー)
	bool PlayerCollision();
	//食料生産
	void BirthGhost();
	//生成可能かを調べます
	bool VerseCheck();
private://ステート
	static void (Ghost::* stateTable[])();
	//何もない状態
	void None();
	//吸収
	void Absorption();
	//操られている
	void Manipulate();
	//生まれる状態
	void Spawm();
	//追従
	void Follow();
	//探索
	void Search();
	//闇落ち
	XMFLOAT3 bossPos;
	bool Collide;
	int DarkCount;
	bool stateSpawn;
	bool DarkOtiClean;
	void DarkSide();
	//洗脳
	void Jack();
	//洗脳
	void HyperJack();
	void GetRotation2Player();
	//消える
	void Vanish();
	//食べ物を運ぶ
	void CarryFood();
	//
	bool CollideBullet(vector<InterBullet*>bullet);

	void BirthBullet();
public:
	bool GetStateSpawn() { return stateSpawn; }
	void SetStateSpawn(bool state) { stateSpawn=state; }
	bool JugNONE() { if (_charaState == STATE_NONE)return true; else return false; }
	bool GetCollide() { return Collide; }
	void SetCollide(bool col) { Collide = col; }
	void SetFivePos(XMFLOAT3 pos) { bossPos = pos; }

	//ゴースト同士の当たり判定
	void GhostCollision(const XMFLOAT3& pos);

	void DeleteBullet();
public://getter setter
	const bool& GetAlive() { return m_Alive; }
	const bool& GetIsRefer() { return m_IsRefer; }
	const bool& GetCatch() { return m_Catch; }
	const bool& GetVanish() { return m_Vanish; }
	const bool& GetFollow() { return m_Follow; }
	const bool& GetDFollow() { return m_DFollow; }
	const bool& GetAbsorption() { return m_Absorption; }
	const bool& GetSearch() { return m_Search; }
	const bool& GetIsVerse() { return isVerse; }
	const bool& GetManipulate() { return m_Manipulate; }
	const bool& GetIsPostionCheck() { return m_IsPostionCheck; }
	const float& GetLimit() { return m_Limit; }
	const int GetStateInst() { return (int)_charaState; }
	const XMFLOAT3& GetTargetPos() { return m_TargetPos; }

	void SetIsRefer(const bool isRefer) { this->m_IsRefer = isRefer; }
	void SetIsHyperRefer(const bool isRefer) { this->m_IsHyperRefer = isRefer; }

	void SetIsVerse(const bool isVerse, int verseCureTimer = 0) { this->isVerse = isVerse; m_VerseCureTimer = verseCureTimer; }
	void SetManipulate(const bool Manipulate) { m_Manipulate = Manipulate; }
	void SetCatch(const bool Catch) { m_Catch = Catch; }
	void SetAbsorption(const bool Absorption) { m_Absorption = Absorption; }
	void SetVanish(const bool Vanish) { m_Vanish = Vanish; }
	void SetIsPostionCheck(const bool m_IsPostionCheck) { this->m_IsPostionCheck = m_IsPostionCheck; }
	void SetIsAllPostionCheck(const bool m_IsPostionCheck) { this->m_IsAllPostionCheck = m_IsPostionCheck; }

	void SetTargetPos(const XMFLOAT3& TargetPos) { m_TargetPos = TargetPos; }
	void SetLimit(const float Limit) { m_Limit = Limit; }
private:
	vector<AttackNote*> attacknotes;//弾幕
	bool m_Alive = true;//生存フラグ
	bool m_Catch = false;//捕獲フラグ
	bool isVerse = true;//リスポーンフラグ
	int m_VerseCureTimer = 0;//リスポーン回復フラグ
	int m_ResPornTimer = 0;//復活の時間
	XMFLOAT3 m_FollowPos = {};//追従先
	XMFLOAT3 m_BirthPos;
	XMFLOAT3 m_OBBScale = {};//OBB用の大きさ
public:
	//キャラの状態
	enum CharaState {
		STATE_NONE,
		STATE_SPAWN,
		STATE_SEARCH,
		STATE_FOLLOW,
		STATE_DARKOTI,
		STATE_JACK,
		STATE_HYPERJACK,
		STATE_MANIPULATE,
		STATE_VANISH,
	}_charaState = CharaState::STATE_NONE;

	bool GetCleanghost() { return DarkOtiClean; }
	void SetCleanGhost(bool f) { DarkOtiClean = f; }

	XMFLOAT3 GetBirthPos() { return m_BirthPos; };

	bool CollideDarkBul(OBB obb);
	CharaState GetState() { return _charaState; }
private:
	unique_ptr<IKEModel> model_follow = nullptr;
	unique_ptr<IKEModel> model_seach = nullptr;

	XMFLOAT3 m_OldPos = {};
	bool m_Follow = false;
	//追従状態
	enum FollowState {
		Follow_NO,
		Follow_START,
		Follow_END,
	}_followState = FollowState::Follow_NO;
	//探索状態
	enum SearchState {
		SEARCH_NO,
		SEARCH_START,
		SEARCH_END,
	}_searchState = SearchState::SEARCH_NO;

	int m_SearchTimer = 0;
private://探索
	bool m_Search = false;
	XMFLOAT3 m_SearchPos = {};

	float noneTimer = 0.0f;

	float m_SpawnTimer = 0.f;
	float kSpawnTimerMax = 60.f;

	float flash = 0.0f;
	float flashVel = 0.1f;
	enum {
		add=1,
		sub=-1
	};
	float flashAdd = 1.0f;
	bool m_IsRefer = false;
	bool m_IsHyperRefer = false;
	bool m_IsPostionCheck = false;
	bool m_IsAllPostionCheck = false;
	float RottoPlayer = 0.0f;
	XMFLOAT3 f_pos = {};
	float m_radius = 0.0f;
	enum {
		addDir = 1,
		subDir = -1,
	};
	float m_dir = 1.0f;
	float m_angle = 0.0f;
	//消える
	bool m_Vanish = false;
	float m_Frame = 0.0f;

	bool m_Absorption = false;
	XMFLOAT3 m_TargetPos = {};
private:
	//探索するものの範囲
	float m_Limit = {};

	bool m_Hit = false;
	bool m_DFollow;
	OBB m_OBB1 = {}, m_OBB2 = {};

	//操られている
	bool m_Manipulate = false;

	enum ManiState {
		MANI_SET,
		MANI_MOVE,
		MANI_ATTACK,
		MANI_END,
	}_ManiState;

	XMFLOAT3 m_AfterPos = {};
	float m_AfterRotY = {};

	int m_RotTimer = {};

	int m_DarkC;
};
