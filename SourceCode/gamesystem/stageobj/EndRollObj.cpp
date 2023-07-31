#include "EndRollObj.h"
#include "Helper.h"
#include "Player.h"
EndRollObj::EndRollObj() {
	//各ボスのインスタンス生成
	boss[BIN].reset(new FirstBoss());
	boss[BIN]->Initialize();

	boss[BOOK].reset(new SecondBoss());
	boss[BOOK]->Initialize();

	boss[STAMP].reset(new ThirdBoss());
	boss[STAMP]->Initialize();

	boss[CAMERA].reset(new FourthBoss());
	boss[CAMERA]->Initialize();

	boss[DARK].reset(new FiveBoss());
	boss[DARK]->Initialize();

	boss[DJ].reset(new SixBoss());
	boss[DJ]->Initialize();

	boss[LAST].reset(new SevenBoss());
	boss[LAST]->Initialize();

	//各クラス
	Player::GetInstance()->InitState({ 0.0f,5.0f,-5.0f });
}
//初期化
void EndRollObj::Initialize() {

}
//更新
void EndRollObj::Update() {
	for (size_t i = 0; i < boss.size(); i++) {
		boss[i]->EndRollUpdate();
	}
}
//描画
void EndRollObj::Draw(DirectXCommon* dxCommon) {
	for (size_t i = 0; i < boss.size(); i++) {
		boss[i]->Draw(dxCommon);
	}

}
//スプライト描画
void EndRollObj::SpriteDraw() {

}
//Imgui
void EndRollObj::ImGuiDraw() {
	for (size_t i = 0; i < boss.size(); i++) {
		boss[i]->ImGuiDraw();
	}

	//Player::GetInstance()->ImGuiDraw();
}