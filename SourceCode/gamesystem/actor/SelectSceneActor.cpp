#include "SelectSceneActor.h"
#include "SceneManager.h"
#include "ParticleEmitter.h"
#include <HungerGauge.h>
#include "BackObj.h"
#include"SelectScene.h"
#include "Helper.h"
const XMVECTOR kSkyBlue{ 0.f,1.f,1.f,1.f };
const XMVECTOR kPink{ 0.9f,0.6f,0.8f,1.f };

//初期化
void SelectSceneActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	dxCommon->SetFullScreen(true);
	//共通の初期化
	BaseInitialize(dxCommon);
	Audio::GetInstance()->StopWave(AUDIO_TITLE);
	Audio::GetInstance()->StopWave(AUDIO_BATTLE);
	Audio::GetInstance()->StopWave(AUDIO_BATTLE2);
	//オーディオ
	Audio::GetInstance()->LoopWave(AUDIO_LOAD, VolumManager::GetInstance()->GetBGMVolum() + 0.5f);
	//ポストエフェクト
	PlayPostEffect = true;
	//パーティクル全削除
	ParticleEmitter::GetInstance()->AllDelete();

	//FirstStageクリアしたら初期化切る(各シーンごとにResetParamで)
	if(!SceneSave::GetInstance()->GetClearFlag(SeceneCategory::kFirstStage))
	SelectScene::GetIns()->Init();

	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();

	//各クラス
	//プレイヤー
	Player::GetInstance()->InitState({ 0.0f,5.0f,-70.0f });

	Player::GetInstance()->SetCanShot(false);
	Player::GetInstance()->MoveStop(true);
	//敵
	enemymanager = std::make_unique<EnemyManager>("THIRDSTAGE");
	//カメラ
	camerawork->Update(camera);
	camerawork->SetBoss(enemymanager->GetBoss());
	camerawork->SetCameraState(CAMERA_BOSSAPPEAR);
	camerawork->SetSceneName("FIRSTSTAGE");
	//UI
	ui = std::make_unique<UI>();
	ui->Initialize();
	ui->SetBoss(enemymanager->GetBoss());

	BackObj::GetInstance()->Initialize();

	//}
	//スポットライト
	lightgroup->SetDirLightActive(0, false);
	lightgroup->SetDirLightActive(1, false);
	lightgroup->SetDirLightActive(2, false);

	lightgroup->SetSpotLightActive(0, true);

	spotLightPos[0] = {SelectScene::GetIns()->GetNowSelePos().x,10.0f,SelectScene::GetIns()->GetNowSelePos().z};
	spotLightDir[0] = { 0, -1, 0 };
	spotLightColor[0] = { 1, 1, 1 };

	ClearText::GetInstance()->Initialize();
}
//更新
void SelectSceneActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {


	const int IntervalTextC = 200;

	Input* input = Input::GetInstance();

	//プレイヤー
	lightgroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(0, XMFLOAT3({ Player::GetInstance()->GetPosition().x, 0.0f, Player::GetInstance()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));

	//ボス
	lightgroup->SetCircleShadowDir(1, XMVECTOR({ BosscircleShadowDir[0], BosscircleShadowDir[1], BosscircleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(1, XMFLOAT3({ enemymanager->GetBoss()->GetPosition().x, 0.0f, 	enemymanager->GetBoss()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(1, XMFLOAT3(BosscircleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(1, XMFLOAT2(BosscircleShadowFactorAngle));

	
	//音楽の音量が変わる
	Audio::GetInstance()->VolumChange(0, VolumManager::GetInstance()->GetBGMVolum());
	VolumManager::GetInstance()->Update();
	ui->Update();


	XMFLOAT3 Position = enemymanager->GetBoss()->GetPosition();
	XMVECTOR tex2DPos = { Position.x, Position.y, Position.z };
	tex2DPos = Helper::GetInstance()->PosDivi(tex2DPos, camera->GetViewMatrix(), false);
	tex2DPos = Helper::GetInstance()->PosDivi(tex2DPos, camera->GetProjectionMatrix(), true);
	tex2DPos = Helper::GetInstance()->WDivision(tex2DPos, false);
	tex2DPos = Helper::GetInstance()->PosDivi(tex2DPos, camera->GetViewPort(), false);

	postEffect->SetRadCenter(XMFLOAT2(tex2DPos.m128_f32[0], tex2DPos.m128_f32[1]));
	postEffect->SetRadPower(camerawork->GetEffectPower());
	postEffect->SetCloseRad(SelectScene::GetIns()->GetCloseIconRad());

	sceneChanger_->Update();
	//BackObj::GetInstance()->Update();

	SelectScene::GetIns()->Upda();
	camerawork->SetEye({ SelectScene::GetIns()->GetPedestalPos().x,
						SelectScene::GetIns()->GetPedestalPos().y + 20.f,
						SelectScene::GetIns()->GetPedestalPos().z - 60.f
		});
	camerawork->SetTarget(SelectScene::GetIns()->GetPedestalPos());

	camerawork->DefUpda(camera);

	lightgroup->SetSpotLightDir(0, XMVECTOR({0,-1,0,0 }));
	lightgroup->SetSpotLightPos(0, {SelectScene::GetIns()->GetNowSelePos().x, 30.0f, SelectScene::GetIns()->GetNowSelePos().z});
	lightgroup->SetSpotLightColor(0,{1,1,1});
	lightgroup->SetSpotLightAtten(0, {0.001f,0.f,0.001f});
	lightgroup->SetSpotLightFactorAngle(0, XMFLOAT2(spotLightFactorAngle));
	
	lightgroup->Update();
	ParticleEmitter::GetInstance()->Update();
}
//描画
void SelectSceneActor::Draw(DirectXCommon* dxCommon) {
	//描画方法
	//ポストエフェクトをかけるか
	if (PlayPostEffect) {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		//BackObj::GetInstance()->Draw(dxCommon);
		BackDraw(dxCommon);
		
		IKESprite::PreDraw();
		SelectScene::GetIns()->Draw_SpriteBack();
		IKESprite::PostDraw();
		postEffect->PostDrawScene(dxCommon->GetCmdList());

		dxCommon->PreDraw();
		postEffect->Draw(dxCommon->GetCmdList());
		FrontDraw(dxCommon);
		//ImGuiDraw(dxCommon);
		SelectScene::GetIns()->ImGuiDraw();
		postEffect->ImGuiDraw();
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
void SelectSceneActor::Finalize() {
}
//後ろの描画
void SelectSceneActor::BackDraw(DirectXCommon* dxCommon) {
	
	SelectScene::GetIns()->Draw_Obj(dxCommon);
	IKEObject3d::PreDraw();
	ParticleEmitter::GetInstance()->FlontDrawAll();
	IKEObject3d::PostDraw();
}
//ポストエフェクトがかからない
void SelectSceneActor::FrontDraw(DirectXCommon* dxCommon) {
	IKESprite::PreDraw();

	IKESprite::PostDraw();
	sceneChanger_->Draw();	//完全に前に書くスプライト
	//if (camerawork->GetAppearType() == APPEAR_SEVEN || camerawork->GetAppearType() == APPEAR_EIGHT) {
	IKESprite::PreDraw();
	//blackwindow->Draw();
	SelectScene::GetIns()->Draw_Sprite();
	camerawork->feedDraw();
	IKESprite::PostDraw();
}
//IMGuiの描画
void SelectSceneActor::ImGuiDraw(DirectXCommon* dxCommon) {
	////loadobj->ImGuiDraw();
	SceneSave::GetInstance()->ImGuiDraw();
	//SelectScene::GetIns()->ImGuiDraw();
}

