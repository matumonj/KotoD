#include "NoteEffect.h"
#include "ImageManager.h"
#include <random>
//ì«Ç›çûÇ›
NoteEffect::NoteEffect() {

	for (int i = 0; i < NoteSprite.size(); i++) {
		NoteSprite[i] = IKESprite::Create(ImageManager::NOTE, { 0,0 });
		NoteSprite[i]->SetAnchorPoint({ 0.f,0.f });
		NoteSprite[i]->SetSize({ 400.0f,400.0f });
	}
	for (int i = 0; i < NoteSprite.size(); i++) {
		NoteSprite2[i] = IKESprite::Create(ImageManager::NOTE2, { 0,0 });
		NoteSprite2[i]->SetAnchorPoint({ 0.f,0.f });
		NoteSprite2[i]->SetSize({ 400.0f,400.0f });
	}
}

//èâä˙âª
void NoteEffect::Initialize() {
	for (int i = 0; i < NoteSprite.size(); i++) {
		m_NotePos[i] = { 1380.0f,200.0f };
		m_NoteColor[i] = {};
		m_NotePos2[i] = { -100.0f,200.0f };
		m_NoteColor2[i] = {};
	}
}

//çXêV
void NoteEffect::Update() {
	FlowNote();
	for (int i = 0; i < NoteSprite.size(); i++) {
		NoteSprite[i]->SetPosition(m_NotePos[i]);
		NoteSprite[i]->SetColor(m_NoteColor[i]);
		NoteSprite2[i]->SetPosition(m_NotePos2[i]);
		NoteSprite2[i]->SetColor(m_NoteColor2[i]);
	}
}
//ï`âÊ
void NoteEffect::Draw(DirectXCommon* dxCommon) {
	IKESprite::PreDraw();
	if (m_Alive) {
		for (int i = 0; i < NoteSprite.size(); i++) {
			NoteSprite[i]->Draw();
			NoteSprite2[i]->Draw();
		}
	}
	IKESprite::PostDraw();
}
//ImGui
void NoteEffect::ImGuiDraw() {
}

void NoteEffect::FlowNote() {
	const float l_AddPos = 40.0f;
	const int l_LimitFlow = 200;
	if (m_Alive) {
		if (m_NoteState == NOTE_SET) {
			mt19937 mt{ std::random_device{}() };
			uniform_int_distribution<int> l_RandomR(70, 100);
			uniform_int_distribution<int> l_RandomG(70, 100);
			uniform_int_distribution<int> l_RandomB(70, 100);
			for (int i = 0; i < NoteSprite.size(); i++) {
				if (i < 5) {
					m_NotePos[i].x = ((1380.0f) + (i * 200.0f));
					m_NotePos[i].y = 0.0f;
					m_NotePos2[i].x = ((-100.0f) - (i * 200.0f));
					m_NotePos2[i].y = 0.0f;
				}
				else if (i >= 5 && i < 10) {
					m_NotePos[i].x = ((1920.0f) + ((i - 3) * 200.0f));
					m_NotePos[i].y = 300.0f;
					m_NotePos2[i].x = ((-100.0f) - (i * 200.0f));
					m_NotePos2[i].y = 300.0f;
				}
				else {
					m_NotePos[i].x = ((1920.0f) + ((i - 6) * 200.0f));
					m_NotePos[i].y = 600.0f;
					m_NotePos2[i].x = ((-100.0f) - (i * 200.0f));
					m_NotePos2[i].y = 600.0f;
				}
				m_NoteColor[i] = { float(l_RandomR(mt)) / 100.0f,float(l_RandomG(mt)) / 100.0f,float(l_RandomR(mt)) / 100.0f,1.0f };
				m_NoteColor2[i] = { float(l_RandomR(mt)) / 100.0f,float(l_RandomG(mt)) / 100.0f,float(l_RandomR(mt)) / 100.0f,1.0f };
			}
			m_NoteState = NOTE_MOVE;
		}
		else {
			m_FlowTimer++;
			for (int i = 0; i < NoteSprite.size(); i++) {
				m_NotePos[i].x -= l_AddPos;
				m_NotePos2[i].x += l_AddPos;
			}

			if (m_FlowTimer == l_LimitFlow) {
				m_FlowTimer = 0;
				m_NoteState = NOTE_SET;
				m_Alive = false;
			}
		}
	}
}
