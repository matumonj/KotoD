#include "ThirdStageActor.h"
#include"Easing.h"
#include "ParticleEmitter.h"
#include "ImageManager.h"
#include "BackObj.h"
#include "Menu.h"
#include "SelectScene.h"
#include "Helper.h"
#include "HitStop.h"
//初期化
void ThirdStageActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	dxCommon->SetFullScreen(true);
	//共通の初期化
	BaseInitialize(dxCommon);
	//オーディオ
	Audio::GetInstance()->LoopWave(AUDIO_BATTLE, VolumManager::GetInstance()->GetBGMVolum() + 1.0f);

	//ポストエフェクト
	PlayPostEffect = true;
	//パーティクル全削除
	ParticleEmitter::GetInstance()->AllDelete();

	//各クラス
	Player::GetInstance()->InitState({ 0.0f,-2.0f,-30.0f });

	backScreen_ = IKESprite::Create(ImageManager::PLAY, { 0,0 });
	backScreen_->SetAddOffset(-0.0005f);
	backScreen_->SetSize({ 1280.0f,720.0f });
	//シーンチェンジャー
	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();
	SelectScene::GetIns()->SetTexSpeed(200.f);
	SelectScene::GetIns()->SetTexScl(20000.f);

	enemymanager = std::make_unique<EnemyManager>("THIRDSTAGE");
	//enemymanager->Initialize(dxCommon);
	text_ = make_unique<BossText>();
	text_->Initialize(dxCommon);
	text_->SelectText(TextManager::ANGER_TALK);
	camerawork->SetBoss(enemymanager->GetBoss());
	camerawork->SetCameraState(CAMERA_BOSSAPPEAR);
	camerawork->SetSceneName("THIRDSTAGE");
	camerawork->Update(camera);
	ui = std::make_unique<UI>();
	ui->Initialize();

	ui->SetBoss(enemymanager->GetBoss());
	BackObj::GetInstance()->Initialize();

	loadobj = std::make_unique<LoadStageObj>();
	loadobj->AllLoad("THIRDSTAGE");
	loadobj->LightSet(lightgroup);
	LoadStageObj::SetEnemyManager(enemymanager.get());

	m_SceneState = SceneState::IntroState;

	lightgroup->SetCircleShadowActive(0, true);
	lightgroup->SetCircleShadowActive(1, true);
	//SelectScene::GetIns()->ResetParama();
	//SelectScene::GetIns()->Init();
	menu = make_unique<Menu>();
	menu->Initialize();
}
//更新
void ThirdStageActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {

	//関数ポインタで状態管理
	if (menu->Pause()) {
		menu->Update();
		if (menu->ReturnSelect()) {
			SelectScene::GetIns()->SetTexSpeed(180.f);
			SelectScene::GetIns()->SetTexScl(12500.f);

			sceneChanger_->ChangeStart();
			sceneChanger_->ChangeScene("SELECT", SceneChanger::Reverse);
		}
		return;
	}
	if (!HitStop::GetInstance()->GetHitStop()) {
		(this->*stateTable[static_cast<size_t>(m_SceneState)])(camera);
	}
	sceneChanger_->Update();
	lightgroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(0, XMFLOAT3({ Player::GetInstance()->GetPosition().x, 0.0f, Player::GetInstance()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));
	//プレイヤー
	if (enemymanager->BossDestroy() && camerawork->GetFeedEnd()) {
		SceneSave::GetInstance()->SetClearFlag(kThirdStage, true);
		lightgroup->SetCircleShadowActive(0, false);
	}
	//ボス
	lightgroup->SetCircleShadowDir(1, XMVECTOR({ BosscircleShadowDir[0], BosscircleShadowDir[1], BosscircleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(1, XMFLOAT3({ enemymanager->GetBoss()->GetPosition().x, 	0.0f, 	enemymanager->GetBoss()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(1, XMFLOAT3(BosscircleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(1, XMFLOAT2(BosscircleShadowFactorAngle));
	lightgroup->Update();
	//if (SelectScene::GetIns()->GetCloseScl() < 10000.f)
	//	SelectScene::GetIns()->Upda();
	if (SelectScene::GetIns()->GetCloseScl() < 15000.f)
		SelectScene::GetIns()->Upda();

	postEffect->SetCloseRad(SelectScene::GetIns()->GetCloseIconRad());
	menu->Update();
	ui->Update();
	postEffect->SetCloseRad(SelectScene::GetIns()->GetCloseIconRad());
	HitStop::GetInstance()->Update();
}
//描画
void ThirdStageActor::Draw(DirectXCommon* dxCommon) {
	//描画方法
	//ポストエフェクトをかけるか
	if (PlayPostEffect) {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		BackDraw(dxCommon);
		FrontDraw(dxCommon);
		IKESprite::PreDraw();
		SelectScene::GetIns()->Draw_Sprite();
		IKESprite::PostDraw();
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
		FrontDraw(dxCommon);
		//ImGuiDraw(dxCommon);
		dxCommon->PostDraw();
	}
}
//解放
void ThirdStageActor::Finalize() {
}
//後ろの描画
void ThirdStageActor::BackDraw(DirectXCommon* dxCommon) {
	IKESprite::PreDraw();
	backScreen_->Draw();
	IKESprite::PostDraw();
	IKEObject3d::PreDraw();
	BackObj::GetInstance()->Draw(dxCommon);
	//パーティクル描画
	if (!camerawork->GetFeedEnd() && m_SceneState == SceneState::MainState) {
		if (!enemymanager->BossDestroy()) {
			ParticleEmitter::GetInstance()->BackDrawAll();
		}
	}

	Player::GetInstance()->Draw(dxCommon);

	////各クラスの描画
	if (!camerawork->GetFeedEnd()) {
		loadobj->Draw(dxCommon);
	}
	enemymanager->Draw(dxCommon);
	IKEObject3d::PostDraw();
}
//ポストエフェクトがかからない
void ThirdStageActor::FrontDraw(DirectXCommon* dxCommon) {
	//パーティクル描画
	if (!camerawork->GetFeedEnd() && m_SceneState == SceneState::MainState) {
		ParticleEmitter::GetInstance()->FlontDrawAll();
	}

	ParticleEmitter::GetInstance()->DeathDrawAll();
	//完全に前に書くスプライト
	IKESprite::PreDraw();
	if (m_SceneState == SceneState::MainState && !camerawork->GetFeedEnd()) {
		ui->Draw();
	}
	if (m_SceneState == SceneState::IntroState) {
		if ((camerawork->GetAppearType() == APPEAR_SEVEN) || (camerawork->GetAppearType() == APPEAR_EIGHT)) {
			text_->SpriteDraw(dxCommon);
		}
	}
	ClearText::GetInstance()->Draw();
	menu->Draw();
	sceneChanger_->Draw();
	IKESprite::PostDraw();
	camerawork->feedDraw();

}
//IMGuiの描画
void ThirdStageActor::ImGuiDraw(DirectXCommon* dxCommon) {
}
//登場シーン
void ThirdStageActor::IntroUpdate(DebugCamera* camera) {
	if (camerawork->GetAppearType() == APPEAR_SEVEN || camerawork->GetAppearType() == APPEAR_EIGHT) {
		text_->Display();
		//最初の言葉(怒り)
		if (m_AppState == AppState::ANGER_START) {
			if (m_AppTimer == 1) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
			}
			m_AppTimer++;
			text_->SelectText(TextManager::ANGER_TALK);
			if (m_AppTimer == 150) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
				m_AppState = AppState::ANGER_Third;
			}
		}
		//2つ目の言葉(怒り)
		else if (m_AppState == AppState::ANGER_Third) {
			m_AppTimer++;
			text_->SelectText(TextManager::ANGER_TALK2);
			if (m_AppTimer == 300) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
				m_AppState = AppState::JOY_START;
				enemymanager->DirSet(DIR_JOY);
			}
		}
		//最初の言葉(喜び)
		else if (m_AppState == AppState::JOY_START) {
			m_AppTimer++;
			text_->SelectText(TextManager::JOY_TALK);
			if (m_AppTimer == 450) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
				m_AppState = AppState::JOY_Third;
			}
		}
		//2個目の言葉(喜び)
		else if (m_AppState == AppState::JOY_Third) {
			m_AppTimer++;
			text_->SelectText(TextManager::JOY_TALK2);
			if (m_AppTimer == 600) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
				m_AppState = AppState::JOY_THIRD;
			}
		}
		//3個めの言葉(喜び)
		else if (m_AppState == AppState::JOY_THIRD) {
			m_AppTimer++;
			text_->SelectText(TextManager::JOY_TALK3);
			if (m_AppTimer == 750) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
				m_AppState = AppState::SELECT_EMO;
			}
		}
		//選択肢
		else if (m_AppState == AppState::SELECT_EMO) {
			text_->ChangeColor(1, { 1.0f,0.0f,0.0f,1.0f });
			text_->ChangeColor(2, { 0.5f,1.0f,0.1f,1.0f });
			text_->SelectText(TextManager::SELECT_TALK);
			if (Input::GetInstance()->TriggerButton(Input::Y)) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
				enemymanager->DirSet(DIR_ANGER);
				m_AppState = AppState::EMO_ANGER;
			} else if (Input::GetInstance()->TriggerButton(Input::X)) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
				enemymanager->DirSet(DIR_JOY);
				m_AppState = AppState::EMO_JOY;
			}
		}
		//イカリを選んだ場合
		else if (m_AppState == AppState::EMO_ANGER) {
			m_AppTimer++;
			for (int i = 0; i < 3; i++) {
				text_->ChangeColor(i, { 1.0f,1.0f,1.0f,1.0f });
			}
			text_->SelectText(TextManager::SELECT_ANGER);
			if (m_AppTimer == 900) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
				m_AppState = AppState::EMO_ANGER2;
				camerawork->SetApproach(true);
			}
		} else if (m_AppState == AppState::EMO_ANGER2) {
			for (int i = 0; i < 3; i++) {
				text_->ChangeColor(i, { 1.0f,0.0f,0.0f,1.0f });
			}
			text_->SelectText(TextManager::SELECT_ANGER2);
		}
		//よろこびを選んだ場合
		else if (m_AppState == AppState::EMO_JOY) {
			m_AppTimer++;
			for (int i = 0; i < 3; i++) {
				text_->ChangeColor(i, { 1.0f,1.0f,1.0f,1.0f });
			}
			text_->SelectText(TextManager::SELECT_JOY);
			if (m_AppTimer == 900) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
				m_AppState = AppState::EMO_JOY2;
				camerawork->SetApproach(true);
			}
		} else if (m_AppState == AppState::EMO_JOY2) {
			for (int i = 0; i < 3; i++) {
				text_->ChangeColor(i, { 1.0f,0.0f,0.0f,1.0f });
			}
			text_->SelectText(TextManager::SELECT_JOY2);
		}
	}

	//最後までテキストを見た
	if (enemymanager->GetEnemyFinishAppear()) {
		m_SceneState = SceneState::MainState;
		camerawork->SetCameraState(CAMERA_NORMAL);
	}

	//演出スキップ
	if (Input::GetInstance()->TriggerButton(Input::START)) {
		camerawork->SetCameraSkip(true);
	}

	if (camerawork->GetAppearEndF()) {
		m_SceneState = SceneState::MainState;
		camerawork->SetCameraState(CAMERA_NORMAL);
		enemymanager->SkipInitialize();
	}

	//各クラス更新
	BackObj::GetInstance()->Update();
	ParticleEmitter::GetInstance()->Update();
	Player::GetInstance()->AppearUpdate();
	enemymanager->AppearUpdate();

	camerawork->Update(camera);
}
//バトルシーン
void ThirdStageActor::MainUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
	//カメラワークのセット
	if (enemymanager->BossDestroy())
	{
		Audio::GetInstance()->StopWave(AUDIO_BATTLE);
		//フェード前
		if (!camerawork->GetFeedEnd()) {
			enemymanager->SetDeadThrow(true);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_BEFORE);
			Player::GetInstance()->DeathUpdate();
		}
		//フェード後
		else
		{
			m_DeathTimer++;
			PlayPostEffect = false;
			loadobj->AllClear();
			enemymanager->SetDeadThrow(false);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_AFTER_THIRD);
			Player::GetInstance()->DeathUpdateAfter(m_DeathTimer);
		}

		if (camerawork->GetEndDeath()) {
			sceneChanger_->ChangeStart();
			SelectScene::GetIns()->ResetParama();
			SelectScene::GetIns()->SetTexSpeed(180.f);
			SelectScene::GetIns()->SetTexScl(12500.f);

			sceneChanger_->ChangeScene("SELECT", SceneChanger::NonReverse);
		}
	} else
	{
		Player::GetInstance()->Update();
	}

	if (PlayerDestroy()) {
		Audio::GetInstance()->StopWave(AUDIO_BATTLE);
		SceneSave::GetInstance()->SetLoseFlag(SeceneCategory::kThirdStage, true);
		sceneChanger_->ChangeStart();
		//SelectScene::GetIns()->ResetParama();
		SelectScene::GetIns()->SetTexSpeed(180.f);
		SelectScene::GetIns()->SetTexScl(12500.f);

		sceneChanger_->ChangeSceneLose("GAMEOVER");
	}

	//音楽の音量が変わる
	VolumManager::GetInstance()->Update();

	//各クラス更新
	BackObj::GetInstance()->Update();


	enemymanager->BattleUpdate();
	loadobj->ThirdUpdate();
	ParticleEmitter::GetInstance()->Update();

	camerawork->Update(camera);

	XMFLOAT3 Position = enemymanager->GetBoss()->GetPosition();
	XMVECTOR tex2DPos = { Position.x, Position.y, Position.z };
	tex2DPos = Helper::GetInstance()->PosDivi(tex2DPos, camera->GetViewMatrix(), false);
	tex2DPos = Helper::GetInstance()->PosDivi(tex2DPos, camera->GetProjectionMatrix(), true);
	tex2DPos = Helper::GetInstance()->WDivision(tex2DPos, false);
	tex2DPos = Helper::GetInstance()->PosDivi(tex2DPos, camera->GetViewPort(), false);

	postEffect->SetRadCenter(XMFLOAT2(tex2DPos.m128_f32[0], tex2DPos.m128_f32[1]));
	postEffect->SetRadPower(camerawork->GetEffectPower());

	ClearText::GetInstance()->Update();
}
//撃破シーン
void ThirdStageActor::FinishUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
}