#include "GameoverScene.h"
#include "GameOverSceneActor.h"
//������
void GameoverScene::Initialize(DirectXCommon* dxCommon) {
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�N���A�̃A�N�^�[
	BaseActor* actor_;
	actor_ = new GameOverSceneActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void GameoverScene::Update(DirectXCommon* dxCommon) {
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void GameoverScene::Draw(DirectXCommon* dxCommon) {
	//�|�X�g�G�t�F�N�g�������邩
	actor->Draw(dxCommon);
}
//���
void GameoverScene::Finalize() {
	actor->Finalize();
}