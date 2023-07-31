#pragma once
#include "ObjCommon.h"
#include "InterBullet.h"
enum SetType {
	SET_FOLLOW,
	SET_DIFF,
};

//ボスが生む攻撃音符クラス
class AttackNote :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	AttackNote();

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

	bool Collide();	//当たり判定
	
private://ステート
	static void (AttackNote::* stateTable[])();
private:
	void Particle();
	void Follow();
	void Diffusion();
public:

public://getter setter
public:
	//gettersetter
	const bool& GetAlive() { return m_Alive; }
	void SetType(const int SetType) { m_SetType = SetType; }
	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetChange(const bool Change) { m_Change = Change; }
	void SetAngle(const XMFLOAT2& Angle) { m_Angle = Angle; }
private:
	bool m_Alive = true;//生存フラグ
private:
	//キャラの状態
	enum CharaState
	{
		STATE_FOLLOW,
		STATE_DIFF,
	}_charaState = CharaState::STATE_FOLLOW;

private:
	IKEModel* m_Model2;
	XMFLOAT2 m_Angle = {};//弾の角度
	float m_AddSpeed = {};//加速度


	int m_SetType = SET_FOLLOW;

	int m_TargetTimer = {};//出現時間の目標

	int m_Timer = 0;

	float m_Damage = {};

	bool m_Change = false;
};
