#include "InterBullet.h"

//更新
void InterBullet::Update() {
	Action();//弾ごとの処理
}

//描画
void InterBullet::Draw(DirectXCommon* dxCommon) {
	if (m_Alive) {
		Draw_Origin();
		Obj_Draw();
	

	}
}
//ImGui
void InterBullet::ImGuiDraw() {
	ImGui_Origin();//それぞれのImGui
}