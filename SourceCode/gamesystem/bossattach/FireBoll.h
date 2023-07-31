#pragma once
#include "ObjCommon.h"
#include "IKETexture.h"
#include "BreakEffect.h"
#include "InterBullet.h"
//火の玉クラス
class FireBoll :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	FireBoll();
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
	bool Collide();	//当たり判定

	void Move();

	void BirthParticle();

	void CollideBul(vector<InterBullet*>bullet);

	//エフェクト発生
	void BirthEffect();
public:
	//gettersetter
	const bool& GetAlive() { return m_Alive; }

	void SetCircleSpeed(const float CircleSpeed) { m_CircleSpeed = CircleSpeed; }
	void SetLimitTimer(const int LimitTimer) { m_LimitTimer = LimitTimer; }
	void SetDelete(const int Delete) { m_Delete = Delete; }
private:
	vector<InterEffect*> effects;
	unique_ptr<IKETexture> tex;
	XMFLOAT4 m_TexColor = { 1.0f,1.0f,1.0f,0.0f };
	XMFLOAT3 m_TexPos = {};
	XMFLOAT3 m_AfterPos = {};
	bool m_TexAlive = false;
	bool m_Alive = false;
	int m_MoveTimer = {};
	float m_CircleSpeed = {};
	float m_CircleScale = {};
	float m_Damage = {};
	float m_Frame = {};
	XMFLOAT3 m_TargetPos = {};
	float m_SinAngle = {};
	float m_SinAngle2 = {};
	enum MoveState {
		MOVE_SET,
		MOVE_PLAY,
		MOVE_OMEN,
		MOVE_ATTACK,
		MOVE_END,
	}_MoveState;

	int m_LimitTimer = 200;

	float m_HP = 1.0f;
	bool m_Delete = false;
};