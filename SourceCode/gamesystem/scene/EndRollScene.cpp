#include "EndRollScene.h"

//������
void EndRollScene::Initialize(DirectXCommon* dxCommon) {
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//���[�h�̃A�N�^�[
	BaseActor* actor_;
	actor_ = new EndRollActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void EndRollScene::Update(DirectXCommon* dxCommon) {
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void EndRollScene::Draw(DirectXCommon* dxCommon) {
	//�|�X�g�G�t�F�N�g�������邩
	actor->Draw(dxCommon);
}
//���
void EndRollScene::Finalize() {
	actor->Finalize();
}