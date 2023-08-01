#include "SevenStageActor.h"
#include"Easing.h"
#include "ParticleEmitter.h"
#include "ImageManager.h"
#include "BackObj.h"
#include "Menu.h"
#include "SelectScene.h"
#include "Helper.h"
#include "HitStop.h"
//初期化
void SevenStageActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
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

	enemymanager = std::make_unique<EnemyManager>("SEVENSTAGE");
	//enemymanager->Initialize(dxCommon);
	text_ = make_unique<BossText>();
	text_->Initialize(dxCommon);
	text_->SelectText(TextManager::LAST_TALK_FIRST);
	camerawork->SetBoss(enemymanager->GetBoss());
	camerawork->SetCameraState(CAMERA_BOSSAPPEAR);
	camerawork->SetSceneName("SEVENSTAGE");
	camerawork->SplineSet();
	camerawork->Update(camera);
	ui = std::make_unique<UI>();
	ui->Initialize();

	//SelectScene::GetIns()->Init();
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

	messagewindow_ = make_unique<MessageWindow>();
	messagewindow_->Initialize();
	messagewindow_->Display();
	WhoRead("sutopon");
}
//更新
void SevenStageActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//関数ポインタで状態管理
	if (menu->Pause()) {
		menu->Update();
		if (menu->ReturnSelect()) {
			SelectScene::GetIns()->ResetParama();
			sceneChanger_->ChangeStart();
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
		SceneSave::GetInstance()->SetClearFlag(kSevenStage, true);
	}

	lightgroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(0, XMFLOAT3({ Player::GetInstance()->GetPosition().x, -2.0f, Player::GetInstance()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));

	//ボス
	lightgroup->SetCircleShadowDir(1, XMVECTOR({ BosscircleShadowDir[0], BosscircleShadowDir[1], BosscircleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(1, XMFLOAT3({ enemymanager->GetBoss()->GetPosition().x, 	0.0f, 	enemymanager->GetBoss()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(1, XMFLOAT3(BosscircleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(1, XMFLOAT2(BosscircleShadowFactorAngle));

	lightgroup->Update();
	sceneChanger_->Update();

	messagewindow_->Update(girl_color_, sutopon_color_);

	if (SelectScene::GetIns()->GetCloseScl() < 10000.f)
		SelectScene::GetIns()->Upda();
	ui->Update();
	menu->Update();
	postEffect->SetCloseRad(SelectScene::GetIns()->GetCloseIconRad());
	HitStop::GetInstance()->Update();
}
//描画
void SevenStageActor::Draw(DirectXCommon* dxCommon) {
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
		dxCommon->PostDraw();
	} else {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		postEffect->Draw(dxCommon->GetCmdList());
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		BackDraw(dxCommon);
		FrontDraw(dxCommon);
		dxCommon->PostDraw();
	}
}
//解放
void SevenStageActor::Finalize() {
}
//後ろの描画
void SevenStageActor::BackDraw(DirectXCommon* dxCommon) {
	IKESprite::PreDraw();
	backScreen_->Draw();
	IKESprite::PostDraw();
	IKEObject3d::PreDraw();
	BackObj::GetInstance()->Draw(dxCommon);
	//パーティクル描画
	if (!camerawork->GetFeedEnd() && m_SceneState == SceneState::MainState) {
		if (!enemymanager->BossDestroy()) {
			if (camerawork->GetCameraState() != CAMERA_BOSS_STRONG) {
				ParticleEmitter::GetInstance()->BackDrawAll();
			}
		}
	}

	if (!camerawork->GetChangeStrong()) {
		Player::GetInstance()->Draw(dxCommon);
	}
	////各クラスの描画
	if (!camerawork->GetFeedEnd()) {
		if (camerawork->GetCameraState() != CAMERA_BOSS_STRONG) {
			loadobj->Draw(dxCommon);
		}
	}
	enemymanager->Draw(dxCommon);
	IKEObject3d::PostDraw();
}
//ポストエフェクトがかからない
void SevenStageActor::FrontDraw(DirectXCommon* dxCommon) {
	//パーティクル描画
	if (!camerawork->GetFeedEnd() && m_SceneState == SceneState::MainState) {
		if (camerawork->GetCameraState() != CAMERA_BOSS_STRONG) {
			ParticleEmitter::GetInstance()->FlontDrawAll();
		}
	}

	ParticleEmitter::GetInstance()->DeathDrawAll();
	//完全に前に書くスプライト
	IKESprite::PreDraw();
	if (m_SceneState == SceneState::MainState && !camerawork->GetFeedEnd() && camerawork->GetCameraState() == CAMERA_NORMAL) {
		ui->Draw();
	}
	if (m_SceneState == SceneState::IntroState || camerawork->GetFeedEnd()) {
		SkipUI->Draw();
	}
	if ((m_SceneState == SceneState::IntroState) ||
		(m_SceneState == SceneState::MainState && (camerawork->GetChangeStrong() || camerawork->GetCameraState() == CAMERA_BOSSDEAD_AFTER_SEVEN))) {
		messagewindow_->SetNotBlack();
		if (messagewindow_->DisplayCheck()) {
			text_->SpriteDraw(dxCommon);
		}
		IKESprite::PreDraw();
		messagewindow_->Draw();
		IKESprite::PostDraw();
	}
	menu->Draw();
	sceneChanger_->Draw();
	IKESprite::PostDraw();
	IKESprite::PreDraw();
	//blackwindow->Draw();
	camerawork->feedDraw();
	//	SelectScene::GetIns()->Draw_Sprite();
	IKESprite::PostDraw();
}
//IMGuiの描画
void SevenStageActor::ImGuiDraw(DirectXCommon* dxCommon) {
	//Player::GetInstance()->ImGuiDraw();
}
//登場シーン
void SevenStageActor::IntroUpdate(DebugCamera* camera) {

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
	//テキスト読み
	TextRead();
	//各クラス更新
	Player::GetInstance()->LastAppearUpdate(m_AppTimer);
	BackObj::GetInstance()->Update();
	ParticleEmitter::GetInstance()->Update();
	enemymanager->AppearUpdate();
	camerawork->SetLastTimer(m_AppTimer);
	camerawork->Update(camera);
}
//バトルシーン
void SevenStageActor::MainUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
	//覚醒シーンに入る
	if (enemymanager->GetEnemyStrong() && !camerawork->GetCameraStrong()) {
		if (!camerawork->GetFeedF()) {
			camerawork->SetFeedF(true);
			camerawork->SetCameraState(CAMERA_BOSS_STRONG);
			camerawork->SetCameraStrong(true);
		}
	}

	if (camerawork->GetCameraState() == CAMERA_BOSS_STRONG) {
		//一回目のフェードの後に位置を初期化する
		if (camerawork->GetChangeStrong()) {
			Player::GetInstance()->AwakeInit();
			enemymanager->DeleteObj();
			loadobj->AwakeInit();
			text_->Display();
			m_AwakeTimer++;
			AwakeText();
		}
		//二回目のフェードが終わるとバトルモードに戻る
		if (camerawork->GetEndStrong()) {
			Player::GetInstance()->SetPosition({ 0.0f,0.0f,-5.0f });
			camerawork->SetCameraState(CAMERA_NORMAL);
		}
	}
	//カメラワークのセット
	if (enemymanager->BossDestroy()) {
		//フェード前
		if (!camerawork->GetFeedEnd()) {
			enemymanager->SetDeadThrow(true);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_BEFORE);
		}
		//フェード後
		else {
			loadobj->AllClear();
			text_->Display();
			m_EndTimer++;
			PlayPostEffect = false;
			enemymanager->SetDeadThrow(false);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_AFTER_SEVEN);
			camerawork->SetEndTimer(m_EndTimer);
			Player::GetInstance()->LastDeadUpdate(m_EndTimer);
			DeathText();

			if (Input::GetInstance()->TriggerButton(Input::A)) {
				camerawork->SetEndDeath(true);
			}
		}

		if (camerawork->GetEndDeath()) {
			sceneChanger_->ChangeStart();
			//SelectScene::GetIns()->ResetParama();
			sceneChanger_->ChangeScene("ENDROLL", SceneChanger::NonReverse);
			Audio::GetInstance()->StopWave(AUDIO_BATTLE2);
		}

		Player::GetInstance()->DeathUpdate();
	} else {
		if (camerawork->GetCameraState() == CAMERA_NORMAL)
			Player::GetInstance()->Update();
		else
			Player::GetInstance()->DeathUpdate();
	}

	if (PlayerDestroy()) {
		Audio::GetInstance()->StopWave(AUDIO_BATTLE2);
		SceneSave::GetInstance()->SetLoseFlag(SeceneCategory::kSevenStage, true);
		sceneChanger_->ChangeStart();
		//SelectScene::GetIns()->ResetParama();
		sceneChanger_->ChangeSceneLose("GAMEOVER");
	}

	//音楽の音量が変わる
	VolumManager::GetInstance()->Update();

	//各クラス更新
	BackObj::GetInstance()->Update();
	if (camerawork->GetCameraState() == CAMERA_NORMAL) {
		enemymanager->BattleUpdate();
	} else {
		enemymanager->Awake();
	}

	loadobj->SevenUpdate();
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

}
//撃破シーン
void SevenStageActor::FinishUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
}
//テキスト関係
void SevenStageActor::TextRead() {
	text_->Display();
	m_AppTimer++;
	if (m_AppTimer == 240) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_SECOND);
		WhoRead("sutopon");
	} else if (m_AppTimer == 350) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_THIRD);
		WhoRead("girl");
	} else if (m_AppTimer == 500) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_FOURTH);
		WhoRead("sutopon");
	} else if (m_AppTimer == 650) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_FIVE);
		WhoRead("boss");
	} else if (m_AppTimer == 800) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_SIX);
		WhoRead("sutopon");
	} else if (m_AppTimer == 950) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_SEVEN);
		WhoRead("boss");
	} else if (m_AppTimer == 1100) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_EIGHT);
	} else if (m_AppTimer == 1250) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_NINE);
	} else if (m_AppTimer == 1400) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_TEN);
	} else if (m_AppTimer == 1550) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_ELEVEN);
	} else if (m_AppTimer == 1700) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_TWELVE);
	} else if (m_AppTimer == 1850) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_THIRTEEN);
		WhoRead("sutopon");
	} else if (m_AppTimer == 2000) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_FOURTEEN);
	} else if (m_AppTimer == 2150) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_FIFETEEN);
		WhoRead("boss");
	} else if (m_AppTimer == 2300) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_SIXTEEN);
	} else if (m_AppTimer == 2450) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_SEVENTEEN);
		WhoRead("sutopon");
	} else if (m_AppTimer == 2600) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_EIGHTTEEN);
	} else if (m_AppTimer == 2750) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::LAST_TALK_NINETEEN);
	} else if (m_AppTimer == 2950) {
		camerawork->SetCameraSkip(true);
	}

	messagewindow_->DisplayCharacter(sutopon_color_);
}
//覚醒時のテキスト
void SevenStageActor::AwakeText() {
	if (m_AwakeTimer == 1) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		WhoRead("none");
		text_->SelectText(TextManager::AWAKE_FIRST);
	} else if (m_AwakeTimer == 200) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::AWAKE_SECOND);
	} else if (m_AwakeTimer == 400) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::AWAKE_THIRD);
	}
}
//ボス撃破時のエフェクト
void SevenStageActor::DeathText() {
	if (m_EndTimer == 1) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_FIRST);
		WhoRead("boss");
	} else if (m_EndTimer == 200) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_SECOND);
	} else if (m_EndTimer == 350) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_THIRD);
	} else if (m_EndTimer == 500) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_FOURTH);
		WhoRead("sutopon");
	} else if (m_EndTimer == 650) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_FIVE);
	} else if (m_EndTimer == 800) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_SIX);//ボス
		WhoRead("boss");
	} else if (m_EndTimer == 900) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_SEVEN);//ボス
	} else if (m_EndTimer == 1000) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_EIGHT);//コトコ
		WhoRead("girl");
	} else if (m_EndTimer == 1100) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_NINE);//ボス
		WhoRead("boss");
	} else if (m_EndTimer == 1200) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_TEN);//コトコ
		WhoRead("girl");
	} else if (m_EndTimer == 1300) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_ELEVEN);//コトコ
	} else if (m_EndTimer == 1400) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_TWELVE);//ボス
		WhoRead("boss");
	} else if (m_EndTimer == 1500) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_THIRTEEN);//コトコ
		WhoRead("girl");
	} else if (m_EndTimer == 1600) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_FOURTEEN);//すと
		WhoRead("sutopon");
	} else if (m_EndTimer == 1700) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::DEATH_FIFTEEN);//ボス
		WhoRead("boss");
	}
}
//どっちが呼んでいるか
void SevenStageActor::WhoRead(const string& name) {
	if (name == "sutopon") {
		sutopon_color_ = { 1.0f,1.0f,1.0f,1.0f };
		girl_color_ = { 0.5f,0.5f,0.5f,0.5f };
		text_->ChangeColor({ 0.f,1.f,1.f,1.f });
	} else if (name == "girl") {
		girl_color_ = { 1.3f,1.3f,1.3f,1.0f };
		sutopon_color_ = { 0.5f,0.5f,0.5f,0.5f };
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
	} else if (name == "boss") {
		sutopon_color_ = { 0.5f,0.5f,0.5f,0.5f };
		girl_color_ = { 0.5f,0.5f,0.5f,0.5f };
		text_->ChangeColor({ 1.f,1.f,1.f,1.f });
	} else if (name == "none") {
		sutopon_color_ = { 0.0f,0.0f,0.0f,0.0f };
		girl_color_ = { 0.0f,0.0f,0.0f,0.0f };
		text_->ChangeColor({ 1.f,1.f,1.f,1.f });
	}
}