#include "ClearText.h"
#include "ImageManager.h"
#include "Helper.h"
#include "Easing.h"
#include "Audio.h"
#include "VolumManager.h"
ClearText* ClearText::GetInstance()
{
	static ClearText instance;

	return &instance;
}

void ClearText::LoadResource() {
	//ƒ^ƒCƒgƒ‹
	ClearSprite = IKESprite::Create(ImageManager::CLEARLOGO, {640.0f,200.0f});
	ClearSprite->SetAnchorPoint({ 0.5f,0.5f });
}

void ClearText::Initialize() {
	m_Size = {};
	m_Alive = {};
	m_Frame = {};
	m_Sound = false;
}

void ClearText::Update() {
	const float l_AddFrame = 0.05f;

	if (m_Alive) {
		if (m_Frame > 0.3f) {
			if (!m_Sound) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/clearSE.wav", VolumManager::GetInstance()->GetSEVolum() * 0.5f);
				m_Sound = true;
			}
		}
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}
	}

	m_Size = { Ease(In,Cubic,m_Frame,m_Size.x,512.0f),
	Ease(In,Cubic,m_Frame,m_Size.y,300.0f) };

	ClearSprite->SetSize(m_Size);
}

void ClearText::Draw() {
	IKESprite::PreDraw();
	if (m_Alive) {
		ClearSprite->Draw();
	}
	IKESprite::PostDraw();
}

void ClearText::ImGuiDraw() {
	ImGui::Begin("Clear");
	ImGui::Text("Alive:%d", m_Alive);
	ImGui::Text("Frame:%f",m_Frame);
	ImGui::End();
}