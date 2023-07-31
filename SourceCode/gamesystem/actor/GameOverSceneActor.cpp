#include "GameOverSceneActor.h"
#include "SceneManager.h"
#include <Easing.h>
#include "ImageManager.h"
#include "VariableCommon.h"
#include "Audio.h"
#include "SelectScene.h"

//初期化
void GameOverSceneActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//共通の初期化
	BaseInitialize(dxCommon);
	camerawork->SetEye({ 0,10,0 });
	camerawork->SetTarget({ 0,0,0 });
	camerawork->SetCameraState(CAMERA_NORMAL);
	camerawork->DefUpda(camera);
	//オーディオ
	Audio::GetInstance()->PlayWave("Resources/Sound/SE/gameOverSE.wav", VolumManager::GetInstance()->GetSEVolum());
	//シーンチェンジャー
	PlayPostEffect = false;
	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();
	m_SceneState = SceneState::IntroState;


	SutoponObj = make_unique<IKEObject3d>();
	SutoponObj->Initialize();
	SutoponObj->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Sutopon));
	
	FrontSprite[0]=IKESprite::Create(ImageManager::GAMEOVER_1,{0,0});
	FrontSprite[1]=IKESprite::Create(ImageManager::GAMEOVER_2,{0,0});
	FrontSprite[2]=IKESprite::Create(ImageManager::GAMEOVER_3,{0,0});
	for (auto i = 0; i < FrontSprite.size(); i++) {
		FrontSprite[i]->SetSize({ 1280,720 });

		FrontSprite[i]->SetColor({ 1,1,1,0 });
	}
	//タイトル
	ClearSprite = IKESprite::Create(ImageManager::GAMEOVER, { 0.0f,0.0f });
	ClearSprite->SetSize({ 1280.0f,720.0f });
	ClearSprite->SetColor({ 1,1,1,1 });
}
//更新
void GameOverSceneActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	Input* input = Input::GetInstance();
	if (input->TriggerButton(input->A) || input->TriggerButton(input->B)) {
		if (sceneChanger_->GetEasingStart()) { return; }
		sceneChanger_->ChangeStart();
		if (input->TriggerButton(input->A)) {
			SelectScene::GetIns()->ResetParama();
			str = "TITLE";
			Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Over.wav", VolumManager::GetInstance()->GetSEVolum());
		} else {
			str = NextStageName();
			Audio::GetInstance()->PlayWave("Resources/Sound/SE/Voice_Retry.wav", VolumManager::GetInstance()->GetSEVolum());
		}
		m_SceneState = SceneState::FinishState;
	}
	(this->*stateTable[static_cast<size_t>(m_SceneState)])(camera);
}
//描画
void GameOverSceneActor::Draw(DirectXCommon* dxCommon) {
	//ポストエフェクトをかけるか
	if (PlayPostEffect) {
		dxCommon->PreDraw();
		//postEffect->Draw(dxCommon->GetCmdList());
		BackDraw(dxCommon);
		FrontDraw();
		ImGuiDraw(dxCommon);
		dxCommon->PostDraw();
	} else {
		dxCommon->PreDraw();
		ImGuiDraw(dxCommon);
		BackDraw(dxCommon);
		FrontDraw();
		dxCommon->PostDraw();
	}
}
//前面描画
void GameOverSceneActor::FrontDraw() {
	sceneChanger_->Draw();
}
void GameOverSceneActor::IntroUpdate(DebugCamera* camera) {
	frame += 1.0f / kFrameMax;
	frame = clamp(frame,0.f,1.f);
	radius = Ease(Out, Quad, frame, 0.f, 720.0f);
	m_eye.x = sinf(radius * (XM_PI / 180.0f)) * 20.f;
	m_eye.y = Ease(Out, Quad, frame, 20.0f, 0.0f);
	m_eye.z = cosf(radius * (XM_PI / 180.0f)) * 20.f;
	camerawork->SetEye(m_eye);
	camerawork->DefUpda(camera);

	sca += 0.2f;
	float scale=1.0f+sinf(sca)*0.05f;
	SutoponObj->SetScale({scale,scale ,scale });
	SutoponObj->Update();
	if (frame >= 1.0f) {
		heartframe += 1.0f / 60.0f;
		heartframe = clamp(heartframe, 0.f, 1.f);
		if (heartframe >= 1.0f) {
			SutoponObj->SetScale({ 1,1,1 });
			SutoponObj->Update();
			frame = 0.0f;
			heartframe = 0.0f;
			m_SceneState = SceneState::MainState;
		}
	}

}
void GameOverSceneActor::MainUpdate(DebugCamera* camera) {
	frame += 1.0f / kFrameMax;
	frame = clamp(frame, 0.f, 1.f);

	float scale = Ease(Out, Sine, frame, 1.f, 0.0f);
	SutoponObj->SetScale({ scale,scale,scale });
	XMFLOAT3 rot=SutoponObj->GetRotation();
	rot.x+=3.f;
	rot.y+=3.f;
	SutoponObj->SetRotation(rot);
	SutoponObj->Update();
	
	if (frame<1.0f) {
		XMFLOAT3 pos = SutoponObj->GetPosition();
		ParticleEmitter::GetInstance()->FireEffect(120, pos, 3.0f, 0.5f, { 1.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	} else {
		heartframe += 1.0f / 60.0f;
		heartframe = clamp(heartframe, 0.f, 1.f);
 
		alphas[0] += 1.0f / 15.f;
		for (auto i = 0; i < alphas.size(); i++) {
			if (i != 0 && alphas[i - 1] >= 1.f) {
				alphas[i] += 1.0f / 15.f;
			}
			alphas[i] = clamp(alphas[i], 0.f, 1.f);
			FrontSprite[i]->SetColor({1,1,1,alphas[i] });
		}
	}
	ParticleEmitter::GetInstance()->Update();

}
void GameOverSceneActor::FinishUpdate(DebugCamera* camera) {
	sceneChanger_->ChangeScene(str, SceneChanger::Reverse);
	sceneChanger_->Update();
}
string GameOverSceneActor::NextStageName() {
	string str = "TUTORIAL";
	for (int i = 0; i < kMaxStage; i++) {
		if (!SceneSave::GetInstance()->GetLoseFlag((SeceneCategory)i)) { continue; }
		switch (i) {
		case 1:
			str = "FIRSTSTAGE";
			break;
		case 2:
			str = "SECONDSTAGE";
			break;
		case 3:
			str = "THIRDSTAGE";
			break;
		case 4:
			str = "FOURTHSTAGE";
			break;
		case 5:
			str = "FIVESTAGE";
			break;
		case 6:
			str = "SIXSTAGE";
			break;
		case 7:
			str = "SEVENSTAGE";
			break;
		default:
			break;
		}
		SceneSave::GetInstance()->SetLoseFlag((SeceneCategory)i, false);
		break;
	}
	return str;
}
//背面
void GameOverSceneActor::BackDraw(DirectXCommon* dxCommon) {
	IKESprite::PreDraw();
	ClearSprite->Draw();
	IKESprite::PostDraw();

	IKEObject3d::PreDraw();
	SutoponObj->Draw();
	IKEObject3d::PostDraw();
	ParticleEmitter::GetInstance()->FlontDrawAll();
	IKESprite::PreDraw();
	for (auto i = 0; i < alphas.size(); i++) {
		FrontSprite[i]->Draw();
	}
	IKESprite::PostDraw();

}
//ImGui描画
void GameOverSceneActor::ImGuiDraw(DirectXCommon* dxCommon) {
	//SceneSave::GetInstance()->ImGuiDraw();
}
//解放
void GameOverSceneActor::Finalize() {
}
