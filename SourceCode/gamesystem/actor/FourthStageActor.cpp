#include "FourthStageActor.h"
#include "ParticleEmitter.h"
#include "ImageManager.h"
#include "BackObj.h"
#include "Menu.h"
#include "SelectScene.h"
#include "Helper.h"
#include <HungerGauge.h>
#include "HitStop.h"
const XMVECTOR kSkyBlue{ 0.f,1.f,1.f,1.f };
const XMVECTOR kPink{ 0.9f,0.6f,0.8f,1.f };

//初期化
void FourthStageActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	dxCommon->SetFullScreen(true);
	//共通の初期化
	BaseInitialize(dxCommon);
	//オーディオ
	Audio::GetInstance()->LoopWave(AUDIO_BATTLE, VolumManager::GetInstance()->GetBGMVolum() + 1.0f);
	//ポストエフェクト
	PlayPostEffect = true;

	//パーティクル全削除
	ParticleEmitter::GetInstance()->AllDelete();

	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();

	backScreen_ = IKESprite::Create(ImageManager::PLAY, { 0,0 });
	backScreen_->SetAddOffset(-0.0005f);
	backScreen_->SetSize({ 1280.0f,720.0f });

	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Food);
	apple = make_unique<IKEObject3d>();
	apple->Initialize();
	apple->SetModel(m_Model);
	apple->SetPosition({ 0,0,-15 });
	apple->SetScale({ 3, 3, 3 });
	photo[Photo_Out_Top] = IKESprite::Create(ImageManager::PHOTO_OUT, { 0,-360 });
	photo[Photo_Out_Under] = IKESprite::Create(ImageManager::PHOTO_OUT, { 0,1080 });
	for (int i = Photo_Out_Top; i <= Photo_Out_Under; i++) {
		photo[i]->SetSize({ 1280,360 });
	}

	//各クラス
	//プレイヤー
	Player::GetInstance()->InitState({ 0.0f,-2.0f,-70.0f });

	Player::GetInstance()->SetCanShot(false);
	Player::GetInstance()->MoveStop(true);
	//敵
	enemymanager = std::make_unique<EnemyManager>("FOURTHSTAGE");
	//カメラ
	camerawork->Update(camera);
	camerawork->SetBoss(enemymanager->GetBoss());
	camerawork->SetCameraState(CAMERA_BOSSAPPEAR);
	camerawork->SetSceneName("FOURTHSTAGE");
	camerawork->SplineSet();
	//UI
	ui = std::make_unique<UI>();
	ui->Initialize();
	ui->SetBoss(enemymanager->GetBoss());
	//背景
	BackObj::GetInstance()->Initialize();
	//ステージOBJ
	loadobj = std::make_unique<LoadStageObj>();
	loadobj->AllLoad("FIRSTSTAGE");
	loadobj->LightSet(lightgroup);
	LoadStageObj::SetEnemyManager(enemymanager.get());

	text_ = make_unique<BossText>();
	text_->Initialize(dxCommon);
	text_->SelectText(TextManager::Name_First::VIEWBOSS);

	SelectScene::GetIns()->SetTexSpeed(300.f);
	SelectScene::GetIns()->SetTexScl(30000.f);

	messagewindow_ = make_unique<MessageWindow>();
	messagewindow_->Initialize();
	messagewindow_->SetNotBlack();
	messagewindow_->Display();

	lightgroup->SetCircleShadowActive(0, true);
	lightgroup->SetCircleShadowActive(1, true);
	menu = make_unique<Menu>();
	menu->Initialize();
}
//更新
void FourthStageActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
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
	if (isVisible) { apple->Update(); }

	if (!HitStop::GetInstance()->GetHitStop()) {
		(this->*stateTable[static_cast<size_t>(m_SceneState)])(camera);
	}
	sceneChanger_->Update();
	camerawork->Update(camera);
	menu->Update();
	ui->Update();
	if (SelectScene::GetIns()->GetCloseScl() < 20000.f)
		SelectScene::GetIns()->Upda();

	postEffect->SetCloseRad(SelectScene::GetIns()->GetCloseIconRad());
	messagewindow_->Update(girl_color_, sutopon_color_);

	//ゲームクリアフラグ
	if (enemymanager->BossDestroy() && camerawork->GetFeedEnd()) {
		SceneSave::GetInstance()->SetClearFlag(kFourthStage, true);
	}
	lightgroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(0, XMFLOAT3({ Player::GetInstance()->GetPosition().x, 0.0f, Player::GetInstance()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));
	lightgroup->Update();
	HitStop::GetInstance()->Update();
}
//描画
void FourthStageActor::Draw(DirectXCommon* dxCommon) {
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
		//postEffect->ImGuiDraw();
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
void FourthStageActor::Finalize() {
}
//後ろの描画
void FourthStageActor::BackDraw(DirectXCommon* dxCommon) {
	IKESprite::PreDraw();
	backScreen_->Draw();
	IKESprite::PostDraw();

	IKEObject3d::PreDraw();
	BackObj::GetInstance()->Draw(dxCommon);
	if ((m_SceneState == SceneState::MainState) && !camerawork->GetFeedEnd()) {
		loadobj->Draw(dxCommon);
	}
	ParticleEmitter::GetInstance()->DeathDrawAll();
	//パーティクル描画
	if (camerawork->GetCameraState() != CameraState::CAMERA_BOSSAPPEAR &&
		camerawork->GetCameraState() != CameraState::CAMERA_BOSSDEAD_AFTER_FOURTH) {
		ParticleEmitter::GetInstance()->BackDrawAll();
		ParticleEmitter::GetInstance()->FlontDrawAll();
	}

	////各クラスの描画
	if (camerawork->GetAppearEndF()) {
		Player::GetInstance()->Draw(dxCommon);
	}
	enemymanager->Draw(dxCommon);
	if (m_SceneState == SceneState::MainState && !camerawork->GetFeedEnd()) {
		IKESprite::PreDraw();
		ui->Draw();
		IKESprite::PostDraw();
	}
	if (isVisible) {
		IKEObject3d::PreDraw();
		apple->Draw();
		IKEObject3d::PostDraw();
	}

	IKEObject3d::PostDraw();
}
//ポストエフェクトがかからない
void FourthStageActor::FrontDraw(DirectXCommon* dxCommon) {
	//パーティクル描画
	ParticleEmitter::GetInstance()->DeathDrawAll();
	//完全に前に書くスプライト
	if (m_SceneState == SceneState::IntroState) {
		if (messagewindow_->DisplayCheck()) {
			text_->SpriteDraw(dxCommon);
		}
		IKESprite::PreDraw();
		messagewindow_->Draw();
		IKESprite::PostDraw();

		IKESprite::PreDraw();
		for (int i = Photo_Out_Top; i <= Photo_Out_Under; i++) {
			photo[i]->Draw();
		}
		IKESprite::PostDraw();
	}
	ClearText::GetInstance()->Draw();
	menu->Draw();
	sceneChanger_->Draw();
	camerawork->feedDraw();
}
//IMGuiの描画
void FourthStageActor::ImGuiDraw(DirectXCommon* dxCommon) {
}


void FourthStageActor::ColEnemy(std::vector<InterEnemy*> enelist) {
	for (auto i = 0; i < enelist.size(); ++i) {
		for (auto j = 0; j < enelist.size(); ++j) {
			XMFLOAT3 ghostpos = enelist[i]->GetPosition();
			XMFLOAT3 ghostpos2 = enelist[j]->GetPosition();
			if ((i == j)) { continue; }
			if (Collision::SphereCollision(ghostpos, 1.5f, ghostpos2, 1.5f)) {
				enelist[i]->EnemyColNormal(ghostpos2);
				enelist[j]->EnemyColNormal(ghostpos);
			}
		}
	}
}

void FourthStageActor::IntroUpdate(DebugCamera* camera) {
	//演出スキップ
	if (Input::GetInstance()->TriggerButton(Input::A)) {
		camerawork->SetCameraSkip(true);
	}

	if (camerawork->GetAppearEndF()) {
		isVisible = false;
		m_SceneState = SceneState::MainState;
		Player::GetInstance()->SetCanShot(true);
		Player::GetInstance()->MoveStop(false);
		camerawork->SetCameraState(CAMERA_NORMAL);
		enemymanager->SkipInitialize();
	}

	//各クラス更新
	BackObj::GetInstance()->Update();
	ParticleEmitter::GetInstance()->Update();
	Player::GetInstance()->AppearUpdate();
	enemymanager->AppearUpdate();
	camerawork->Update(camera);
	if (!isShutter) {
		m_AppTimer++;
	} else {
		if (ShutterEffect()) {
			text_->SelectText(TextManager::TALK_NULL);
			if (m_AppTimer < 1000) {
				isVisible = true;
			}
			stopTime++;
			if (stopTime >= stopTimerMax) {
				if (ShutterFeed()) {
					ShutterReset();
				}
			}
		}
	}
	//テキスト関係
	text_->Display();
	if (m_AppTimer == 1) {
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_FIRST_T);
		girl_color_ = { 1.2f,1.2f,1.2f,1 };
	} else if (m_AppTimer == 200) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 0.0f,1.0f,0.0f,1.0f });
		text_->SelectText(TextManager::TALK_SECOND_T);
		girl_color_ = { 1.2f,1.2f,1.2f,0 };
	} else if (m_AppTimer == 400) {
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 1.0f,1.0f,1.0f,1.0f });
		text_->SelectText(TextManager::TALK_THIRD_T);
		girl_color_ = { 1.2f,1.2f,1.2f,1 };
	} else if (m_AppTimer == 600) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 0.0f,1.0f,0.0f,1.0f });
		text_->SelectText(TextManager::TALK_FOURTH_T);
		girl_color_ = { 1.2f,1.2f,1.2f,0 };
	} else if (m_AppTimer == 800) {
		isShutter = true;
		m_AppTimer++;
	} else if (m_AppTimer == 850) {
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_FIVE_T);
		girl_color_ = { 1.2f,1.2f,1.2f,1 };
	} else if (m_AppTimer == 1050) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 0.0f,1.0f,0.0f,1.0f });
		text_->SelectText(TextManager::TALK_SIX_T);
		girl_color_ = { 1.2f,1.2f,1.2f,0 };
	} else if (m_AppTimer == 1250) {
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_SEVEN_T);
		girl_color_ = { 1.2f,1.2f,1.2f,1 };
	} else if (m_AppTimer == 1450) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 0.0f,1.0f,0.0f,1.0f });
		text_->SelectText(TextManager::TALK_EIGHT_T);
		girl_color_ = { 1.2f,1.2f,1.2f,0 };
	} else if (m_AppTimer == 1650) {
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_NINE_T);
		girl_color_ = { 1.2f,1.2f,1.2f,1 };
	} else if (m_AppTimer == 1850) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 0.0f,1.0f,0.0f,1.0f });
		text_->SelectText(TextManager::TALK_TEN_T);
		girl_color_ = { 1.2f,1.2f,1.2f,0 };
	} else if (m_AppTimer == 2050) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		isVisible = false;
		shutterTimeMax = 60.0f;
		stopTimerMax = 100.0f;
		feedTimeMax = 10.0f;
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
		text_->SelectText(TextManager::TALK_ELEVEN_T);
		girl_color_ = { 1.2f,1.2f,1.2f,1 };
	} else if (m_AppTimer == 2250) {
		isShutter = true;
		m_AppTimer++;
	} else if (m_AppTimer == 2450) {
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_XII_T);
		girl_color_ = { 1.2f,1.2f,1.2f,1 };
	} else if (m_AppTimer == 2650) {
		text_->ChangeColor({ 0.0f,1.f,1.f,1.f });
		text_->SelectText(TextManager::TALK_XIII_T);
		girl_color_ = { 1.2f,1.2f,1.2f,0 };
		sutopon_color_ = { 1,1,1,1 };
	} else if (m_AppTimer == 2850) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 0.0f,1.0f,0.0f,1.0f });
		text_->SelectText(TextManager::TALK_XIV_T);
		sutopon_color_ = { 1,1,1,0 };
	} else if (m_AppTimer == 3050) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 0.8f,0.0f,0.0f,1.0f });
		text_->SelectText(TextManager::TALK_XV_T);
	} else if (m_AppTimer == 3250) {
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_XVI_T);
		girl_color_ = { 1.2f,1.2f,1.2f,1 };
	} else if (m_AppTimer == 3450) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 0.8f,0.0f,0.0f,1.0f });
		text_->SelectText(TextManager::TALK_XVII_T);
		girl_color_ = { 1.2f,1.2f,1.2f,0 };
	} else if (m_AppTimer == 3650) {
		text_->ChangeColor({ 0.9f,0.6f,0.8f,1.f });
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->SelectText(TextManager::TALK_XVIII_T);
		girl_color_ = { 1.2f,1.2f,1.2f,1 };
	} else if (m_AppTimer == 3850) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->ChangeColor({ 0.8f,0.0f,0.0f,1.0f });
		text_->SelectText(TextManager::TALK_XVIV_T);
		girl_color_ = { 1.2f,1.2f,1.2f,0 };
	} else if (m_AppTimer == 4050) {
		camerawork->SetCameraSkip(true);
	}
}

void FourthStageActor::MainUpdate(DebugCamera* camera) {

	if (enemymanager->BossDestroy()) {
		Audio::GetInstance()->StopWave(AUDIO_BATTLE);
		//フェード前
		if (!camerawork->GetFeedEnd()) {
			Player::GetInstance()->DeathUpdate();
			enemymanager->SetDeadThrow(true);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_BEFORE);
			apple->SetPosition({ Player::GetInstance()->GetPosition().x + 10.0f,10.0f,Player::GetInstance()->GetPosition().z });
		}
		//フェード後
		else {
			m_DeathTimer++;
			PlayPostEffect = false;
			isVisible = true;
			XMFLOAT3 pos = apple->GetPosition();
			pos.y -= 0.3f;
			pos.y = clamp(pos.y, 0.0f, 100.0f);
			apple->SetPosition(pos);
			HungerGauge::GetInstance()->SetNowHunger(0);
			HungerGauge::GetInstance()->Update();
			loadobj->AllClear();
			enemymanager->SetDeadThrow(false);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_AFTER_FOURTH);
			Player::GetInstance()->DeathUpdateAfter(m_DeathTimer);
		}

		if (camerawork->GetEndDeath()) {
			SelectScene::GetIns()->ResetParama();
			SelectScene::GetIns()->SetTexSpeed(180.f);
			SelectScene::GetIns()->SetTexScl(12500.f);
			sceneChanger_->ChangeStart();
			sceneChanger_->ChangeScene("SELECT", SceneChanger::NonReverse);

		}
	} else {
		Player::GetInstance()->Update();
	}

	if (PlayerDestroy()) {
		Audio::GetInstance()->StopWave(AUDIO_BATTLE);
		SceneSave::GetInstance()->SetLoseFlag(SeceneCategory::kFourthStage, true);
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
	ColEnemy(enemymanager->GetBulEnemy());
	loadobj->FourthUpdate();
	ParticleEmitter::GetInstance()->Update();

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

void FourthStageActor::FinishUpdate(DebugCamera* camera) {
}
bool FourthStageActor::ShutterEffect() {
	shutterTime += 1.0f / shutterTimeMax;
	shutterTime = clamp(shutterTime, 0.0f, 1.0f);

	shutterHight[0] = Ease(Out, Quad, shutterTime, -360, 0);
	shutterHight[1] = Ease(Out, Quad, shutterTime, 1080, 360);

	photo[Photo_Out_Top]->SetPosition({ 0,shutterHight[0] });
	photo[Photo_Out_Under]->SetPosition({ 0,shutterHight[1] });
	if (shutterTime == 1.0f) {
		return true;
	} else {
		return false;
	}
}

bool FourthStageActor::ShutterFeed() {
	//SEを鳴らす
	if (feedTimer == 0.0f) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Cemera.wav", VolumManager::GetInstance()->GetSEVolum());
	}
	feedTimer += 1.0f / feedTimeMax;
	float color = Ease(Out, Linear, feedTimer, 1.0f, 0.0f);
	photo[Photo_Out_Top]->SetColor({ 1,1,1, color });
	photo[Photo_Out_Under]->SetColor({ 1,1,1,color });
	feedTimer = clamp(feedTimer, 0.0f, 1.0f);
	if (feedTimer == 1.0f) {
		return true;
	} else {
		return false;
	}
}

void FourthStageActor::ShutterReset() {
	isShutter = false;
	stopTime = 0;
	shutterHight[0] = -360.0f;
	shutterHight[1] = 1080.0f;
	photo[Photo_Out_Top]->SetPosition({ 0,shutterHight[0] });
	photo[Photo_Out_Under]->SetPosition({ 0,shutterHight[1] });
	photo[Photo_Out_Top]->SetColor({ 1,1,1,1 });
	photo[Photo_Out_Under]->SetColor({ 1,1,1,1 });
	shutterTime = 0.0f;
	feedTimer = 0.0f;
}
