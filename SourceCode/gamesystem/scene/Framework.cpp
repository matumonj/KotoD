#include "Framework.h"
#include "IKEFbxLoader.h"
#include "ParticleEmitter.h"
#include "AudioManager.h"
#include "Font.h"
#include "SceneSave.h"
#include "TItleObj.h"
#include"Menu.h"
void Framework::Run()
{
	Initialize(dxcommon);
	while (true) {
		if (FPSManager::GetInstance()->Run()) {
			//���t���[���X�V
			Update(dxcommon);

			//�I�����N�G�X�g�������甲����
			if (endRequst) {
				break;
			}

			//�`��
			Draw(dxcommon);
		}
	}
	//�������
	Finalize();

}
void Framework::Initialize(DirectXCommon* dxCommon)
{
	winApp = new WinApp();
	winApp->Initialize();
	dxcommon = new DirectXCommon();
	dxcommon->Initialize(winApp);
	// ���͂̏�����
	input = Input::GetInstance();
	input->Initialize(winApp);

	audio = Audio::GetInstance();
	audio->Initialize();
	//FPS�Œ�
	fpsManager = FPSManager::GetInstance();
	fpsManager->Init();
	// nullptr�`�F�b�N
	assert(dxcommon);
	assert(input);
	assert(audio);
	const int debugTextTexNumber = 0;
	IKESprite::StaticInitialize(dxcommon->GetDev(), dxcommon->GetCmdList(), WinApp::window_width, WinApp::window_height);
	// �f�o�b�O�e�L�X�g�p�e�N�X�`���ǂݍ���
	if (!IKESprite::LoadTexture(debugTextTexNumber, L"Resources/2d/debugfont.png")) {
		assert(0);
		return;
	}
	// ���C�g�ÓI������
	LightGroup::StaticInitialize(dxcommon->GetDev());
	// �f�o�b�O�e�L�X�g������
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	IKEObject3d::StaticInitialize(dxcommon->GetDev(), dxcommon->GetCmdList(), WinApp::window_width, WinApp::window_height);
	IKETexture::StaticInitialize(dxcommon->GetDev(), dxcommon->GetCmdList(), WinApp::window_width, WinApp::window_height);
	
	// FBX�֘A�ÓI������
	IKEFbxLoader::GetInstance()->Initialize(dxcommon->GetDev());
	//���\�[�X�}�l�[�W���[�̓ǂݍ���
	ModelManager::GetInstance()->StartInitialize();
	ImageManager::GetInstance()->StartLoad2D();
	AudioManager::GetInstance()->StartLoadAudio();
	//�V���h�E�}�b�v�̋��ʏ�����
	ShadowMap::ShadowMapCommon(dxcommon->GetDev(),dxcommon->GetCmdList());
	shadowmap = ShadowMap::Create();
	// �p�[�e�B�N���}�l�[�W��������
	ParticleManager::CreateCommon(dxcommon->GetDev(), dxcommon->GetCmdList());
	//�p�[�e�B�N���G�~�b�^�[������
	ParticleEmitter::GetInstance()->Initialize();
	Font::Initialize(dxcommon);
	
	SceneSave::GetInstance()->AllReset();

	//
	TitleObj::GetInstance()->LoadResource();
}

void Framework::Finalize()
{
	ModelManager::GetInstance()->Finalize();
	IKESprite::Finalize();
	IKETexture::Finalize();
	LightGroup::Finalize();
	SceneManager::GetInstance()->Finalize();
	input->GetInstance()->Finalize();
	delete sceneFactory_;
	dxcommon->Finalize();
	//dxcommon->Reset();
	delete dxcommon;
	winApp->Finalize();
	delete winApp;
}

void Framework::Update(DirectXCommon* dxCommon)
{
	//�E�B���h�E���b�Z�[�W����
	if (winApp->ProcessMessage()) {
		//�Q�[�����[�v�I��
		endRequst = true;
		return;
	}

	input->Update();

	//�V�[���X�V����
	SceneManager::GetInstance()->Update(dxCommon);
}

void Framework::Draw(DirectXCommon* dxCommon)
{
	//dxCommon->PreDraw();

	//�V�[���`��
	SceneManager::GetInstance()->Draw(dxCommon);

	//dxCommon->PostDraw();
	//�Ńo�b�N�e�L�X�g�̕`��
	//debugText->DrawAll();
}