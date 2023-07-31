#include "ClearScene.h"
//������
void ClearScene::Initialize(DirectXCommon* dxCommon) {
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�N���A�̃A�N�^�[
	BaseActor* actor_;
	actor_ = new ClearSceneActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void ClearScene::Update(DirectXCommon* dxCommon) {
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void ClearScene::Draw(DirectXCommon* dxCommon) {
	//�|�X�g�G�t�F�N�g�������邩
	actor->Draw(dxCommon);
}
//���
void ClearScene::Finalize() {
	actor->Finalize();
}