#include "BaseScene.h"
#include "ImageManager.h"
#include "VariableCommon.h"
#include "ParticleEmitter.h"
#include "Helper.h"

//���ʂ̏�����
void BaseScene::BaseInitialize(DirectXCommon* dxCommon) {
	// �f�o�C�X���Z�b�g
	IKEFBXObject3d::SetDevice(dxCommon->GetDev());
	// �O���t�B�b�N�X�p�C�v���C������
	IKEFBXObject3d::CreateGraphicsPipeline();
	// �J��������
	camera = new DebugCamera(WinApp::window_width, WinApp::window_height);
	IKETexture::SetCamera(camera);
	// 3D�I�u�W�F�N�g�ɃJ�������Z�b�g
	IKEObject3d::SetCamera(camera);
	IKEFBXObject3d::SetCamera(camera);
	ParticleManager::SetCamera(camera);
	// ���C�g����
	lightGroup = LightGroup::Create();
	// 3D�I�u�G�N�g�Ƀ��C�g���Z�b�g
	IKEObject3d::SetLightGroup(lightGroup);

	ParticleEmitter::GetInstance()->AllDelete();
	//�ۉe�̂��߂̂��
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightActive(1, true);
	lightGroup->SetDirLightActive(2, true);
}
