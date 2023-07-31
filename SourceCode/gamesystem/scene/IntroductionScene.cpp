#include "IntroductionScene.h"
//������
void IntroductionScene::Initialize(DirectXCommon* dxCommon) {
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�����̃A�N�^�[
	BaseActor* actor_;
	actor_ = new IntroductionSceneActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void IntroductionScene::Update(DirectXCommon* dxCommon) {
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void IntroductionScene::Draw(DirectXCommon* dxCommon) {
	actor->Draw(dxCommon);
}
//���
void IntroductionScene::Finalize() {
	actor->Finalize();
}