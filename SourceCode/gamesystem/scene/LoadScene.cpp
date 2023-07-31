#include "LoadScene.h"

//������
void LoadScene::Initialize(DirectXCommon* dxCommon) {
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//���[�h�̃A�N�^�[
	BaseActor* actor_;
	actor_ = new LoadSceneActor();
	actor_->Initialize(dxCommon,camera,lightGroup);
	actor.reset(actor_);
}
//�X�V
void LoadScene::Update(DirectXCommon* dxCommon) {
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void LoadScene::Draw(DirectXCommon* dxCommon) {
	//�|�X�g�G�t�F�N�g�������邩
	actor->Draw(dxCommon);
}
//���
void LoadScene::Finalize() {
	actor->Finalize();
}