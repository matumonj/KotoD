#pragma once
#include "IKETexture.h"
#include "IKEObject3d.h"
#include "IKEModel.h"
#include <vector>
#include <any>
#include <memory>
using namespace DirectX;
using namespace std;         //  名前空間指定
//ダメージエリア
class DamageArea {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	DamageArea(const int Num);
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(DirectXCommon* dxCommon);
	//IMGUI
	void ImGuiDraw();

private:
	void PointUpdate();	//点の更新
	void LineUpdate();	//線の更新
	void StateManager();//状態を管理する
	bool Collide();//当たり判定
private:
	IKEModel* model = nullptr;
	vector<IKEObject3d*> obj;//モデル
	vector<IKETexture*> tex;//エリア

	//点に必要な変数
	vector<XMFLOAT3> m_Position;
	vector<XMFLOAT3> m_Rotation;
	vector<XMFLOAT3> m_Scale;
	vector<XMFLOAT4> m_Color;
	vector<bool> m_Alive;

	//エリアに必要な変数
	vector<XMFLOAT3> m_TexPosition;
	vector<XMFLOAT3> m_TexRotation;
	vector<XMFLOAT3> m_TexScale;
	vector<XMFLOAT4> m_TexColor;
	vector<bool> m_TexAlive;
	vector<int> m_Hit;
	enum AreaState {
		POINT_BIRTH,
		LINE_BIRTH,
		STAY,
		VANISH_AREA,
	};

	int m_AreaState = POINT_BIRTH;

	float m_Frame = {};
	float m_Alpha = {};
	float m_CommonScale = {};
	float m_AfterAlpha = 1.0f;
	float m_AfterScale = 1.3f;

	int m_StayTimer = 0;
	int m_VanishLimit = {};

	float m_Damage = {};
	XMFLOAT2 m_OffSet = { -0.01f,0.0f };
};