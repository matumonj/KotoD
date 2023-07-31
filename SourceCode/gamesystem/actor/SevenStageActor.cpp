#include "SevenStageActor.h"
#include"Easing.h"
#include "ParticleEmitter.h"
#include "ImageManager.h"
#include "BackObj.h"
#include "Menu.h"
#include "SelectScene.h"
#include "Helper.h"
#include "HitStop.h"
//������
void SevenStageActor::Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	dxCommon->SetFullScreen(true);
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�I�[�f�B�I
	Audio::GetInstance()->LoopWave(AUDIO_BATTLE2, VolumManager::GetInstance()->GetBGMVolum() + 0.1f);

	//�|�X�g�G�t�F�N�g
	PlayPostEffect = true;
	//�p�[�e�B�N���S�폜
	ParticleEmitter::GetInstance()->AllDelete();

	//�e�N���X
	Player::GetInstance()->InitState({ 0.0f,-2.0f,-5.0f });

	backScreen_ = IKESprite::Create(ImageManager::PLAY, { 0,0 });
	backScreen_->SetAddOffset(-0.0005f);
	backScreen_->SetSize({ 1280.0f,720.0f });
	//�V�[���`�F���W���[
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
//�X�V
void SevenStageActor::Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) {
	//�֐��|�C���^�ŏ�ԊǗ�
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

	//�v���C���[
	if (enemymanager->BossDestroy() && camerawork->GetFeedEnd()) {
		SceneSave::GetInstance()->SetClearFlag(kSevenStage, true);
	}

	lightgroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0 }));
	lightgroup->SetCircleShadowCasterPos(0, XMFLOAT3({ Player::GetInstance()->GetPosition().x, -2.0f, Player::GetInstance()->GetPosition().z }));
	lightgroup->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	lightgroup->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));

	//�{�X
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
//�`��
void SevenStageActor::Draw(DirectXCommon* dxCommon) {
	//�`����@
	//�|�X�g�G�t�F�N�g�������邩
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
//���
void SevenStageActor::Finalize() {
}
//���̕`��
void SevenStageActor::BackDraw(DirectXCommon* dxCommon) {
	IKESprite::PreDraw();
	backScreen_->Draw();
	IKESprite::PostDraw();
	IKEObject3d::PreDraw();
	BackObj::GetInstance()->Draw(dxCommon);
	//�p�[�e�B�N���`��
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
	////�e�N���X�̕`��
	if (!camerawork->GetFeedEnd()) {
		if (camerawork->GetCameraState() != CAMERA_BOSS_STRONG) {
			loadobj->Draw(dxCommon);
		}
	}
	enemymanager->Draw(dxCommon);
	IKEObject3d::PostDraw();
}
//�|�X�g�G�t�F�N�g��������Ȃ�
void SevenStageActor::FrontDraw(DirectXCommon* dxCommon) {
	//�p�[�e�B�N���`��
	if (!camerawork->GetFeedEnd() && m_SceneState == SceneState::MainState) {
		if (camerawork->GetCameraState() != CAMERA_BOSS_STRONG) {
			ParticleEmitter::GetInstance()->FlontDrawAll();
		}
	}

	ParticleEmitter::GetInstance()->DeathDrawAll();
	//���S�ɑO�ɏ����X�v���C�g
	IKESprite::PreDraw();
	if (m_SceneState == SceneState::MainState && !camerawork->GetFeedEnd() && camerawork->GetCameraState() == CAMERA_NORMAL) {
		ui->Draw();
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
//IMGui�̕`��
void SevenStageActor::ImGuiDraw(DirectXCommon* dxCommon) {
	//Player::GetInstance()->ImGuiDraw();
}
//�o��V�[��
void SevenStageActor::IntroUpdate(DebugCamera* camera) {

	//���o�X�L�b�v
	if (Input::GetInstance()->TriggerButton(Input::A)) {
		camerawork->SetCameraSkip(true);
	}

	if (camerawork->GetAppearEndF()) {
		_AppState = APP_END;
		m_SceneState = SceneState::MainState;
		camerawork->SetCameraState(CAMERA_NORMAL);
		enemymanager->SkipInitialize();
	}
	//�e�L�X�g�ǂ�
	TextRead();
	//�e�N���X�X�V
	Player::GetInstance()->LastAppearUpdate(m_AppTimer);
	BackObj::GetInstance()->Update();
	ParticleEmitter::GetInstance()->Update();
	enemymanager->AppearUpdate();
	camerawork->SetLastTimer(m_AppTimer);
	camerawork->Update(camera);
}
//�o�g���V�[��
void SevenStageActor::MainUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
	//�o���V�[���ɓ���
	if (enemymanager->GetEnemyStrong() && !camerawork->GetCameraStrong()) {
		if (!camerawork->GetFeedF()) {
			camerawork->SetFeedF(true);
			camerawork->SetCameraState(CAMERA_BOSS_STRONG);
			camerawork->SetCameraStrong(true);
		}
	}

	if (camerawork->GetCameraState() == CAMERA_BOSS_STRONG) {
		//���ڂ̃t�F�[�h�̌�Ɉʒu������������
		if (camerawork->GetChangeStrong()) {
			Player::GetInstance()->AwakeInit();
			enemymanager->DeleteObj();
			loadobj->AwakeInit();
			text_->Display();
			m_AwakeTimer++;
			AwakeText();
		}
		//���ڂ̃t�F�[�h���I���ƃo�g�����[�h�ɖ߂�
		if (camerawork->GetEndStrong()) {
			Player::GetInstance()->SetPosition({ 0.0f,0.0f,-5.0f });
			camerawork->SetCameraState(CAMERA_NORMAL);
		}
	}
	//�J�������[�N�̃Z�b�g
	if (enemymanager->BossDestroy()) {
		//�t�F�[�h�O
		if (!camerawork->GetFeedEnd()) {
			enemymanager->SetDeadThrow(true);
			enemymanager->DeadUpdate();
			camerawork->SetCameraState(CAMERA_BOSSDEAD_BEFORE);
		}
		//�t�F�[�h��
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

	//���y�̉��ʂ��ς��
	VolumManager::GetInstance()->Update();

	//�e�N���X�X�V
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
//���j�V�[��
void SevenStageActor::FinishUpdate(DebugCamera* camera) {
	Input* input = Input::GetInstance();
}
//�e�L�X�g�֌W
void SevenStageActor::TextRead() {
	text_->Display();
	m_AppTimer++;
	if (m_AppTimer == 240) {
		text_->SelectText(TextManager::LAST_TALK_SECOND);
		WhoRead("sutopon");
	} else if (m_AppTimer == 350) {
		text_->SelectText(TextManager::LAST_TALK_THIRD);
		WhoRead("girl");
	} else if (m_AppTimer == 500) {
		text_->SelectText(TextManager::LAST_TALK_FOURTH);
		WhoRead("sutopon");
	} else if (m_AppTimer == 650) {
		text_->SelectText(TextManager::LAST_TALK_FIVE);
		WhoRead("boss");
	} else if (m_AppTimer == 800) {
		text_->SelectText(TextManager::LAST_TALK_SIX);
		WhoRead("sutopon");
	} else if (m_AppTimer == 950) {
		text_->SelectText(TextManager::LAST_TALK_SEVEN);
		WhoRead("boss");
	} else if (m_AppTimer == 1100) {
		text_->SelectText(TextManager::LAST_TALK_EIGHT);
	} else if (m_AppTimer == 1250) {
		text_->SelectText(TextManager::LAST_TALK_NINE);
	} else if (m_AppTimer == 1400) {
		text_->SelectText(TextManager::LAST_TALK_TEN);
	} else if (m_AppTimer == 1550) {
		text_->SelectText(TextManager::LAST_TALK_ELEVEN);
	} else if (m_AppTimer == 1700) {
		text_->SelectText(TextManager::LAST_TALK_TWELVE);
	} else if (m_AppTimer == 1850) {
		text_->SelectText(TextManager::LAST_TALK_THIRTEEN);
		WhoRead("sutopon");
	} else if (m_AppTimer == 2000) {
		text_->SelectText(TextManager::LAST_TALK_FOURTEEN);
	} else if (m_AppTimer == 2150) {
		text_->SelectText(TextManager::LAST_TALK_FIFETEEN);
		WhoRead("boss");
	} else if (m_AppTimer == 2300) {
		text_->SelectText(TextManager::LAST_TALK_SIXTEEN);
	} else if (m_AppTimer == 2450) {
		text_->SelectText(TextManager::LAST_TALK_SEVENTEEN);
		WhoRead("sutopon");
	} else if (m_AppTimer == 2600) {
		text_->SelectText(TextManager::LAST_TALK_EIGHTTEEN);
	} else if (m_AppTimer == 2750) {
		text_->SelectText(TextManager::LAST_TALK_NINETEEN);
	} else if (m_AppTimer == 2950) {
		camerawork->SetCameraSkip(true);
	}

	messagewindow_->DisplayCharacter(sutopon_color_);
}
//�o�����̃e�L�X�g
void SevenStageActor::AwakeText() {
	if (m_AwakeTimer == 1) {
		WhoRead("none");
		text_->SelectText(TextManager::AWAKE_FIRST);
	} else if (m_AwakeTimer == 200) {
		text_->SelectText(TextManager::AWAKE_SECOND);
	} else if (m_AwakeTimer == 400) {
		text_->SelectText(TextManager::AWAKE_THIRD);
	}
}
//�{�X���j���̃G�t�F�N�g
void SevenStageActor::DeathText() {
	if (m_EndTimer == 1) {
		text_->SelectText(TextManager::DEATH_FIRST);
		WhoRead("boss");
	} else if (m_EndTimer == 200) {
		text_->SelectText(TextManager::DEATH_SECOND);
	} else if (m_EndTimer == 350) {
		text_->SelectText(TextManager::DEATH_THIRD);
	} else if (m_EndTimer == 500) {
		text_->SelectText(TextManager::DEATH_FOURTH);
		WhoRead("sutopon");
	} else if (m_EndTimer == 650) {
		text_->SelectText(TextManager::DEATH_FIVE);
	} else if (m_EndTimer == 800) {
		text_->SelectText(TextManager::DEATH_SIX);//�{�X
		WhoRead("boss");
	} else if (m_EndTimer == 900) {
		text_->SelectText(TextManager::DEATH_SEVEN);//�{�X
	} else if (m_EndTimer == 1000) {
		text_->SelectText(TextManager::DEATH_EIGHT);//�R�g�R
		WhoRead("girl");
	} else if (m_EndTimer == 1100) {
		text_->SelectText(TextManager::DEATH_NINE);//�{�X
		WhoRead("boss");
	} else if (m_EndTimer == 1200) {
		text_->SelectText(TextManager::DEATH_TEN);//�R�g�R
		WhoRead("girl");
	} else if (m_EndTimer == 1300) {
		text_->SelectText(TextManager::DEATH_ELEVEN);//�R�g�R
	} else if (m_EndTimer == 1400) {
		text_->SelectText(TextManager::DEATH_TWELVE);//�{�X
		WhoRead("boss");
	} else if (m_EndTimer == 1500) {
		text_->SelectText(TextManager::DEATH_THIRTEEN);//�R�g�R
		WhoRead("girl");
	} else if (m_EndTimer == 1600) {
		text_->SelectText(TextManager::DEATH_FOURTEEN);//����
		WhoRead("sutopon");
	} else if (m_EndTimer == 1700) {
		text_->SelectText(TextManager::DEATH_FIFTEEN);//�{�X
		WhoRead("boss");
	}
}
//�ǂ������Ă�ł��邩
void SevenStageActor::WhoRead(const string& name) {
	if (name == "sutopon") {
		sutopon_color_ = { 1.0f,1.0f,1.0f,1.0f };
		girl_color_ = { 0.5f,0.5f,0.5f,0.5f };
	} else if (name == "girl") {
		girl_color_ = { 1.0f,1.0f,1.0f,1.0f };
		sutopon_color_ = { 0.5f,0.5f,0.5f,0.5f };
	} else if (name == "boss") {
		sutopon_color_ = { 0.5f,0.5f,0.5f,0.5f };
		girl_color_ = { 0.5f,0.5f,0.5f,0.5f };
	} else if (name == "none") {
		sutopon_color_ = { 0.0f,0.0f,0.0f,0.0f };
		girl_color_ = { 0.0f,0.0f,0.0f,0.0f };
	}
}