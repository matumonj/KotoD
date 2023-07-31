#include "ClearSceneActor.h"
#include "SceneManager.h"
#include <Easing.h>
#include "ImageManager.h"
#include "VariableCommon.h"
#include "SelectScene.h"
#include "Audio.h"
//������
void ClearSceneActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//���ʂ̏�����
	BaseInitialize(dxCommon);

	//�I�[�f�B�I
	//Audio::GetInstance()->LoadSound(3, "Resources/Sound/BGM/jto3s-8fzcz.wav");
	//Audio::GetInstance()->LoopWave(3, VolumManager::GetInstance()->GetBGMVolum());
	//�V�[���`�F���W���[
	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();

	//�^�C�g��
	ClearSprite = IKESprite::Create(ImageManager::GAMEOVER, { 0.0f,0.0f });
	ClearSprite->SetColor({1.2f,1.2f,1.2f,1.0f});
	ClearSprite->SetSize({ 1280.0f,720.0f });

}
//�X�V
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
	//�ۉe
	lightgroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(0, XMFLOAT3({ 0.0f,0.0f,0.0f }));
	lightgroup->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));
}
//�`��
void ClearSceneActor::Draw(DirectXCommon* dxCommon) {
	//�|�X�g�G�t�F�N�g�������邩
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
//�O�ʕ`��
void ClearSceneActor::FrontDraw() {
	IKESprite::PreDraw();
	ClearSprite->Draw();
	IKESprite::PostDraw();
	sceneChanger_->Draw();
}
//�w��
void ClearSceneActor::BackDraw(DirectXCommon* dxCommon)
{
}
//ImGui�`��
void ClearSceneActor::ImGuiDraw(DirectXCommon* dxCommon) {
}
//���
void ClearSceneActor::Finalize() {
}
