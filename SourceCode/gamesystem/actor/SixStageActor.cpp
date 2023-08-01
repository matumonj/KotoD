#include "SixStageActor.h"
#include"Easing.h"
#include "ParticleEmitter.h"
#include "ImageManager.h"
#include "BackObj.h"
#include "SelectScene.h"
#include "Helper.h"
#include "HitStop.h"
//初期化
void SixStageActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	dxCommon->SetFullScreen(true);
	//共通の初期化
	BaseInitialize(dxCommon);
	//オーディオ
	Audio::GetInstance()->LoopWave(AUDIO_BATTLE2, VolumManager::GetInstance()->GetBGMVolum() + 0.1f);

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

	enemymanager = std::make_unique<EnemyManager>("SIXSTAGE");
	//enemymanager->Initialize(dxCommon);
	text_ = make_unique<BossText>();
	text_->Initialize(dxCommon);
	text_->SelectText(TextManager::TALK_FIRST);
	camerawork->SetBoss(enemymanager->GetBoss());
	camerawork->SetCameraState(CAMERA_BOSSAPPEAR);
	camerawork->SetSceneName("SIXSTAGE");
	camerawork->SplineSet();
	camerawork->Update(camera);
	ui = std::make_unique<UI>();
	ui->Initialize();

	SelectScene::GetIns()->Init();
	menu = make_unique<Menu>();
	menu->Initialize();
	ui->SetBoss(enemymanager->GetBoss());
	BackObj::GetInstance()->Initialize();

	loadobj = std::make_unique<LoadStageObj>();
	loadobj->AllLoad("FIRSTSTAGE");
	loadobj->LightSet(lightgroup);
	LoadStageObj::SetEnemyManager(enemymanager.get());

	m_SceneState = SceneState::IntroState;

	lightgroup->SetCircleShadowActive(0, true);
	lightgroup->SetCircleShadowActive(1, true);

	//丸影のためのやつ
	lightgroup->SetDirLightActive(0, false);
	lightgroup->SetDirLightActive(1, false);
	lightgroup->SetDirLightActive(2, false);
	for (int i = 0; i < SPOT_NUM; i++) {
		lightgroup->SetSpotLightActive(i, true);
	}

	spotLightPos[0] = { 30,  10, 60 };
	spotLightPos[1] = { 30,  10,  0 };
	spotLightPos[2] = { -30, 10, 60 };
	spotLightPos[3] = { -30, 10,  0 };

	spotLightDir[0] = { 0, -1, 0 };
	spotLightDir[1] = { 0, -1, 0 };
	spotLightDir[2] = { 0, -1, 0 };
	spotLightDir[3] = { 0, -1, 0 };

	spotLightColor[0] = { 1, 1, 1 };
	spotLightColor[1] = { 1, 1, 1 };
	spotLightColor[2] = { 1, 1, 1 };
	spotLightColor[3] = { 1, 1, 1 };
}
//更新
void SixStageActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//関数ポインタで状態管理
	if (menu->Pause()) {
		menu->Update();
		if (menu->ReturnSelect()) {
			sceneChanger_->ChangeStart();
			SelectScene::GetIns()->ResetParama();

			sceneChanger_->ChangeScene("SELECT", SceneChanger::Reverse);
		}
		return;
	}
	if (!HitStop::GetInstance()->GetHitStop()) {
		(this->*stateTable[static_cast<size_t>(m_SceneState)])(camera);
	}
	sceneChanger_->Update();
	//プレイヤー
	if (enemymanager->BossDestroy() && camerawork->GetFeedEnd()) {
		SceneSave::GetInstance()->SetClearFlag(kFiveStage, true);
		lightgroup->SetCircleShadowActive(0, false);
	}
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
	if (SelectScene::GetIns()->GetCloseScl() < 10000.f)
		SelectScene::GetIns()->Upda();

	//スポットライトの動き
	MoveSpotLight();
	if (_AppState == APP_END) {
		//丸影のためのやつ
		lightgroup->SetDirLightActive(0, true);
		lightgroup->SetDirLightActive(1, true);
		lightgroup->SetDirLightActive(2, true);
		for (int i = 0; i < SPOT_NUM; i++) {
			lightgroup->SetSpotLightActive(i, false);
		}
	}

	///スポットライト
	for (int i = 0; i < SPOT_NUM; i++) {
		lightgroup->SetSpotLightDir(i, XMVECTOR({ spotLightDir[i].x,spotLightDir[i].y,spotLightDir[i].z,0 }));
		lightgroup->SetSpotLightPos(i, spotLightPos[i]);
		lightgroup->SetSpotLightColor(i, spotLightColor[i]);
		lightgroup->SetSpotLightAtten(i, XMFLOAT3(spotLightAtten));
		lightgroup->SetSpotLightFactorAngle(i, XMFLOAT2(spotLightFactorAngle));
	}
	lightgroup->Update();
	menu->Update();
	ui->Update();
	HitStop::GetInstance()->Update();
	postEffect->SetCloseRad(SelectScene::GetIns()->GetCloseIconRad());
}
//描画
void SixStageActor::Draw(DirectXCommon* dxCommon) {
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
void SixStageActor::Finalize() {
}
//後ろの描画
void SixStageActor::BackDraw(DirectXCommon* dxCommon) {
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
void SixStageActor::FrontDraw(DirectXCommon* dxCommon) {
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
	if (m_SceneState == SceneState::IntroState || camerawork->GetFeedEnd()) {
		SkipUI->Draw();
	}
	if (m_SceneState == SceneState::IntroState) {
		text_->SpriteDraw(dxCommon);
	}
	ClearText::GetInstance()->Draw();
	menu->Draw();
	sceneChanger_->Draw();
	camerawork->feedDraw();
	//SelectScene::GetIns()->Draw_Sprite();
	IKESprite::PostDraw();
}
//IMGuiの描画
void SixStageActor::ImGuiDraw(DirectXCommon* dxCommon) {
}
//登場シーン
void SixStageActor::IntroUpdate(DebugCamera* camera) {

	//演出スキップ
	if (Input::GetInstance()->TriggerButton(Input::A)) {
		camerawork->SetCameraSkip(true);
	}

	if (camerawork->GetAppearEndF()) {
		_AppState = APP_END;
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

	//テキスト関係
	text_->Display();
	if (m_AppTimer == 1) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_FIRST);
	} else if (m_AppTimer == 150) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_SECOND);
	} else if (m_AppTimer == 300) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_THIRD);
		text_->ChangeColor(0, { 1.0f,0.0f,0.0f,1.0f });
	} else if (m_AppTimer == 400) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_FOURTH);
		for (int i = 0; i < 3; i++) {
			text_->ChangeColor(i, { 1.0f,1.0f,0.0f,1.0f });
		}
	} else if (m_AppTimer == 500) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_FIVE);
	}
}
//バトルシーン
void SixStageActor::MainUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
	//カメラワークのセット
	if (enemymanager->BossDestroy()) {
		Audio::GetInstance()->StopWave(AUDIO_BATTLE2);
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
			loadobj->AllClear();
			enemymanager->SetDeadThrow(false);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_AFTER_SIX);
			Player::GetInstance()->DeathUpdateAfter(m_DeathTimer);
			if (Input::GetInstance()->TriggerButton(Input::A)) {
				camerawork->SetEndDeath(true);
			}
		}

		if (camerawork->GetEndDeath()) {
			sceneChanger_->ChangeStart();
			SelectScene::GetIns()->ResetParama();
			sceneChanger_->ChangeScene("SELECT", SceneChanger::NonReverse);

		}
	} else {
		Player::GetInstance()->Update();
	}

	if (PlayerDestroy()) {
		Audio::GetInstance()->StopWave(AUDIO_BATTLE2);
		SceneSave::GetInstance()->SetLoseFlag(SeceneCategory::kSixStage, true);
		sceneChanger_->ChangeStart();
		//SelectScene::GetIns()->ResetParama();
		sceneChanger_->ChangeSceneLose("GAMEOVER");
	}

	//音楽の音量が変わる
	VolumManager::GetInstance()->Update();

	//各クラス更新
	BackObj::GetInstance()->Update();


	enemymanager->BattleUpdate();
	loadobj->SixUpdate();
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
void SixStageActor::FinishUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
}
//スポットライトの動き
void SixStageActor::MoveSpotLight() {
	const float l_AddAngle = 5.0f;
	const float l_AddFrame = 0.5f;
	const float l_DirMax = 2.0f;
	const float l_DirMin = -2.0f;
	const float l_PosMax = 100.0f;
	const float l_PosMin = -100.0f;
	//sin波によって上下に動く
	if (_AppState == APP_START) {
		for (int i = 0; i < SPOT_NUM; i++) {
			m_Angle[i] += (l_AddAngle - (0.5f * i));
			m_Angle2[i] = m_Angle[i] * (3.14f / 180.0f);
		}

		spotLightDir[0].x = (sin(m_Angle2[0]) * 6.0f + (-2.0f));
		spotLightDir[0].z = (sin(m_Angle2[0]) * 6.0f + (-2.0f));
		spotLightDir[1].x = (sin(m_Angle2[1]) * 6.0f + (-2.0f));
		spotLightDir[1].z = (sin(m_Angle2[1]) * 6.0f + (2.0f));
		spotLightDir[2].x = (sin(m_Angle2[2]) * 6.0f + (2.0f));
		spotLightDir[2].z = (sin(m_Angle2[2]) * 6.0f + (-2.0f));
		spotLightDir[3].x = (sin(m_Angle2[3]) * 6.0f + (2.0f));
		spotLightDir[3].z = (sin(m_Angle2[3]) * 6.0f + (2.0f));

	} else if (_AppState == APP_NOTICE) {
		SpotSet(spotLightDir[0], { l_DirMin,{},l_DirMin }, l_AddFrame);
		SpotSet(spotLightDir[1], { l_DirMin,{},l_DirMax }, l_AddFrame);
		SpotSet(spotLightDir[2], { l_DirMax,{},l_DirMin }, l_AddFrame);
		SpotSet(spotLightDir[3], { l_DirMax,{},l_DirMax }, l_AddFrame);
	} else if (_AppState == APP_VANISH) {
		//角度
		SpotSet(spotLightDir[0], {}, l_AddFrame);
		SpotSet(spotLightDir[1], {}, l_AddFrame);
		SpotSet(spotLightDir[2], {}, l_AddFrame);
		SpotSet(spotLightDir[3], {}, l_AddFrame);
		//座標
		SpotSet(spotLightPos[0], { l_PosMax,spotLightPos[0].y,l_PosMax }, l_AddFrame);
		SpotSet(spotLightPos[1], { l_PosMax,spotLightPos[1].y,l_PosMin }, l_AddFrame);
		SpotSet(spotLightPos[2], { l_PosMin,spotLightPos[2].y,l_PosMax }, l_AddFrame);
		SpotSet(spotLightPos[3], { l_PosMin,spotLightPos[3].y,l_PosMin }, l_AddFrame);
	}
}
//スポットライト
void SixStageActor::SpotSet(XMFLOAT3& Pos, const XMFLOAT3& AfterPos, const float AddFrame) {
	Pos = { Ease(In,Cubic,AddFrame,Pos.x,AfterPos.x),
		Pos.y,
		Ease(In,Cubic,AddFrame,Pos.z,AfterPos.z),
	};
}
