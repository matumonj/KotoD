#pragma once
#include "ObjCommon.h"
using namespace std;         //  名前空間指定

enum Bullettype {
	BULLET_FORROW,//追従
	BULLET_SEARCH,//探索
	BULLET_ATTACK,//攻撃
};

enum AttackState {
	POWER_NONE,
	POWER_MIDDLE,
	POWER_STRONG,
	POWER_UNLIMITED
};
//弾の基底クラス
class InterBullet :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	//初期化
	virtual bool Initialize() = 0;
	//更新
	void Update();
	
	//描画
	void Draw(DirectXCommon* dxCommon);

	virtual void Action() = 0;//弾特有の処理
	//ImGui既定
	void ImGuiDraw();
	//特有のImGui
	virtual void ImGui_Origin() = 0;

	virtual void Draw_Origin() = 0;
public:
	//gettersetter
	const bool& GetAlive() { return m_Alive; }
	const bool& GetInArea() { return m_InArea; }
	const bool& GetBossCatch() { return m_BossCatch; }
	const int& GetBulletType() { return m_BulletType; }
	const int& GetPowerState() { return m_PowerState; }
	const float& GetPower() { return m_DamagePower; }
	const float& GetRadius() { return m_Radius; }

	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetAngle(const XMFLOAT2& Angle) { m_Angle = Angle; }
	void SetBulletType(const int BulletType) { m_BulletType = BulletType; }
	void SetPowerState(const int PowerState) { m_PowerState = PowerState; }
	void SetCharge(const bool Charge) { m_Charge = Charge; }
	void SetInArea(const bool InArea) { m_InArea = InArea; }
	void SetBossCatch(const bool BossCatch) { m_BossCatch = BossCatch; }
	void SetTargetPos(const XMFLOAT3& TargetPos) { m_TargetPos = TargetPos; }
	void SetNumMag(const float NumMag) { m_NumMag = NumMag; }
public:
	XMFLOAT2 m_Angle = {};//弾の角度
	float m_AddSpeed = {};//加速度
	int m_BulletType = {};//弾の種類
	int m_Timer = {};//出現時間
	int m_TargetTimer = {};//出現時間の目標
	bool m_Alive = true;//生存フラグ
	bool m_Charge = false;//チャージしたかどうか

	int m_PowerState = POWER_NONE;
	float m_Power = {};//ダメージ量
	float m_DamagePower = {};

	//ボスが弾をキャッチしたか
	bool m_BossCatch = false;
	XMFLOAT3 m_TargetPos = {};

	bool m_InArea = false;

	float m_NumMag = 0.0f;

	//弾の当たり判定の半径
	float m_Radius = {};
};