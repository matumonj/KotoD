#include "ConfuEffect.h"
#include "ImageManager.h"
#include "Easing.h"
#include "Player.h"
#include "Helper.h"
#define WindowW 1920
#define WindowH 1020
//読み込み
ConfuEffect::ConfuEffect() {
	//初期化
	EmiSprite = IKESprite::Create(ImageManager::FEED, { 0,0 });
	EmiSprite->SetAnchorPoint({ 0.f,0.f });
	EmiSprite->SetSize({ WindowW,WindowH });

	for (std::size_t i = 0; i < stuntex.size(); i++) {
		stuntex[i].reset(IKETexture::Create(ImageManager::STUN, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
		stuntex[i]->TextureCreate();
		stuntex[i]->SetRotation({ 0,0,0 });
		stuntex[i]->SetScale({ 0.2f,0.2f,0.2f });
	}
}
//初期化
void ConfuEffect::Initialize() {
	for (int i = 0; i < stuntex.size(); i++) {
		m_Stunscale[i] = 4.0f;
		m_StunSpeed[i] = i * 90.0f;
		m_StunCircleX[i] = {};
		m_StunCircleZ[i] = {};
		m_StunPos[i] = {};
		m_Stunradius[i] = {};
	}
}
//更新
void ConfuEffect::Update() {
	//発光
	Emission();
	//ぴよぴよ
	StunEffect();
}
//描画
void ConfuEffect::Draw(DirectXCommon* dxCommon) {
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	for (std::size_t i = 0; i < stuntex.size(); i++) {
		if (Player::GetInstance()->GetConfu()) {
			stuntex[i]->Draw();
		}
	}
	IKETexture::PostDraw();

	IKESprite::PreDraw();
	if (m_Alive) {
		EmiSprite->Draw();
	}
	IKESprite::PostDraw();
}
//ImGui
void ConfuEffect::ImGuiDraw() {
}
//一瞬の発光
void ConfuEffect::Emission() {
	const float l_AddFrame = 0.1f;
	if (m_Alive) {
		if (m_EmiState == EMI_SET) {
			if (m_Frame < m_FrameMax) {
				m_Frame += l_AddFrame;
			}
			else {
				m_Frame = {};
				m_AfterAlpha = {};
				m_EmiState = EMI_END;
			}
		}
		else {
			if (m_Frame < m_FrameMax) {
				m_Frame += l_AddFrame;
			}
			else {
				m_Frame = {};
				m_AfterAlpha = 1.0f;
				m_EmiState = EMI_SET;
				m_Alive = false;
			}
		}

		m_Alpha = Ease(In, Cubic, m_Frame, m_Alpha, m_AfterAlpha);
	}

	EmiSprite->SetColor({ m_Color.x,m_Color.y,m_Color.z,m_Alpha });
}
//スタン時エフェクト
void ConfuEffect::StunEffect() {
	//スタン時のぴよぴよ
	for (int i = 0; i < stuntex.size(); i++) {
		if (Player::GetInstance()->GetConfu()) {
			m_StunSpeed[i] += 2.0f;
		}
		m_Stunradius[i] = m_StunSpeed[i] * PI / 180.0f;
		m_StunCircleX[i] = cosf(m_Stunradius[i]) * m_Stunscale[i];
		m_StunCircleZ[i] = sinf(m_Stunradius[i]) * m_Stunscale[i];
		m_StunPos[i].x = m_StunCircleX[i] + m_BasePos.x;
		m_StunPos[i].z = m_StunCircleZ[i] + m_BasePos.z;
		m_StunPos[i].y = m_BasePos.y + 7.0f;
		stuntex[i]->SetPosition(m_StunPos[i]);
		stuntex[i]->Update();
		stuntex[i]->SetIsBillboard(true);
	}
}