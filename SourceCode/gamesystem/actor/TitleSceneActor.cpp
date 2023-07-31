#include "TitleSceneActor.h"
#include "input.h"
#include "ImageManager.h"
#include "ParticleEmitter.h"
#include "Menu.h"
#include "SelectScene.h"
#include "TItleObj.h"
//初期化
void TitleSceneActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//共通の初期化
	BaseInitialize(dxCommon);
	dxCommon->SetFullScreen(true);
	//オーディオ
	if (!SceneSave::GetInstance()->GetEndRoll()) {
		Audio::GetInstance()->LoopWave(AUDIO_TITLE, VolumManager::GetInstance()->GetBGMVolum() + 1.0f);
	}
	feed = new Feed();
	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();
	sceneChanger_->FeedInit();
	//sceneChanger_->ChangeFeedStart();
	if (Feed2::GetInstance() == nullptr) {
		Feed2::GetInstance()->SetIni2();
	}
	//タイトル
	TitleSprite = IKESprite::Create(ImageManager::PLAY, { 0.0f,0.0f });
	TitleSprite->SetAddOffset(-0.0005f);
	TitleWordSprite = IKESprite::Create(ImageManager::TITLEWORD, pos);
	TitleWordSprite->SetScale(0.3f);

	CreditSprite = IKESprite::Create(ImageManager::CREDIT, { 640.0f,360.0f });
	CreditSprite->SetAnchorPoint({ 0.5f,0.5f });

	const int TitleCount = TITLE_MAX;
	const float l_Width_Cut = 256.0f;
	const float l_Height_Cut = 64.0f;
	for (int i = 0; i < TitlePartsSprite.size(); i++) {
		TitlePartsSprite[i] = IKESprite::Create(ImageManager::TITLESELECT, { 0.0f,0.0f });
		int number_index_y = i / TitleCount;
		int number_index_x = i % TitleCount;
		TitlePartsSprite[i]->SetTextureRect(
			{ static_cast<float>(number_index_x) * l_Width_Cut, static_cast<float>(number_index_y) * l_Height_Cut },
			{ static_cast<float>(l_Width_Cut), static_cast<float>(l_Height_Cut) });
		TitlePartsSprite[i]->SetAnchorPoint({ 0.5f,0.5f });
		m_PartsSize[i] = { l_Width_Cut,l_Height_Cut };
		m_PartsPos[i] = { 1000.0f,(100.0f * i) + 350.0f };
	}

	PlayPostEffect = true;
	menu = make_unique<Menu>();
	menu->Initialize();

	camerawork->SetCameraState(CAMERA_TITLE);
	camerawork->Update(camera);

	//各クラス
	Player::GetInstance()->LoadResource();
	Player::GetInstance()->InitState({ 0.0f,-2.0f,-30.0f });
	TitleObj::GetInstance()->Initialize();
}
//更新
void TitleSceneActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//セレクト
	SceneSelect();
	Input* input = Input::GetInstance();
	if (feedF) {
		Feed2::GetInstance()->FeedIn2(Feed2::FeedType2::BLACK, 0.02f, feedF);
		if (Feed2::GetInstance()->GetAlpha2() >= 1.0f) {
			SceneManager::GetInstance()->ChangeScene("LOAD");
		}
	}
	sceneChanger_->Update();
	lightgroup->Update();
	ParticleEmitter::GetInstance()->FireEffect(100, { 0.0f,23.0f,0.0f }, 5.0f, 0.0f, { 1.0f,0.5f,0.0f,0.5f }, { 1.0f,0.5f,0.0f,0.5f });
	//パーティクル更新
	ParticleEmitter::GetInstance()->Update();

	postEffect->SetCloseRad(SelectScene::GetIns()->GetCloseIconRad());
	if (SelectScene::GetIns()->GetPedestal() != nullptr) {
		if (SelectScene::GetIns()->GetCloseScl() < 10000.f)
			SelectScene::GetIns()->Upda();
		else
			SelectScene::GetIns()->ResetParama();
	}

	Player::GetInstance()->TitleUpdate();
	TitleObj::GetInstance()->Update();
	menu->Update();
	camerawork->Update(camera);


}
//描画
void TitleSceneActor::Draw(DirectXCommon* dxCommon) {
	//ポストエフェクトをかけるか
	if (PlayPostEffect) {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		BackDraw(dxCommon);
		FrontDraw();
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		postEffect->Draw(dxCommon->GetCmdList());
		//ImGuiDraw(dxCommon);
		dxCommon->PostDraw();
	} else {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		postEffect->Draw(dxCommon->GetCmdList());
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		BackDraw(dxCommon);
		FrontDraw();
		//ImGuiDraw(dxCommon);
		dxCommon->PostDraw();
	}
}
//前面描画
void TitleSceneActor::FrontDraw() {
	IKESprite::PreDraw();
	TitleWordSprite->Draw();
	TitlePartsSprite[3]->Draw();
	for (int i = 0; i < 3; i++) {
		TitlePartsSprite[i]->Draw();
	}
	SelectScene::GetIns()->Draw_Sprite();

	CreditSprite->Draw();
	IKESprite::PostDraw();
	menu->Draw();
	if (feedF)
		Feed2::GetInstance()->Draw2();
	if (SceneSave::GetInstance()->GetEndRoll()) {
		sceneChanger_->Draw();
	}
}
//背面描画
void TitleSceneActor::BackDraw(DirectXCommon* dxCommon) {
	IKESprite::PreDraw();
	TitleSprite->Draw();
	IKESprite::PostDraw();
	IKEObject3d::PreDraw();
	Player::GetInstance()->Draw(dxCommon);
	TitleObj::GetInstance()->Draw(dxCommon);
	IKEObject3d::PostDraw();
}
//ImGui描画
void TitleSceneActor::ImGuiDraw(DirectXCommon* dxCommon) {
}
//解放
void TitleSceneActor::Finalize() {

}

void TitleSceneActor::SceneSelect() {
	Input* input = Input::GetInstance();

	if (!m_Change && !m_Credit) {
		if (_SelectType < SELECT_CREDIT) {
			//ボタンの場合
			if (input->TriggerButton(Input::DOWN)) {
				_SelectType++;
			}

			//スティックの場合
			if (input->TiltPushStick(Input::L_DOWN)) {
				if (!m_StickDOWN) {
					_SelectType++;
					m_StickDOWN = true;
				}
			} else {
				m_StickDOWN = false;
			}
		}
		if (_SelectType > NORMAL_SCENE) {
			//ボタンの場合
			if (input->TriggerButton(Input::UP)) {
				_SelectType--;
			}

			//スティックの場合
			if (input->TiltPushStick(Input::L_UP)) {
				if (!m_StickUP) {
					_SelectType--;
					m_StickUP = true;
				}
			} else {
				m_StickUP = false;
			}
		}
	}

	if ((input->TriggerButton(input->B) &&
		!sceneChanger_->GetEasingStart())) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Decide.wav", VolumManager::GetInstance()->GetSEVolum());

		if (_SelectType != SELECT_CREDIT) {
			sceneChanger_->ChangeStart();;
			feedF = true;
			SceneSave::GetInstance()->SetEndRoll(false);
			if (_SelectType == NORMAL_SCENE) {
				s_Skip = false;
			} else if (_SelectType == SELECT_SCENE) {
				s_Skip = true;
			}
			m_Change = true;
		} else {
			m_Credit = true;
		}
	}

	//クレジットを開いているかどうか
	if (!m_Credit) {
		m_CreditSize = { Ease(In,Cubic,0.5f,m_CreditSize.x,0.0f),
		Ease(In,Cubic,0.5f,m_CreditSize.y,0.0f), };
	} else {
		m_CreditSize = { Ease(In,Cubic,0.5f,m_CreditSize.x,1280.0f),
		Ease(In,Cubic,0.5f,m_CreditSize.y,720.0f), };

		if (input->TriggerButton(input->START)) {
			m_Credit = false;
		}
	}

	CreditSprite->SetSize(m_CreditSize);

	TitleMove();
}
//パーツの動き(タイトル)
void TitleSceneActor::TitleMove() {
	if (_SelectType == NORMAL_SCENE) {
		m_Angle[NORMAL_SCENE] += 2.0f;
		m_Angle2[NORMAL_SCENE] = m_Angle[NORMAL_SCENE] * (3.14f / 180.0f);
		//選択時座標が上下に動く
		m_PartsPos[SELECT_AREA] = m_PartsPos[NORMAL_SCENE];
		m_Angle[SELECT_SCENE] = 0.0f;
		m_Angle[SELECT_CREDIT] = 0.0f;
		//選択時サイズも少し変わる
		m_PartsSize[NORMAL_SCENE] = { (sin(m_Angle2[NORMAL_SCENE]) * 16.0f) + (256.0f),
			(sin(m_Angle2[NORMAL_SCENE]) * 16.0f) + (64.0f) };
		m_PartsSize[SELECT_AREA] = m_PartsSize[NORMAL_SCENE];
		m_PartsSize[SELECT_SCENE] = { 256.0f,64.0f };
		m_PartsSize[SELECT_CREDIT] = { 256.0f,64.0f };
	} else if (_SelectType == SELECT_SCENE) {
		m_Angle[SELECT_SCENE] += 2.0f;
		m_Angle2[SELECT_SCENE] = m_Angle[SELECT_SCENE] * (3.14f / 180.0f);
		//選択時座標が上下に動く
		m_PartsPos[SELECT_AREA] = m_PartsPos[SELECT_SCENE];
		m_Angle[NORMAL_SCENE] = 0.0f;
		m_Angle[SELECT_CREDIT] = 0.0f;
		//選択時サイズも少し変わる
		m_PartsSize[SELECT_SCENE] = { (sin(m_Angle2[SELECT_SCENE]) * 16.0f) + (256.0f),
			(sin(m_Angle2[SELECT_SCENE]) * 16.0f - 16.0f) + (64.0f) };
		m_PartsSize[SELECT_AREA] = m_PartsSize[SELECT_SCENE];
		m_PartsSize[NORMAL_SCENE] = { 256.0f,64.0f };
		m_PartsSize[SELECT_CREDIT] = { 256.0f,64.0f };
	} else {
		m_Angle[SELECT_CREDIT] += 2.0f;
		m_Angle2[SELECT_CREDIT] = m_Angle[SELECT_CREDIT] * (3.14f / 180.0f);
		//選択時座標が上下に動く
		m_PartsPos[SELECT_AREA] = m_PartsPos[SELECT_CREDIT];
		m_Angle[NORMAL_SCENE] = 0.0f;
		m_Angle[SELECT_SCENE] = 0.0f;
		//選択時サイズも少し変わる
		m_PartsSize[SELECT_CREDIT] = { (sin(m_Angle2[SELECT_CREDIT]) * 16.0f) + (256.0f),
			(sin(m_Angle2[SELECT_CREDIT]) * 16.0f - 16.0f) + (64.0f) };
		m_PartsSize[SELECT_AREA] = m_PartsSize[SELECT_CREDIT];
		m_PartsSize[NORMAL_SCENE] = { 256.0f,64.0f };
		m_PartsSize[SELECT_SCENE] = { 256.0f,64.0f };
	}

	for (auto i = 0; i < TitlePartsSprite.size(); i++) {
		TitlePartsSprite[i]->SetPosition(m_PartsPos[i]);
		TitlePartsSprite[i]->SetSize(m_PartsSize[i]);
	}
}