#include "TitleScene.h"
//������
void TitleScene::Initialize(DirectXCommon* dxCommon) {
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�^�C�g���̃A�N�^�[
	BaseActor* actor_;
	actor_ = new TitleSceneActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void TitleScene::Update(DirectXCommon* dxCommon) {
	lightGroup->Update();
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void TitleScene::Draw(DirectXCommon* dxCommon) {
	actor->Draw(dxCommon);
}
//���
void TitleScene::Finalize() {
	actor->Finalize();
}