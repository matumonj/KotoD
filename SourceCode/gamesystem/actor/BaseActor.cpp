#include "BaseActor.h"
#include "VariableCommon.h"
#include "Player.h"
#include"Menu.h"
bool BaseActor::s_GameLoop = false;
bool BaseActor::s_Skip = false;
void (BaseActor::* BaseActor::stateTable[])(DebugCamera* camera) = {
	&BaseActor::IntroUpdate,//�v�f0
	&BaseActor::MainUpdate, //�v�f1
	&BaseActor::FinishUpdate,
};


//���ʏ�����
void BaseActor::BaseInitialize(DirectXCommon* dxCommon, XMFLOAT3 eye, XMFLOAT3 target) {
	//�|�X�g�G�t�F�N�g�̏�����
	//(����)
	postEffect = make_unique<PostEffect>();
	postEffect->Initialize();
	//�|�X�g�G�t�F�N�g�̃t�@�C���w��
	postEffect->CreateGraphicsPipeline(L"Resources/Shaders/PostEffectTestVS.hlsl", L"Resources/Shaders/PostEffectTestPS.hlsl");

	camerawork = make_unique<CameraWork>(eye, target);
	Player::GetInstance()->BulletDelete();
}

//�v���C���[�����񂾂�
bool BaseActor::PlayerDestroy() {
	if (Player::GetInstance()->GetHP() <= 0.0f) {
		return true;
	}
	else {
		return false;
	}

	return false;
}
