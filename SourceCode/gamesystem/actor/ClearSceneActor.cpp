#include "ClearSceneActor.h"
#include "SceneManager.h"
#include <Easing.h>
#include "ImageManager.h"
#include "VariableCommon.h"
#include "SelectScene.h"
#include "Audio.h"
//初期化
void ClearSceneActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//共通の初期化
	BaseInitialize(dxCommon);

	//オーディオ
	//Audio::GetInstance()->LoadSound(3, "Resources/Sound/BGM/jto3s-8fzcz.wav");
	//Audio::GetInstance()->LoopWave(3, VolumManager::GetInstance()->GetBGMVolum());
	//シーンチェンジャー
	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();

	//タイトル
	ClearSprite = IKESprite::Create(ImageManager::GAMEOVER, { 0.0f,0.0f });
	ClearSprite->SetColor({1.2f,1.2f,1.2f,1.0f});
	ClearSprite->SetSize({ 1280.0f,720.0f });

}
//更新
void ClearSceneActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	Input* input = Input::GetInstance();
	if (input->TriggerButton(input->B)&&!sceneChanger_->GetEasingStart()) {
		sceneChanger_->ChangeStart();
		str = "SELECT";
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Clear.wav", VolumManager::GetInstance()->GetSEVolum()+1.5f);
		SceneSave::GetInstance()->AllClear();
		if (SceneSave::GetInstance()->GetClearFlag(kFirstStage)) {
			SelectScene::GetIns()->SetSelectState(SELECT_SECOND);
		}
	}

	SelectScene::GetIns()->ResetParama();
	sceneChanger_->ChangeScene(str, SceneChanger::Reverse);
	sceneChanger_->Update();

	lightgroup->Update();
	//丸影
	lightgroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(0, XMFLOAT3({ 0.0f,0.0f,0.0f }));
	lightgroup->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));
}
//描画
void ClearSceneActor::Draw(DirectXCommon* dxCommon) {
	//ポストエフェクトをかけるか
	if (PlayPostEffect) {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		BackDraw(dxCommon);
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		postEffect->Draw(dxCommon->GetCmdList());
		FrontDraw();
		ImGuiDraw(dxCommon);
		dxCommon->PostDraw();
	}
	else {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		postEffect->Draw(dxCommon->GetCmdList());
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		ImGuiDraw(dxCommon);
		BackDraw(dxCommon);
		FrontDraw();
		dxCommon->PostDraw();
	}
}
//前面描画
void ClearSceneActor::FrontDraw() {
	IKESprite::PreDraw();
	ClearSprite->Draw();
	IKESprite::PostDraw();
	sceneChanger_->Draw();
}
//背面
void ClearSceneActor::BackDraw(DirectXCommon* dxCommon)
{
}
//ImGui描画
void ClearSceneActor::ImGuiDraw(DirectXCommon* dxCommon) {
}
//解放
void ClearSceneActor::Finalize() {
}
