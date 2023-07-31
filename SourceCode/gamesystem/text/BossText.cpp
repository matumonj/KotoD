#include "BossText.h"
#include "Helper.h"
#include "ImageManager.h"
#include "Easing.h"
BossText::BossText() {
	window_ = IKESprite::Create(ImageManager::WINDOW, window_pos);
	window_->SetAnchorPoint({ 0.5f,0.5f });
	window_->SetSize(window_size);
	window_->SetPosition(window_pos);
}

void BossText::Initialize(DirectXCommon* dxCommon) {
	text_ = make_unique<TextManager>();
	text_->Initialize(dxCommon);
	text_->SetConversation(TextManager::AISATU);
}

void BossText::SelectText(TextManager::Name_Second name) {
	text_->SetSecondConversation(name);
}
void BossText::SelectText(TextManager::Name_First name, const XMVECTOR& color) {
	text_->SetConversation(name,color);
}

void BossText::SelectText(TextManager::Name_Cap name, const XMVECTOR& color)
{
	text_->SetCapConversation(name);
}

void BossText::SelectText(TextManager::Name_Fourth name) {
	text_->SetFourthConversation(name);
}
void BossText::SelectText(TextManager::Name_CameraBoss name, const XMVECTOR& color) {
	text_->SetCameraBossConversation(name);
}
void BossText::SelectText(TextManager::Name_Last name, const XMVECTOR& color) {
	text_->SetLastConversation(name);
}
void BossText::SpriteDraw(DirectXCommon* dxCommon) {
	if (window_invisible_) {
		IKESprite::PreDraw();
		window_->Draw();
		IKESprite::PostDraw();
	}
	text_->TestDraw(dxCommon);
}

void BossText::Display()
{
	if (Helper::GetInstance()->FrameCheck(m_Frame, 0.05f)) {
		m_Frame = 1.0f;
	}
	window_pos.y = Ease(Out, Sine, m_Frame, WinApp::window_height + 100, WinApp::window_height - 100);
	window_size.x = Ease(Out, Sine, m_Frame, 0, 1300);
	window_size.y = Ease(Out, Sine, m_Frame, 0, 223);

	window_->SetPosition(window_pos);
	window_->SetSize(window_size);
}

void BossText::ChangeColor(int row, const XMVECTOR& color) {
	text_->SetOnceColor(row, color);
}

void BossText::ChangeColor(const XMVECTOR& color) {
	text_->SetAllColor(color);

}
