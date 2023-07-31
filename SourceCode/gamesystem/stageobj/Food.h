#pragma once
#include "ObjCommon.h"
#include <Ghost.h>
//食べ物のクラス
class Food :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	Food();

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


	void CarryStart(Ghost* ghost);
private:
	//パーティクル
	void Particle();
	//当たり判定
	bool Collision();
	//食料生産
	void BirthFood();
	//食料削除
	void DeleteFood();
	//食べ物を運ばせる
	bool CarriedGhost();
	//
	bool FallSpawn();
public://getter setter
public:
	//gettersetter
	const bool& GetAlive() { return m_Alive; }
	const bool& GetLockOn() { return m_LockOn; }
	const bool& GetIsCarried() { return m_IsCarried; }
	const bool& GetLightSet() { return m_LightSet; }

	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetLockOn(const bool LockOn) { m_LockOn = LockOn; }
	void SetLimit(const bool Limit) { m_Limit = Limit; }
	void SetIsCarried(const bool IsCarried) { m_IsCarried = IsCarried; }
	void SetLightSet(const bool LightSet) { m_LightSet = LightSet; }

private:
	bool m_Alive = true;//生存フラグ
	bool m_Spawn = true;
	bool m_LockOn = false;
	bool m_IsCarried = false;
	int m_Timer = 0;
	
	bool m_Jump = false;
	float m_Degree = 0.0f;
	float m_SpawnTimer = 0.0f;
	float m_SpawnTimerMax = 15.0f;
	Ghost* m_ghost = nullptr;
	//制限時間を超えたかどうか
	bool m_Limit = false;
	//削除までの時間
	int m_DeleteTimer = 0;

	bool m_LightSet = false;
};
