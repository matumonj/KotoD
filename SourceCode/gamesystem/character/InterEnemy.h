#pragma once
#include "ObjCommon.h"
//敵基底
class InterEnemy :
	public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://getter setter

protected:

public:
	//virtual ~InterEnemy() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual bool Initialize()override;
	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize() = 0;
	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update();

	virtual void Action() = 0;//敵の行動

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw(DirectXCommon* dxCommon)override;

	virtual void ImGuiDraw() = 0;

	virtual void Appearance() {};
protected: //静的メンバ変数
	bool isAlive = true;
	bool isStop = false;
	bool isUnrival = false;
	int HP = 0;
	float radius = 2.5f;
	bool ShotF = false;
	int ShotCount = 0;
	bool DeathEffectF = false;
	bool isWeak = false;

public:
	void SetShotF(bool f) { ShotF = f; }
	void SetIsStop(bool isStop) { this->isStop = isStop; }
	void SetIsWeak(bool isWeak) { this->isWeak = isWeak; }

	bool GetShotF() { return ShotF; }
	int GetShotCount() { return ShotCount; }

	bool GetisAlive() { return isAlive; }

	float GEtAlpha() { return m_Color.w; }

	void SetAlive(bool f) { isAlive = f; }

	void SetColor(XMFLOAT4 color) { m_Color = color; }

	void SetHP(int HP) { this->HP = HP; }

	float GetRadius() { return radius; }
	void SetUnrival(bool isUnrival) {
		this->isUnrival = isUnrival; isActive = isUnrival;};
protected:
	void ColPlayer();
	void OnCollision();
	void DeathAction();
protected:
	bool canRot = false;
	float deathFrame = 0.0f;
	float deathFrameMax = 40.0f;
	bool isActive = false;

	bool Rush = false;
public:
	void SetcanRotandRush(bool f) { canRot = f; Rush = f; }

	void EnemyColNormal(const XMFLOAT3& pos);
};