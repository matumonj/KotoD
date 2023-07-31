#include "SelectStageScene.h"
#include "SelectSceneActor.h"
//������
void SelectStageScene::Initialize(DirectXCommon* dxCommon)
{
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�v���C�V�[���̃A�N�^�[
	BaseActor* actor_;
	actor_ = new SelectSceneActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void SelectStageScene::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void SelectStageScene::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//���
void SelectStageScene::Finalize()
{
	actor->Finalize();
}