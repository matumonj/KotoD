#pragma once
#include "ObjCommon.h"
#include "IKETexture.h"
enum DirState {
	DIR_UP,
	DIR_DOWN,
	DIR_RIGHT,
	DIR_LEFT,
};
//火の玉クラス
class DamageBlock :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	DamageBlock();
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

	void Move();

	void BirthParticle();
public:
	void InitPos(const int Num,const XMFLOAT3& Pos);
public:
	//gettersetter
	const bool& GetAlive() { return m_Alive; }
	void SetAttackDir(const int AttackDir) { m_AttackDir = AttackDir; }
private:
	unique_ptr<IKETexture> tex;//エリア
	int m_AttackDir = {};
	float m_Frame = {};
	bool m_Alive = false;
	XMFLOAT3 m_AfterPos = {};
	float m_Damage = {};

	//テクスチャ関連
	XMFLOAT3 m_TexPos = {};
	XMFLOAT3 m_TexRot = {};
	XMFLOAT3 m_TexScale = {};
	XMFLOAT4 m_TexColor = { 1.0f,1.0f,1.0f,0.0f };
	float m_Alpha = 0.0f;
	float m_AfterAlpha = 0.5f;
private:
	enum BlockState {
		BLOCK_SET,
		BLOCK_PLAY,
	}_BlockState;
public:
	
};