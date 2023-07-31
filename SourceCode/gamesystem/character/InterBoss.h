#pragma once
#include "IKETexture.h"
#include "CollisionPrimitive.h"
#include "InterBullet.h"
#include "ObjCommon.h"
#include "BreakEffect.h"
#include "InterEnemy.h"
#include"Ghost.h"

using namespace std;         //  名前空間指定

//ボスの基底クラス
class InterBoss :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	enum FourthBossInst {
		None = 0,
		StopGhost,
		ChangeGhost,
		SpawnEnemy,
		AllSummon,
		FinishMove,
		DELETEGHOST,
	};
public:
	//初期化
	virtual bool Initialize() = 0;
	//更新
	void Update();
	//ボス登場シーンの更新
	void AppearUpdate();
	//ボス撃破シーンの更新
	void DeadUpdate();
	//エンドロールの更新
	void EndRollUpdate();
	virtual void SkipInitialize() = 0;
	/// <summary>
	/// ポーズのとき
	/// </summary>
	virtual void Pause() = 0;
	//描画
	virtual void Draw(DirectXCommon* dxCommon)override;

	void ImGuiDraw();//ImGuiの描画

	virtual void EffecttexDraw(DirectXCommon* dxCommon) = 0;

	void AwakeUpdate();
	//ボス戦後の更新
	void AfterUpdate();
protected:

	virtual void Action() = 0;//ボス特有の処理

	virtual void EndRollAction() = 0;//エンドロールの処理

	virtual void AppearAction() = 0;//ボス登場の固有の処理

	virtual void DeadAction() = 0;//ボス撃破の固有の処理

	virtual void DeadAction_Throw() = 0;//ボス撃破の固有の処理 スロー

	virtual void ImGui_Origin() = 0;//ボスそれぞれのImGui

	virtual void InitAwake() = 0;//覚醒シーンの初期化

	virtual void AfterAction() = 0;//バトル後の会話

private:
	void BirthEffect();
public:
	void SummonEnemyUpda(std::vector<InterEnemy*> boss);
	void SummonEnemyDraw(std::vector<InterEnemy*> boss, DirectXCommon* dxcomn);
	void EndSummon(std::vector<InterEnemy*> boss);
	void isRespawn(std::vector<InterEnemy*> boss);

	void SpawnSetEnemy(vector<unique_ptr<InterEnemy>> enemys);

protected:
	bool SummonF;
	bool SummobnStop;
	int SummonCool;
	bool SearchF;
	int m_BirthTarget=0;
	bool statereset_ = false;
	// = Collision::GetLength(m_Position, l_player) < 25.f;
private:
	XMFLOAT3 EffectFirstPos;
public://gettersetter
	bool GetIsAlive() { return isAlive; }

	void SetHP(float hp) { m_HP = hp; };
	float GetHP() { return m_HP; }

	void SetLimit(float limit) { m_Limit = limit; };
	float GetLimit() { return m_Limit; }

	void SetStrong(bool strong) { isStrong = strong; };
	bool GetStrong() { return isStrong; }

	void SetSearch(bool isSearch) { this->isSearch = isSearch; };
	bool GetSearch() { return isSearch; }

	void SetIsMiss(bool isMiss) { this->isMiss = isMiss; };

	void SetHyperSearch(bool isSearch) { this->isHyperSearch = isSearch; };
	bool GetHyperSearch() { return isHyperSearch; }

	void SetInstruction(int isInstruction) { this->isInstruction = isInstruction; };
	int GetInstruction() { return isInstruction; }

	float HpPercent();

	void SetCheck(bool Check) { m_Check = Check; };
	bool GetCheck() { return m_Check; }

	void SetAbsorption(bool Absorption) { m_Absorption = Absorption; };
	bool GetAbsorption() { return m_Absorption; }

	void SetManipulate(bool Manipulate) { m_Manipulate = Manipulate; };
	bool GetManipulate() { return m_Manipulate; }

	void SetFinishApp(bool FinishApp) { m_FinishApp = FinishApp; };

	void SetDeleteObj(bool DeleteObj) { m_DeleteObj = DeleteObj; };

	bool GetFinishAppear() { return m_FinishAppear; }

	bool GetBirthHeart() { return m_BirthHeart; }
	void SetBirthHeart(bool HeartBirth) { m_BirthHeart = HeartBirth; };

	void SetDirEmo(int DirEmo) { m_DirEmo = DirEmo; };

	void SetAvatarType(int AvatarType) { m_AvatarType = AvatarType; };

	void SetJackPos(const int num, XMFLOAT3 pos) { jackPos[num] = pos; };

	void SetIsReferCheck(bool isReferCheck) { this->isReferCheck = isReferCheck; };
	bool GetIsReferCheck() { return isReferCheck; }

	int ActionNum=0;
	void SetCircleSpeed(float CircleSpeed) { m_CircleSpeed = CircleSpeed; }
	void SetTargetPos(XMFLOAT3 TargetPos) { m_TargetPos = TargetPos; }

	void SetStateReset(bool flag) { statereset_ = flag; }
protected:
	std::vector<Ghost*> ghosts;
	std::string SceneName;
	vector<InterEffect*> effects;
protected:
	XMFLOAT3 m_TargetPos = {};
	
	//ダメージ食らったとの色変換
	float ColChangeEaseT;
	int ActionTimer;

	int m_CheckTimer = {};

	int ActionDamage;
	int ActionCool;

	bool isAlive = true;
	//ゴーストがフィールドにいない
	bool isMiss = false;
	float m_HP = {};
	float m_MaxHp = {};
	float m_Limit = 20.0f;
	bool isStrong = false;
	//ゴーストを5たいサーチ
	bool isSearch = false;
	//敵を全てサーチします。
	bool isHyperSearch = false;
	//ゴーストを削除しスポーンを止めます。
	int isInstruction = 0;
	//ゴーストの状態がSearch状態かしれます
	bool isReferCheck = false;
	bool m_Check = false;
	XMFLOAT3 m_OBBScale = {};

	//弾とボスの当たり判定に使う大きさ
	float m_Radius = 0.0f;

	//登場シーンが終わったかどうか
	bool m_FinishAppear = false;

	//どっちの顔を向けているか
	int m_DirEmo = {};
	bool m_FinishApp = false;

	int m_DeathTimer = 0;

	bool m_BirthHeart = false;

	//ダメージ倍率
	float m_Magnification = {};
	//上昇度
	float m_AddPower = 0.0f;
	//重力加速度
	float m_Gravity = 0.02f;

	//
	array<XMFLOAT3, 5> jackPos = {};

	//吸収してるか
	bool m_Absorption = false;
	//操る
	bool m_Manipulate = false;

	//円運動
	float m_CircleScale = 30.0f;
	float m_CircleSpeed = {};
	
	bool m_DeleteObj = false;

	//覚醒初期化
	bool m_AwakeInit = false;
	//出現シーンのタイマー
	int m_AppearTimer = {};
	//死亡シーンのタイマー
	int m_DeadTimer = {};
	//分身のタイプ
	int m_AvatarType = {};
	//エンドロールのタイマー
	int m_EndTimer = {};


	enum EndState {
		END_SET,
		END_WALK,
		END_DIR_CAMERA,
	}_EndState = END_SET;

	bool m_EndStop = false;

	bool m_Slow = true;
private:

	enum class ActionList {
		NON,
		NORMAL,
		MOB_A,
		MOB_B,
		FRONT
	}_action;

	struct DefaultParam {
		float Cool;
		int Damage;
		std::vector<float>ParSize;
		bool Shake;
	};
protected:
	//弾との当たり判定
	enum class Type {
		CIRCLE,
		SPHERE
	};
	void CollideBul(vector<InterBullet*>bullet, Type type = Type::SPHERE);
	//当たり判定(プレイヤー)
	void ColPlayer();
	bool EndSummonRepos;
	bool ResF;
public:
	int GhostSize;
	bool Recv;
	bool CanAttack;
	int GetGhostSize() { return GhostSize; }
	void SetGhostSize() { GhostSize++; }
	bool GetRecv() { return Recv; }
	void SetRecv(bool f) { Recv = f; }
	bool GetCanAttack() { return CanAttack; }
	IKEFBXObject3d* GetFbxobj() { return fbxmodel.get(); }

	std::unique_ptr<IKEFBXObject3d>fbxmodel;
	enum AnimeNames
	{
		IDLE,
		SHOT,
		WALK
	}_animeName;
	void AnimationControl(AnimeNames name, const bool& loop, int speed);

	int NextActionInteval;
	bool IdleRota;

	bool GetIdleRotaF() { return IdleRota; }
protected:
	bool DeathSceneF;
	void DeathAction();
	void DeathAction_Throw();
	bool ThrowUpdateF;

public:
	bool GetDeathAction() { return DeathSceneF; }
	void SetThrowUpdateF(bool f) { ThrowUpdateF = f; }
	

	inline void SetGhostList(std::vector<Ghost*>gsize)
	{
		ghosts.resize(gsize.size());
		for(auto i=0;i<gsize.size();i++)
		{
			ghosts[i] = gsize[i];
		}
	};
	std::vector<Ghost*>GetGhost() { return ghosts; }

};

