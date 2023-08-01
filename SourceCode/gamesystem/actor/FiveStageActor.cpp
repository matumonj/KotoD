#include "FiveStageActor.h"
#include"Easing.h"
#include "ParticleEmitter.h"
#include "ImageManager.h"
#include <algorithm>
#include "BackObj.h"
#include "Menu.h"
#include "Helper.h"
#include "SelectScene.h"
#include "HitStop.h"

void FiveStageActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup)
{
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

	enemymanager = std::make_unique<EnemyManager>("FIVESTAGE");
	camerawork->SetBoss(enemymanager->GetBoss());
	camerawork->SetCameraState(CAMERA_BOSSAPPEAR);
	camerawork->SetSceneName("FIVESTAGE");
	camerawork->SplineSet();
	camerawork->Update(camera);

	SelectScene::GetIns()->SetTexSpeed(400.f);
	SelectScene::GetIns()->SetTexScl(35000.f);

	ui = std::make_unique<UI>();
	ui->Initialize();

	ui->SetBoss(enemymanager->GetBoss());
	BackObj::GetInstance()->Initialize();

	loadobj = std::make_unique<LoadStageObj>();
	loadobj->AllLoad("FIRSTSTAGE");
	loadobj->LightSet(lightgroup);
	LoadStageObj::SetEnemyManager(enemymanager.get());

	m_SceneState = SceneState::IntroState;

	lightgroup->SetCircleShadowActive(0, true);
	lightgroup->SetCircleShadowActive(1, true);

	menu = make_unique<Menu>();
	menu->Initialize();
}

void FiveStageActor::Finalize() {
}

void FiveStageActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
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

	enemymanager->SetGhost(loadobj->GetGhost());

	if (!HitStop::GetInstance()->GetHitStop()) {
		(this->*stateTable[static_cast<size_t>(m_SceneState)])(camera);
	}
	//プレイヤー
	if (enemymanager->BossDestroy() && camerawork->GetFeedEnd()) {
		SceneSave::GetInstance()->SetClearFlag(kSixStage, true);
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

	if (SelectScene::GetIns()->GetCloseScl() < 25000.f)
	{
		SelectScene::GetIns()->Upda();
	}



	postEffect->SetCloseRad(SelectScene::GetIns()->GetCloseIconRad());

	sceneChanger_->Update();
	ui->Update();
	menu->Update();
	HitStop::GetInstance()->Update();
}

void FiveStageActor::Draw(DirectXCommon* dxCommon) {
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

void FiveStageActor::FrontDraw(DirectXCommon* dxCommon)
{
	if (m_SceneState == SceneState::IntroState) {
		ParticleEmitter::GetInstance()->IntroDraw();
	}
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
		SkipUI->Draw();
	}
	IKESprite::PostDraw();
	ClearText::GetInstance()->Draw();
	menu->Draw();
	sceneChanger_->Draw();
	camerawork->feedDraw();
}

void FiveStageActor::BackDraw(DirectXCommon* dxCommon) {
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

void FiveStageActor::IntroUpdate(DebugCamera* camera) {
	//演出スキップ
	if (Input::GetInstance()->TriggerButton(Input::A)) {
		camerawork->SetCameraSkip(true);
	}

	if (camerawork->GetAppearEndF()) {
		enemymanager->SkipInitialize();
		_AppState = APP_END;
		m_SceneState = SceneState::MainState;
		camerawork->SetCameraState(CAMERA_NORMAL);

	}

	//各クラス更新
	BackObj::GetInstance()->Update();
	ParticleEmitter::GetInstance()->Update();
	Player::GetInstance()->DarkAppearUpdate(m_AppTimer);
	enemymanager->AppearUpdate();
	camerawork->SetLastTimer(m_AppTimer);
	camerawork->Update(camera);
	m_AppTimer++;
}

void FiveStageActor::MainUpdate(DebugCamera* camera) {
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
			camerawork->SetCameraState(CAMERA_BOSSDEAD_AFTER_FIVE);
			Player::GetInstance()->DeathUpdateAfter(m_DeathTimer);
			if (Input::GetInstance()->TriggerButton(Input::A)) {
				camerawork->SetEndDeath(true);
			}
		}

		if (camerawork->GetEndDeath()) {
			sceneChanger_->ChangeStart();
			SelectScene::GetIns()->ResetParama();
			SelectScene::GetIns()->SetTexSpeed(180.f);
			SelectScene::GetIns()->SetTexScl(12500.f);
			sceneChanger_->ChangeScene("SELECT", SceneChanger::NonReverse);
		}
	} else {
		Player::GetInstance()->Update();
	}

	if (PlayerDestroy()) {
		Audio::GetInstance()->StopWave(AUDIO_BATTLE2);
		SceneSave::GetInstance()->SetLoseFlag(SeceneCategory::kFiveStage, true);
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
	loadobj->SecondUpdate();
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

void FiveStageActor::FinishUpdate(DebugCamera* camera) {
	
}

void FiveStageActor::ImGuiDraw() {
	//Player::GetInstance()->ImGuiDraw();
	//camerawork->ImGuiDraw();
}