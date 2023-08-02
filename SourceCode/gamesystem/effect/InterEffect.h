#pragma once
#include <DirectXMath.h>
#include <array>   
#include <memory> 
#include "ParticleEmitter.h"
using namespace std;         //  名前空間指定
//エフェクトの基底クラス
class InterEffect {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//gettersetter
	bool GetAlive() { return m_Alive; }

	void SetPosition(const XMFLOAT3& position) { m_Position = position; }
	void SetColor(const XMFLOAT4& Color) { m_Color = Color; }
public:

	//初期化
	virtual bool Initialize() = 0;
	//更新
	void Update();

	void ImGuiDraw();
	
	virtual void Action() = 0;//エフェクト特有の処理
	virtual void ImGuiOrigin() = 0;
protected:
	float DiviSpeed;
	XMFLOAT3 m_Position = { 0.0f,0.0f,0.0f };
	XMFLOAT4 m_Color = { 0.0f,0.0f,0.0f,0.0f };
	bool m_Alive = false;

	int m_AliveTimer = 0;
	int m_Life;
public:
	void SetLife(int life) { m_Life = life; }
	void SetDiviSpeed(float speed) { DiviSpeed = speed; }
};

