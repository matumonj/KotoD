#include "TutorialScene.h"
//������
void TutorialScene::Initialize(DirectXCommon* dxCommon) {
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�^�C�g���̃A�N�^�[
	BaseActor* actor_;
	actor_ = new TutorialSceneActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void TutorialScene::Update(DirectXCommon* dxCommon) {
	lightGroup->Update();
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void TutorialScene::Draw(DirectXCommon* dxCommon) {
	actor->Draw(dxCommon);
}
//���
void TutorialScene::Finalize() {
	actor->Finalize();
}