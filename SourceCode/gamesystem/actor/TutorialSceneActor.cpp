#include "TutorialSceneActor.h"
#include "ImageManager.h"
#include "HungerGauge.h"
#include "Helper.h"
const XMVECTOR kWhite{ 1.f,1.f,1.f,1.f };
const XMVECTOR kSkyBlue{ 0.f,1.f,1.f,1.f };
const XMVECTOR kPink{ 0.9f,0.6f,0.8f,1.f };
const XMFLOAT2 kFirstRowPos{ 5.f,0.f };
const XMFLOAT2 kSecondRowPos{ 5.f,-40.f };
const XMFLOAT2 kThirdRowPos{ 5.f, -80.f };
const XMFLOAT4 kHalfClear{ 0.5f,0.5f,0.5f,0.5f };
const XMFLOAT4 kOriginalGirlColor= { 1.2f,1.2f,1.2f,1.f };
const XMFLOAT4 kOriginalSutoponColor= { 1.f,1.f,1.f,1.f };

bool TutorialSceneActor::isDebug = false;
TextManager* instance = TextManager::GetInstance();

//状態遷移
/*stateの並び順に合わせる*/
void (TutorialSceneActor::* TutorialSceneActor::stateTable[])() = {
	&TutorialSceneActor::IntroState,//
	&TutorialSceneActor::MoveState,//
	&TutorialSceneActor::TextTalkState,//
	&TutorialSceneActor::SpawnEnemyState,//
	&TutorialSceneActor::TextCatchFollowState,//
	&TutorialSceneActor::CatchFollowState,//
	&TutorialSceneActor::TextShotState,//
	&TutorialSceneActor::ShotState,//
	&TutorialSceneActor::TextCatchSeachState,//
	&TutorialSceneActor::CatchSeachState,//
	&TutorialSceneActor::TextClearState,//
	&TutorialSceneActor::SpawnAllEnemyState,//
	&TutorialSceneActor::TextLastState,//
	&TutorialSceneActor::MainTutorialState,//
	&TutorialSceneActor::CompleteState,//
	&TutorialSceneActor::Skip,
};

void TutorialSceneActor::IntroState() {


	if (DebugButton() ||
		input->TriggerButton(Input::B)) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		text_->NoneText();
		nowstate_ = state::MOVE;
	}
}
void TutorialSceneActor::MoveState() {
	text_->SetConversation(TextManager::NONE);

	//メガホンobj更新
	sutepon->Update();

	shakeCount++;
	if (shakeCount > 30) {
		if (!isShake) {
			isShake = true;
		}
		else {
			isShake = false;
		}
		shakeCount = 0;
	}
	if (isShake) {
		shakeTimer += 0.7f;
		sutepon->SetPosition(RandomShake({ 0,0,15 }, shakeTimer));
	}

	XMFLOAT3 pos = Player::GetInstance()->GetPosition();
	XMFLOAT3 Spos = sutepon->GetPosition();
	if (Collision::CircleCollision(Spos.x, Spos.z, 5.f, pos.x, pos.z, 1.f)) {
		nowstate_ = state::TEXT_TALK;
	}
	if (input->TriggerKey(DIK_O)) {
		nowstate_ = state::TEXT_TALK;
		text_->SetConversation(TextManager::NONE);
	}
}
void TutorialSceneActor::TextTalkState() {

	messagewindow_->DisplayCharacter(sutopon_color_);
	if (input->TriggerButton(Input::B) && conversation < 5) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		conversation += 1;
	}

	if (conversation == 0) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK2,kPink);
	}
	else if (conversation == 1) {
		girl_color_ = kHalfClear;
		text_->SetConversation(TextManager::TYUTORIAL_TALK3, kSkyBlue);
	}
	else if (conversation == 2) {
		girl_color_ = kOriginalGirlColor;
		sutopon_color_ = kHalfClear;
		text_->SetConversation(TextManager::TYUTORIAL_TALK4,kPink);
	}
	else if (conversation == 3) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK5, kSkyBlue);
	}
	else if (conversation == 4) {
		girl_color_ = kOriginalGirlColor;
		sutopon_color_ = kHalfClear;
		text_->SetConversation(TextManager::TYUTORIAL_TALK6,kPink);
	}


	if (DebugButton() ||
		conversation == 5) {
		text_->SetConversation(TextManager::NONE);
		sutepon->SetPosition({ 0,0,15.0f });
		conversation = 0;
		nowstate_ = state::SPAWNENEMY;
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
	}
}
void TutorialSceneActor::SpawnEnemyState() {
	loadobj->TutorialUpdate();
	enemymanager->TutorialUpdate(0);
	XMFLOAT3 plaPos = Player::GetInstance()->GetPosition();
	XMFLOAT3 enePos = firstEnemy->GetPosition();
	if (DebugButton() ||
		Clear(Collision::CircleCollision(plaPos.x, plaPos.z, 20.0f, enePos.x, enePos.z, 1.0f), 0)) {
		waitTimer = 0;
		nowstate_ = state::TEXT_CATCHFOLLOW;
	}

}
void TutorialSceneActor::TextCatchFollowState() {

	if (input->TriggerButton(Input::B) && conversation < 3) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		conversation += 1;
	}
	if (conversation == 0) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK7, kSkyBlue);
		loadobj->TutorialUpdate();
	}
	else if (conversation == 1) {
		girl_color_ = kOriginalGirlColor;
		sutopon_color_ = kHalfClear;
		text_->SetConversation(TextManager::TYUTORIAL_TALK8,kPink);
	}
	else if (conversation == 2) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK9, kSkyBlue);
	}

	if (DebugButton() ||
		conversation == 3) {
		nowstate_ = state::CATCHFOLLOW;
		text_->SetConversation(TextManager::NONE);
		conversation = 0;
		number = 1;
		Player::GetInstance()->Display();
	}

}
void TutorialSceneActor::CatchFollowState() {
	text_->SetConversation(TextManager::NONE);
	loadobj->TutorialUpdate();
	firstEnemy->SetIsStop(true);
	enemymanager->TutorialUpdate(0);
	Player::GetInstance()->SetCanShot(true);
	if (DebugButton() ||
		Clear(HungerGauge::GetInstance()->GetCatchCount() >= 1, 50)) {
		waitTimer = 0;
		nowstate_ = state::TEXT_SHOT;
		text_->SetConversation(TextManager::NONE);
	}

}
void TutorialSceneActor::TextShotState() {
	Player::GetInstance()->SetCanShot(false);
	if (input->TriggerButton(Input::B) && conversation < 1) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		conversation += 1;
	}

	if (conversation == 0) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK10, kSkyBlue);
		loadobj->TutorialUpdate();
	}
	


	if (DebugButton() ||
		conversation == 1) {
		nowstate_ = state::SHOT;
		text_->SetConversation(TextManager::NONE);
		conversation = 0;
		number = 2;
	}
}
void TutorialSceneActor::ShotState() {
	text_->SetConversation(TextManager::NONE);
	loadobj->TutorialUpdate();
	enemymanager->TutorialUpdate(0);
	Player::GetInstance()->SetCanShot(true);
	if (DebugButton() ||
		Clear(!firstEnemy->GetisAlive(), 45)) {
		waitTimer = 0;
		nowstate_ = state::TEXT_CATCHSEACH;
		text_->SetConversation(TextManager::NONE);
	}
}
void TutorialSceneActor::TextCatchSeachState() {
	Player::GetInstance()->SetCanShot(false);
	if (input->TriggerButton(Input::B) && conversation < 6) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		conversation += 1;
	}

	if (conversation == 0) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK11, kSkyBlue);
		loadobj->TutorialUpdate();
	}
	else if (conversation == 1) {
		girl_color_ = kOriginalGirlColor;
		sutopon_color_ = kHalfClear;
		text_->SetConversation(TextManager::TYUTORIAL_TALK12,kPink);
	}
	else if (conversation == 2) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK13, kSkyBlue);
	}
	else if (conversation == 3) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK14,kSkyBlue);
	}
	else if (conversation == 4) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK15,kSkyBlue);
	}
	else if (conversation == 5) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK16,kSkyBlue);
	}


	if (DebugButton() ||
		conversation == 6) {
		HungerGauge::GetInstance()->ResetFirstCarry();
		nowstate_ = state::CATCHSEACH;
		text_->SetConversation(TextManager::NONE);
		conversation = 0;
		number = 3;
	}
}
void TutorialSceneActor::CatchSeachState() {
	Player::GetInstance()->SetCanShot(true);
	text_->SetConversation(TextManager::NONE);
	loadobj->TutorialUpdate();
	enemymanager->TutorialUpdate(0);


	if ((DebugButton() ||
		Clear(HungerGauge::GetInstance()->GetFirstCarry(), 30))
		&& !Player::GetInstance()->GetIsShotNow()) {
		waitTimer = 0;
		nowstate_ = state::TEXT_CLEAR;
		text_->SetConversation(TextManager::NONE);
	}
}
void TutorialSceneActor::TextClearState() {

	if (input->TriggerButton(Input::B) && conversation < 2) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		conversation += 1;
	}

	if (conversation == 0) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK17, kSkyBlue);
		loadobj->TutorialUpdate();
	}
	else if (conversation == 1) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK18,kSkyBlue);
	}	

	if ((DebugButton() ||
		conversation==2)) {
		Player::GetInstance()->SetCanShot(false);
		nowstate_ = state::SPAWNALLENEMY;
		s_eyepos = camerawork->GetEye();
		s_targetpos = camerawork->GetTarget();
		text_->SetConversation(TextManager::NONE);
		conversation = 0;
		number = 4;
	}
}
void TutorialSceneActor::SpawnAllEnemyState() {
	loadobj->TutorialUpdate();
	Player::GetInstance()->MoveStop(true);
	Player::GetInstance()->SetCanShot(false);
	Player::GetInstance()->Update();
	HungerGauge::GetInstance()->SetIsStop(true);
	if (MovingCamera(s_eyepos, e_eyepos, s_targetpos, e_targetpos)) {
		enemymanager->TutorialUpdate(1);
	}
	if (Clear(cameraframe >= 1.0f, 50)) {
		s_eyepos = { Player::GetInstance()->GetPosition().x,
		Player::GetInstance()->GetPosition().y + 50.0f,
		Player::GetInstance()->GetPosition().z - 20.0f };
		s_targetpos.x = Player::GetInstance()->GetPosition().x;
		s_targetpos.z = Player::GetInstance()->GetPosition().z;
		nowstate_ = state::TEXT_LAST;
		cameraframe = 0.0f;
	}
}
void TutorialSceneActor::TextLastState() {
	loadobj->TutorialUpdate();
	enemymanager->TutorialUpdate(2);
	Player::GetInstance()->Update();
	
	if (conversation == 0) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK19, kSkyBlue);
		loadobj->TutorialUpdate();
	}
	else if (conversation == 1) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK20, kSkyBlue);
	}

	if (MovingCamera(e_eyepos, s_eyepos, e_targetpos, s_targetpos)) {
		if (input->TriggerButton(Input::B) && conversation < 2) {
			Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
			conversation += 1;
		}
		if ((DebugButton() ||
			conversation==2)
			) {
			HungerGauge::GetInstance()->SetIsStop(false);
			text_->SetConversation(TextManager::NONE);
			conversation = 0;
			nowstate_ = state::MAINTUTORIAL;
			Player::GetInstance()->MoveStop(false);
		}
	}

}
void TutorialSceneActor::MainTutorialState() {
	loadobj->TutorialUpdate();
	enemymanager->TutorialUpdate(1);
	Player::GetInstance()->SetCanShot(true);
	if (DebugButton() ||
		Clear(enemymanager->AllDeadEnemy(), 60)) {
		nowstate_ = state::COMPLETE;
	}
}
void TutorialSceneActor::CompleteState() {
	if (input->TriggerButton(Input::B)&&conversation < 12) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Button_Text.wav", VolumManager::GetInstance()->GetSEVolum());
		conversation += 1;
	}

	if (conversation == 0) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK21, kSkyBlue);
		loadobj->TutorialUpdate();
	}
	else if (conversation == 1) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK22,kSkyBlue);
	}
	else if (conversation == 2) {
		girl_color_ = kOriginalGirlColor;
		sutopon_color_ = kHalfClear;
		text_->SetConversation(TextManager::TYUTORIAL_TALK23,kPink);
	}
	else if (conversation == 3) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK24, kSkyBlue);
	}
	else if (conversation == 4) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK25, kSkyBlue);
	}
	else if (conversation == 5) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK26,kSkyBlue);
	}
	else if (conversation == 6) {
		girl_color_ = kOriginalGirlColor;
		sutopon_color_ = kHalfClear;
		text_->SetConversation(TextManager::TYUTORIAL_TALK27,kPink);
	}
	else if (conversation == 7) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK28,kSkyBlue);
	}
	else if (conversation == 8) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK29, kSkyBlue);
	}
	else if (conversation == 9) {
		text_->SetConversation(TextManager::TYUTORIAL_TALK30,kSkyBlue);
	}
	else if (conversation == 10) {
		girl_color_ = kOriginalGirlColor;
		sutopon_color_ = kHalfClear;
		text_->SetConversation(TextManager::TYUTORIAL_TALK31,kPink);
	}
	else if (conversation == 11) {
		girl_color_ = kHalfClear;
		sutopon_color_ = kOriginalSutoponColor;
		text_->SetConversation(TextManager::TYUTORIAL_TALK32,kSkyBlue);
	}
	else if (conversation == 12) {
		girl_color_ = kOriginalGirlColor;
		sutopon_color_ = kHalfClear;
		text_->SetConversation(TextManager::TYUTORIAL_TALK33,kPink);
	}

	if (DebugButton() ||
		conversation == 12) {
		sceneChanger_->ChangeStart();
		SceneSave::GetInstance()->SetClearFlag(kTutorialStage,true);
		Audio::GetInstance()->StopWave(AUDIO_LOAD);
	}
	sceneChanger_->ChangeScene("SELECT", SceneChanger::NonReverse);

}

void TutorialSceneActor::Skip()
{
	
}

bool TutorialSceneActor::DebugButton() {
	if (!isDebug) { return false; }
	if (input->TriggerKey(DIK_SPACE)) {
		return true;
	}
	else {
		return false;
	}
}
XMFLOAT3 TutorialSceneActor::RandomShake(XMFLOAT3 pos, float shakeTimer) {
	//乱数指定
	//mt19937 mt{ std::random_device{}() };
	//std::uniform_real_distribution<float> dist(-0.5f, 0.5f);

	float angle = sinf(shakeTimer) * 0.3f;
	return XMFLOAT3(pos.x + angle, pos.y, pos.z);
}
bool TutorialSceneActor::MovingCamera(const XMFLOAT3& s_eye, const XMFLOAT3& e_eye, const XMFLOAT3& s_target, const XMFLOAT3& e_target) {
	XMFLOAT3 eye = camerawork->GetEye();
	XMFLOAT3 target = camerawork->GetTarget();
	cameraframe += 1.0f / kCameraFrameMax;
	cameraframe = min(1.0f, cameraframe);
	eye = {
	Ease(In,Linear,cameraframe, s_eye.x,e_eye.x),
	eye.y,
	Ease(In,Linear,cameraframe,  s_eye.z,e_eye.z),
	};
	target = {
	Ease(In,Linear,cameraframe, s_target.x,e_target.x),
	target.y,
	Ease(In,Linear,cameraframe, s_target.z,e_target.z),
	};

	camerawork->SetEye(eye);
	camerawork->SetTarget(target);
	Helper::GetInstance()->Clamp(cameraframe, 0.0f, 1.0f);

	if (cameraframe == 1.0f) {
		return true;
	}
	else {
		return false;
	}
}
void TutorialSceneActor::CameraUpdate(DebugCamera* camera) {
	if (!(nowstate_ == state::SPAWNALLENEMY || nowstate_ == state::TEXT_LAST)) {
		camerawork->SetCameraState(CAMERA_NORMAL);
	}
	else {
		camerawork->SetCameraState(CAMERA_LOAD);
	}

	camerawork->Update(camera);
}
bool TutorialSceneActor::Clear(bool mission, int waitTimerMax) {
	if (!mission) { return false; }
	waitTimer++;
	if (waitTimer >= waitTimerMax) {
		return true;
	}
	else {
		return false;
	}
}

//初期化
void TutorialSceneActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	dxCommon->SetFullScreen(true);
	//共通の初期化
	BaseInitialize(dxCommon);
	Audio::GetInstance()->StopWave(AUDIO_TITLE);
	//オーディオ
	Audio::GetInstance()->LoopWave(AUDIO_LOAD, VolumManager::GetInstance()->GetBGMVolum() + 0.5f);
	//ポストエフェクト
	PlayPostEffect = false;
	//パーティクル全削除
	ParticleEmitter::GetInstance()->AllDelete();
	//各クラス
	Player::GetInstance()->InitState({ 0.0f,-2.0f,0.0f });
	Player::GetInstance()->SetCanShot(false);
	//カメラ更新
	camerawork->Update(camera);
	//UIの初期化
	ui = std::make_unique<UI>();
	ui->Initialize();
	//ストポンの初期化
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Sutopon);
	sutepon = make_unique<IKEObject3d>();
	sutepon->Initialize();
	sutepon->SetModel(m_Model);
	sutepon->SetPosition({ 0,0,15.0f });
	sutepon->SetRotation({ 60.0f,0,0 });
	//エネミーの生成
	enemymanager = std::make_unique<EnemyManager>("TUTORIAL");
	//最初のエネミーの参照
	firstEnemy = enemymanager->GetEnemy(0);
	//メッセージウィンドウ生成
	messagewindow_ = make_unique<MessageWindow>();
	messagewindow_->Initialize();
	messagewindow_->Display();
	//背景objの生成
	BackObj::GetInstance()->Initialize();


	text_ = make_unique<TextManager>();
	text_->Initialize(dxCommon);
	text_->SetConversation(TextManager::TYUTORIAL_TALK1,kPink);
	BackObj::GetInstance()->Initialize();
	loadobj = std::make_unique<LoadStageObj>();
	loadobj->AllLoad("TUTORIAL");
	loadobj->LightSet(lightgroup);
	LoadStageObj::SetEnemyManager(enemymanager.get());
	//シーンチェンジャー
	sceneChanger_ = make_unique<SceneChanger>();
	sceneChanger_->Initialize();
	backScreen_ = IKESprite::Create(ImageManager::PLAY, { 0,0 });
	backScreen_->SetAddOffset(-0.0005f);
	backScreen_->SetSize({ 1280.0f,720.0f });

	lightgroup->SetCircleShadowActive(0, false);
	lightgroup->SetCircleShadowActive(1, false);
}
//更新
void TutorialSceneActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//スキップ
	if (input->TriggerButton(Input::A)) {
		skip = true;
	}

	if (skip == true) {
		sceneChanger_->ChangeStart();
		SceneSave::GetInstance()->SetClearFlag(kTutorialStage, true);
		Player::GetInstance()->SetCanShot(true);
		Player::GetInstance()->MoveStop(false);
		Audio::GetInstance()->StopWave(AUDIO_LOAD);
		sceneChanger_->ChangeScene("SELECT", SceneChanger::NonReverse);
	}

	//プレイヤー
	lightgroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(0, XMFLOAT3({ Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().y, Player::GetInstance()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));

	//スとぽん
	lightgroup->SetCircleShadowActive(1, true);
	lightgroup->SetCircleShadowDir(1, XMVECTOR({ BosscircleShadowDir[0], BosscircleShadowDir[1], BosscircleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(1, XMFLOAT3({ sutepon->GetPosition().x, 	sutepon->GetPosition().y, 	sutepon->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(1, XMFLOAT3(BosscircleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(1, XMFLOAT2(BosscircleShadowFactorAngle));
	if (nowstate_ == state::MOVE) {
		lightgroup->SetCircleShadowActive(0, true);
		lightgroup->SetCircleShadowActive(1, true);
	}
	else {
		lightgroup->SetCircleShadowActive(1, false);
	}
	XMFLOAT2 pos[3] = { kFirstRowPos,kSecondRowPos,kThirdRowPos };
	XMFLOAT3 color[3] = { {1,1,1},{1,1,1},{1,1,1} };

	//音楽の音量が変わる
	VolumManager::GetInstance()->Update();
	sceneChanger_->Update();
	//状態移行(stateに合わせる)
	(this->*stateTable[static_cast<size_t>(nowstate_)])();
	//各クラス更新
	if (static_cast<int>(nowstate_) % 2 == 1) {
		ui->SetNowNumber(number);
		ui->SetumeiUpdate();
		ui->Update();
		Player::GetInstance()->Update();
	}
	if (static_cast<int>(nowstate_) == 0) {
		Player::GetInstance()->TyutorialUpdate();
	}
	ParticleEmitter::GetInstance()->Update();
	BackObj::GetInstance()->Update();
	CameraUpdate(camera);
	lightgroup->Update();
	messagewindow_->Update(girl_color_, sutopon_color_);
}
//描画
void TutorialSceneActor::Draw(DirectXCommon* dxCommon) {
	//描画方法
	//ポストエフェクトをかけるか
	if (PlayPostEffect) {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		BackDraw(dxCommon);
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		postEffect->Draw(dxCommon->GetCmdList());
		FrontDraw(dxCommon);
		ImGuiDraw(dxCommon);
		postEffect->ImGuiDraw();
		dxCommon->PostDraw();
	}
	else {
		postEffect->PreDrawScene(dxCommon->GetCmdList());
		postEffect->Draw(dxCommon->GetCmdList());
		postEffect->PostDrawScene(dxCommon->GetCmdList());
		dxCommon->PreDraw();
		BackDraw(dxCommon);
		FrontDraw(dxCommon);
		ImGuiDraw(dxCommon);
		dxCommon->PostDraw();
	}
}
//解放
void TutorialSceneActor::Finalize() {
}
//後ろの描画
void TutorialSceneActor::BackDraw(DirectXCommon* dxCommon) {
	IKEObject3d::PreDraw();
	BackObj::GetInstance()->Draw(dxCommon);
	if (nowstate_ != state::INTORO) {
		ParticleEmitter::GetInstance()->BackDrawAll();
	}
	////各クラスの描画
	Player::GetInstance()->Draw(dxCommon);
	if (nowstate_ <= state::TEXT_TALK) {
		sutepon->Draw();
	}
	else {
		loadobj->Draw(dxCommon);
	}

	enemymanager->TutorialDraw(dxCommon);
	IKEObject3d::PostDraw();
}
//ポストエフェクトがかからない
void TutorialSceneActor::FrontDraw(DirectXCommon* dxCommon) {
	//パーティクル描画
	ParticleEmitter::GetInstance()->FlontDrawAll();
	//完全に前に書くスプライト
	if (static_cast<int>(nowstate_) % 2 == 0) {
		IKESprite::PreDraw();
		messagewindow_->Draw();
		IKESprite::PostDraw();
	}
	else {
		if (nowstate_ != state::INTORO && nowstate_ != state::MOVE) {
			ui->Draw();
		}
	}
	if (messagewindow_->DisplayCheck()) {
		text_->TestDraw(dxCommon);
	}
	sceneChanger_->Draw();
}
//IMGuiの描画
void TutorialSceneActor::ImGuiDraw(DirectXCommon* dxCommon) {
	//Player::GetInstance()->ImGuiDraw();
	//loadobj->ImGuiDraw();
	//enemymanager->ImGuiDraw();
	//camerawork->ImGuiDraw();
}
