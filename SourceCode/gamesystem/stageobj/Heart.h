#pragma once
#include "ObjCommon.h"
//ハートクラス
class Heart :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	Heart();

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

private:
	void HeartJump();

	void HeartSet();

	void HeartVanish();
private://ステート
	static void (Heart::* stateTable[])();
private:
	//パーティクル
	void Particle();
	//当たり判定(プレイヤー)
	bool PlayerCollision();
public://getter setter
	const bool& GetAlive() { return m_Alive; }
private:
	//生存フラグ
	bool m_Alive = false;
	//生存時間
	int m_AliveTimer = {};
	//生存時間の制限
	int m_LimitTimer = {};

	bool m_Vanish = false;
private:
	enum HeartState {
		HEART_JUMP,
		HEART_SET,
		HEART_VANISH,
	}_heartState;

	XMFLOAT3 m_AddPower = {};
	float m_Gravity = 0.05f;

	float m_Frame = 0.0f;
};
