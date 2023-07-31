#include "Predict.h"
#include "ImageManager.h"
#include "Easing.h"
#include "Player.h"
void Predict::Initialize(const XMFLOAT3& pos) {
	tex.reset(IKETexture::Create(ImageManager::DARKBOM, { 0,0,0 }, { 12,12,12 }, { 1,1,1,0.6f }));
	tex->TextureCreate();
	m_Position = pos;
	tex->SetRotation({ 90.0f, 0.0f, 0.0f });
	tex->SetScale({ 1.5f,1.5f,1.5f });
	tex->SetColor(m_Color);
	m_Alive = true;
}

void Predict::Update() {
	float l_AddFrame = 0.01f;
	if (m_Alive) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		}
		else {
			m_Frame = 0.0f;
			m_Alive = false;
		}

		m_Color.w = Ease(In, Cubic, m_Frame, m_Color.w, 0.0f);
	}
	tex->SetPosition(m_Position);
	tex->SetScale(m_Scale);
	tex->SetColor(m_Color);
	tex->Update();
}

void Predict::Draw(DirectXCommon* dxCommon) {
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	if (m_Alive) {
		tex->Draw();
	}
	IKETexture::PostDraw();
}
void Predict::ImGuiDraw() {

}
