#include "FirstStageActor.h"
#include"Easing.h"
#include "ParticleEmitter.h"
#include "ImageManager.h"
#include <algorithm>
#include "BackObj.h"
#include "Menu.h"
#include "Helper.h"
#include "SelectScene.h"
#include "HitStop.h"

void FirstStageActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
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
	Player::GetInstance()->InitState({ 0.0f,-2.0f,-5.0f });

	backScreen_ = IKESprite::Create(ImageManager::PLAY, { 0,0 });
	backScreen_->SetAddOffset(-0.0005f);
	backScreen_->SetSize({ 1280.0f,720.0f });

	SkipUI = IKESprite::Create(ImageManager::SKIPUI, { 10,10 }, { 1.2f,1.2f,1.2f,1.f });
	//シーンチェンジャー
	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();

	enemymanager = std::make_unique<EnemyManager>("FIRSTSTAGE");
	//enemymanager->Initialize(dxCommon);
	camerawork->SetBoss(enemymanager->GetBoss());
	camerawork->SetCameraState(CAMERA_BOSSAPPEAR);
	camerawork->SetSceneName("FIRSTSTAGE");
	camerawork->SplineSet();
	camerawork->Update(camera);
	ui = std::make_unique<UI>();
	ui->Initialize();

	ui->SetBoss(enemymanager->GetBoss());
	BackObj::GetInstance()->Initialize();

	loadobj = std::make_unique<LoadStageObj>();
	loadobj->AllLoad("FIRSTSTAGE");
	loadobj->LightSet(lightgroup);
	LoadStageObj::SetEnemyManager(enemymanager.get());

	quarter_hp_ = 10;

	m_SceneState = SceneState::IntroState;

	lightgroup->SetCircleShadowActive(0, true);
	lightgroup->SetCircleShadowActive(1, true);

	menu = make_unique<Menu>();
	menu->Initialize();

	text_ = make_unique<BossText>();
	text_->Initialize(dxCommon);
	text_->Display();
	text_->SetWindowInvisible();

	//メッセージウィンドウ生成
	messagewindow_ = make_unique<MessageWindow>();
	messagewindow_->Initialize();
	messagewindow_->Display();
	messagewindow_->Invisible(false);

	SelectScene::GetIns()->SetTexSpeed(300.f);
	SelectScene::GetIns()->SetTexScl(30000.f);

	sutopon_color_ = { 1.f,1.f,1.f,0.5f };
	girl_color_ = { 1.3f,1.3f,1.3f,0.5f };
}

void FirstStageActor::Finalize() {
}

void FirstStageActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//関数ポインタで状態管理
	if (menu->Pause()) {
		menu->Update();
		if (menu->ReturnSelect()) {
			sceneChanger_->ChangeStart();
			SelectScene::GetIns()->ResetParama();
			SelectScene::GetIns()->SetTexSpeed(180.f);
			SelectScene::GetIns()->SetTexScl(12500.f);

			sceneChanger_->ChangeScene("SELECT", SceneChanger::Reverse);
		}
		return;
	}
	if (!HitStop::GetInstance()->GetHitStop()) {
		(this->*stateTable[static_cast<size_t>(m_SceneState)])(camera);
	}
	//プレイヤー
	if (enemymanager->BossDestroy() && camerawork->GetFeedEnd()) {
		SceneSave::GetInstance()->SetClearFlag(kFirstStage, true);
		lightgroup->SetCircleShadowActive(0, false);
	}
	//プレイヤー
	lightgroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(0, XMFLOAT3({ Player::GetInstance()->GetPosition().x, 0.0f, Player::GetInstance()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));
	//ボス
	lightgroup->SetCircleShadowDir(1, XMVECTOR({ BosscircleShadowDir[0], BosscircleShadowDir[1], BosscircleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(1, XMFLOAT3({ enemymanager->GetBoss()->GetPosition().x, 	0.0f, 	enemymanager->GetBoss()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(1, XMFLOAT3(BosscircleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(1, XMFLOAT2(BosscircleShadowFactorAngle));
	lightgroup->Update();
	sceneChanger_->Update();
	ui->Update();
	menu->Update();
	messagewindow_->Update(girl_color_, sutopon_color_);
	if (SelectScene::GetIns()->GetCloseScl() < 20000.f) {
		SelectScene::GetIns()->Upda();
	}
	postEffect->SetCloseRad(SelectScene::GetIns()->GetCloseIconRad());
	HitStop::GetInstance()->Update();
}

void FirstStageActor::Draw(DirectXCommon* dxCommon) {
	//描画方法
	//ポストエフェクトをかけるか
	if (PlayPostEffect) {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		BackDraw(dxCommon);
		FrontDraw(dxCommon);
		postEffect->PostDrawScene(dxCommon->GetCmdList());

		dxCommon->PreDraw();
		postEffect->Draw(dxCommon->GetCmdList());
		/*	postEffect->ImGuiDraw();
			camerawork->ImGuiDraw();*/
		dxCommon->PostDraw();
	} else {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		postEffect->Draw(dxCommon->GetCmdList());
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		BackDraw(dxCommon);
		FrontDraw(dxCommon);
		//camerawork->ImGuiDraw();
		dxCommon->PostDraw();
	}
}

void FirstStageActor::FrontDraw(DirectXCommon* dxCommon) {

	if (tolk_F == true) {
		IKESprite::PreDraw();
		messagewindow_->Draw();
		IKESprite::PostDraw();
		text_->SpriteDraw(dxCommon);
	}
	//パーティクル描画
	if (!camerawork->GetFeedEnd() && m_SceneState == SceneState::MainState) {
		ParticleEmitter::GetInstance()->FlontDrawAll();
	}

	ParticleEmitter::GetInstance()->DeathDrawAll();

	IKESprite::PreDraw();

	if (tolk_F == false) {
		if (m_SceneState == SceneState::MainState && !camerawork->GetFeedEnd()) {
			ui->Draw();
		}
	}
	if (m_SceneState == SceneState::IntroState) {
		SkipUI->Draw();
	}
	IKESprite::PostDraw();
	ClearText::GetInstance()->Draw();
	menu->Draw();
	sceneChanger_->Draw();
	camerawork->feedDraw();
	IKESprite::PreDraw();
	SelectScene::GetIns()->Draw_Sprite();
	IKESprite::PostDraw();
}

void FirstStageActor::BackDraw(DirectXCommon* dxCommon) {

	if (tolk_F == true) {
		IKESprite::PreDraw();
		messagewindow_->Draw();
		IKESprite::PostDraw();
		text_->SpriteDraw(dxCommon);
	}
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

void FirstStageActor::IntroUpdate(DebugCamera* camera) {
	//演出スキップ
	if (Input::GetInstance()->TriggerButton(Input::A)) {
		camerawork->SetCameraSkip(true);
		tolk_F = false;
		m_AppTimer = 0;
	}

	if (camerawork->GetAppearEndF()) {
		_AppState = APP_END;
		m_AppTimer = 0;
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

	m_AppTimer++;
	if (m_AppTimer == 400) {
		_AppState = APP_NOTICE;
	} else if (m_AppTimer == 580) {
		_AppState = APP_VANISH;
	}
	text_->Display();
	messagewindow_->Display();
	//テキスト関係
	if (m_AppTimer == 1) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::Name_Cap::CAP1);
	} else if (m_AppTimer == 150) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::Name_Cap::SUTO1);
		sutopon_color_ = { 1.f,1.f,1.f,1.f };
		text_->ChangeColor({ 0.f,1.f,1.f,1.f });
	} else if (m_AppTimer == 300) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::Name_Cap::CAP2);
		sutopon_color_ = { 1.f,1.f,1.f,0.5f };
		text_->ChangeColor({ 1.f,1.f,1.f,1.f });
	} else if (m_AppTimer == 400) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::Name_Cap::SUTO2);
		sutopon_color_ = { 1.f,1.f,1.f,1.f };
		text_->ChangeColor({ 0.f,1.f,1.f,1.f });
	} else if (m_AppTimer == 500) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::Name_Cap::CAP3);
		text_->ChangeColor({ 1.f,1.f,1.f,1.f });
	} else if (m_AppTimer >= 600) {
		tolk_F = false;
		m_AppTimer = 0;
		camerawork->SetCameraSkip(true);
	}
}

void FirstStageActor::MainUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
	CheckHp();
	TalkUpdate();
	if (tolk_F != false) { return; }
	//カメラワークのセット
	if (enemymanager->BossDestroy()) {
		Audio::GetInstance()->StopWave(AUDIO_BATTLE);
		//フェード前
		if (!camerawork->GetFeedEnd()) {
			enemymanager->SetDeadThrow(true);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_BEFORE);
			Player::GetInstance()->DeathUpdate();
		}
		//フェード後
		else {
			m_DeathTimer++;
			PlayPostEffect = false;
			enemymanager->SetDeadThrow(false);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_AFTER_FIRST);
			loadobj->AllClear();
			Player::GetInstance()->DeathUpdateAfter(m_DeathTimer);
		}

		if (camerawork->GetEndDeath()) {
			sceneChanger_->ChangeStart();
			SelectScene::GetIns()->ResetParama();
			SelectScene::GetIns()->SetTexSpeed(180.f);
			SelectScene::GetIns()->SetTexScl(12500.f);
			sceneChanger_->ChangeScene("SELECT", SceneChanger::NonReverse);

		}
	}

	else {
		Player::GetInstance()->Update();
	}

	if (PlayerDestroy()) {
		Audio::GetInstance()->StopWave(AUDIO_BATTLE);
		SceneSave::GetInstance()->SetLoseFlag(SeceneCategory::kFirstStage, true);
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
	loadobj->FirstUpdate();
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

void FirstStageActor::FinishUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
}

void FirstStageActor::CheckHp() {
	boss_hp_ = enemymanager->GetHp();
	if (boss_hp_ <= quarter_hp_) {
		if (isfirst) { return; }
		enemymanager->UpdateStop();
		tolk_F = true;
		isfirst = true;
	} else {
		tolk_F = false;
	}
}

void FirstStageActor::TalkUpdate() {
	if (tolk_F != true) { return; }


	m_AppTimer++;
	text_->Display();
	messagewindow_->Invisible(true);
	if (m_AppTimer == 2) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::Name_First::CHARGE1);
		sutopon_color_ = { 1.f,1.f,1.f,0.5f };
		text_->ChangeColor({ 1.f,1.f,1.f,1.f });
	} else if (m_AppTimer == 150) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::Name_First::CHARGE2);
		sutopon_color_ = { 1.f,1.f,1.f,0.5f };
		girl_color_ = { 1.3f,1.3f,1.3f,1.f };
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
	} else if (m_AppTimer == 300) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::Name_First::CHARGE3);
		text_->ChangeColor({ 0.f,1.f,1.f,1.f });
		sutopon_color_ = { 1.f,1.f,1.f,1.f };
		girl_color_ = { 1.3f,1.3f,1.3f,0.5f };
	} else if (m_AppTimer == 450) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::Name_First::CHARGE4);
	} else if (m_AppTimer == 625) {
		quarter_hp_ = -100.f;
	}
}
