#include "InterStamp.h"

//XV
void InterStamp::Update() {
	Action();

	texture->Update();
	texture->SetPosition(m_Position);
	texture->SetRotation(m_Rotation);
	texture->SetScale(m_Scale);
	texture->SetColor(m_Color);
}
//•`‰æ
void InterStamp::Draw(DirectXCommon* dxCommon) {
	Origin_Draw(dxCommon);
}
//ImGui
void InterStamp::ImGuiDraw() {
	ImGui_Origin();
}