#pragma once
#include "ObjCommon.h"
using namespace std;         //  名前空間指定

//弾の基底クラス
class PlayerAttach :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	PlayerAttach();
	//初期化
	bool Initialize()override;
	//更新
	void Update()override;
	//描画
	void Draw(DirectXCommon* dxCommon)override;
	//ImGui既定
	void ImGuiDraw();
	//パーティクル
	void Particle();
	//弾の出現
	void BirthObj();
	//登場シーンの動き
	void AppearUpdate(int Timer);
	//ダークコトコの登場シーン
	void DarkAppear(int  Timer);
	//撃破シーンの動き
	void LastDeadUpdate(int Timer);
	//エンドロールの動き
	void EndRollUpdate(int Timer);
	void DeathUpdate();
	//タイトル
	void TitleUpdate();
public:
	//gettersetter
	void SetAlive(const bool Alive) { m_Alive = Alive; }
	void SetAngle(const XMFLOAT2& Angle) { m_Angle = Angle; }
	//void SetAfterRot(const float AfterRotY) { m_AfterRotY = AfterRotY; }
private:
	XMFLOAT2 m_Angle = {};//弾の角度
	//float m_AfterScale = 0.0f;
	float m_AddSpeed = {};//加速度
	bool m_Alive = false;
	bool m_Particle = false;
	int m_ParticleTimer = 0;
	int m_AliveTimer = 0;
	float m_SinAngle = {};
	float m_SinAngle2 = {};
	float m_Frame = 0.0f;

	//登場シーン
	enum AppearState {
		APPEAR_SET,
		APPEAR_WALK,
		APPEAR_WALK2,
		APPEAR_STOP,
	}_AppearState;

	//撃破シーンの変数
	enum DeathState {
		DEATH_SET,
		DEATH_MOVE,
		DEATH_TALK,
		DEATH_NO_TALK,
	}_DeathState;
	XMFLOAT3 m_AfterPos = {};


	enum EndState {
		END_SET,
		END_WALK,
		END_DIR_CAMERA,
	}_EndState = END_SET;

	bool m_Rot = false;
	float m_AfterRot = {};

	bool m_EndStop = false;
};