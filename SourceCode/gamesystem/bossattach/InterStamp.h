#pragma once
#include"IKETexture.h"
#include <memory>
#include<DirectXMath.h>
using namespace DirectX;
using namespace std;         //  名前空間指定
class InterStamp {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:

	//初期化
	virtual bool Initialize(const XMFLOAT3& pos) = 0;
	//更新
	void Update();
	
	//描画
	void Draw(DirectXCommon* dxCommon);

	virtual void Action() = 0;//ボス特有の処理

	virtual void ImGui_Origin() = 0;//ボスそれぞれのImGui

	virtual void Origin_Draw(DirectXCommon* dxCommon) = 0;//ボスそれぞれのImGui

	void ImGuiDraw();//ImGuiの描画

public:
	//gettersetter
	bool GetBirth() { return m_Birth; }

	void SetAfterScale(float AfterScale) { m_AfterScale = AfterScale; };
	
protected:
	unique_ptr<IKETexture> texture;
	bool m_Birth = false;//出現
	bool m_Alive = false;//生存
	int m_BirthTimer = 0;//出現時間
	float m_Frame = 0.0f;
	XMFLOAT3 m_Position = { 0.0f,-200.0f,0.0f };
	XMFLOAT3 m_Rotation = { 90.0f,0.0f,0.0f };
	XMFLOAT3 m_Scale = { 0.0f,0.0f,0.0f };
	XMFLOAT4 m_Color = { 1.0f,1.0f,1.0f,0.0f };

	float m_AfterScale = {};

	float m_Radius = {};
};

