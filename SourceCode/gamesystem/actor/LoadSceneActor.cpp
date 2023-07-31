#include "LoadSceneActor.h"
#include "ImageManager.h"
#include "Easing.h"
#include "Helper.h"
#include "SelectScene.h"

//初期化
void LoadSceneActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//オーディオ
	SelectScene::GetIns()->Init();

	BaseInitialize(dxCommon, { 0,10,200 }, { 0,0,-200 });

	if (!s_GameLoop) {
		SceneManager::GetInstance()->SetLoad(true);
		s_GameLoop = true;
	}


	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();

	m_Sprites[BackScreen] = IKESprite::Create(ImageManager::SELECT, { 0,0 });

	for (int i = text_L; i <= text_I; i++) {
		m_SpritesPos[i].x = (i + 1) * 100.0f;
		m_SpritesPos[i].y = 630.0f;
		m_Sprites[i] = IKESprite::Create(ImageManager::LOADING_LO + i, m_SpritesPos[i]);
		m_Sprites[i]->SetAnchorPoint({ 0.5f,0.5f });
		m_Sprites[i]->SetSize({ 96.0f, 96.0f });
	}

	const int w = 128;
	const int h = 128;
	const int l = 4;
	for (int i = 0; i < l; i++) {
		m_Sprites[Sutopon_1 + i] = IKESprite::Create(ImageManager::SUTOPONMOVE, { 1280.f-140,720.f-120.f });
		int number_index_y = i / l;
		int number_index_x = i % l;
		m_Sprites[Sutopon_1 + i]->SetTextureRect(
			{ static_cast<float>(number_index_x) * w, static_cast<float>(number_index_y) * h },
			{ static_cast<float>(w), static_cast<float>(h) });
		m_Sprites[Sutopon_1 + i]->SetSize({ 128.f,128.f });
		m_Sprites[Sutopon_1 + i]->SetScale(1.0f);
		m_Sprites[Sutopon_1 + i]->SetAnchorPoint({ 0.5f,0.5f });
	}

	camerawork->SetCameraState(CAMERA_LOAD);
}
//更新
void LoadSceneActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	lightgroup->Update();
	camerawork->Update(camera);
	//関数ポインタで状態管理
	(this->*stateTable[static_cast<size_t>(m_SceneState)])(camera);
	sceneChanger_->Update();
	if (feedf) {
		Feed2::GetInstance()->FeedIn2(Feed2::FeedType2::BLACK, 0.02f, feedf);
	}

	if (!s_Skip) {
		str = "TUTORIAL";
	}
	else {
		str = "SELECT";
	}
	//一定時間でシーンが変わる
	if (m_LoadTimer >= 200 && !SceneManager::GetInstance()->GetLoad()) {
		SceneManager::GetInstance()->ChangeScene(str);
		Audio::GetInstance()->StopWave(AUDIO_TITLE);
	}
}
//描画
void LoadSceneActor::Draw(DirectXCommon* dxCommon) {
	//ポストエフェクトをかけるか
	if (PlayPostEffect) {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		BackDraw(dxCommon);
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		postEffect->Draw(dxCommon->GetCmdList());
		SpriteDraw();
		//ImGuiDraw(dxCommon);
		dxCommon->PostDraw();
	} else {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		postEffect->Draw(dxCommon->GetCmdList());
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		//ImGuiDraw(dxCommon);
		BackDraw(dxCommon);
		SpriteDraw();
		dxCommon->PostDraw();
	}
}
//前面描画
void LoadSceneActor::SpriteDraw() {

	IKESprite::PreDraw();
	for (int i = 0; i < Sutopon_1; i++) {
		m_Sprites[i]->Draw();
	}
	m_Sprites[sutoponNow]->Draw();
	IKESprite::PostDraw();
	sceneChanger_->Draw();
	//sceneChanger_->FeedDraw();
	if (feedf)
	Feed2::GetInstance()->Draw2();
}
//ロード中の動き
void LoadSceneActor::IntroUpdate(DebugCamera* camera) {
	//�ŏ��̕�����������Ɠ�����
	m_SpritesAngle[0] += AddMovingVal;
	for (int i = text_L; i <= text_I; i++) {
		if (i != 0 && m_SpritesAngle[i - 1] > AddMovingVal * 5.0f) {
			m_SpritesAngle[i] += AddMovingVal;
		}
		m_SpritesPos[i].x = CenterPos.x + static_cast<float>(i) * WordsInter;
		m_SpritesPos[i].y = CenterPos.y + sinf(m_SpritesAngle[i] * PI / PI_180) * space;
		m_Sprites[i]->SetPosition(m_SpritesPos[i]);
	}
	sutoponTimer++;
	Helper::GetInstance()->Clamp(sutoponTimer, 0, 10);
	if (sutoponTimer == 10) {
		sutoponNow++;
		if (sutoponNow > Sutopon_4) {
			sutoponNow = Sutopon_1;
		}
		sutoponTimer = 0;
	}

	if (!SceneManager::GetInstance()->GetLoad()) {
		for (int i = text_L; i <= text_I; i++) {
			m_StopPos[i] = m_SpritesPos[i];
		}
		m_SceneState = SceneState::MainState;
	}
}
void LoadSceneActor::MainUpdate(DebugCamera* camera) {
	m_LoadTimer++;
	float frame = (float)m_LoadTimer / (float)LoadTimerMax;
	for (int i = text_L; i <= text_I; i++) {
		m_SpritesPos[i].x = Ease(In, Linear, frame, m_StopPos[i].x, CenterPos.x + static_cast<float>(i) * WordsInter);
		m_SpritesPos[i].y = Ease(In, Linear, frame, m_StopPos[i].y, CenterPos.y);
		m_Sprites[i]->SetPosition(m_SpritesPos[i]);

	}
	//��莞�ԂŃV�[�����ς��
	if (m_LoadTimer >= LoadTimerMax) {
		m_LoadTimer = 0;
		m_SceneState = SceneState::FinishState;
	}

}
void LoadSceneActor::FinishUpdate(DebugCamera* camera) {
	//一定時間でシーンが変わる
	if (m_LoadTimer >= LoadTimerMax) {
		sceneChanger_->ChangeStart();
		sceneChanger_->ChangeScene(str, SceneChanger::NonReverse);
		return;
	}
	m_LoadTimer++;

	float frame = (float)m_LoadTimer / (float)LoadTimerMax;

	for (int i = text_L; i < SpriteMax; i++) {

		float rot = Ease(In, Quad, frame, 0, PI_360);
		m_Sprites[i]->SetRotation(rot);
	}
}

//背面描画
void LoadSceneActor::BackDraw(DirectXCommon* dxCommon) {


}
//ImGuiの描画
void LoadSceneActor::ImGuiDraw(DirectXCommon* dxCommon) {

}
//解放
void LoadSceneActor::Finalize() {
}