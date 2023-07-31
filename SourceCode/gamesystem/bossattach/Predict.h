#pragma once
#include "IKETexture.h"
#include <memory>
class Predict {
private:
	// DirectX::Çè»ó™
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	void Initialize(const XMFLOAT3& pos);
	void Update();
	void Draw(DirectXCommon* dxCommon);
	void ImGuiDraw();
private:
	
public:
	bool GetAlive() { return m_Alive; }
private:
	std::unique_ptr<IKETexture> tex;
	XMFLOAT3 m_Position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_Rotation = { 0.0f,0.0f,0.0f };
	XMFLOAT4 m_Color = { 1.0f,1.0f,1.0f,1.0f };
	XMFLOAT3 m_Scale = { 1.5f,1.5f,1.5f };
	bool m_Alive = true;
	float m_Frame = 0.0f;
};

