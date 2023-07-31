#include "FirstStage.h"
//������
void FirstStage::Initialize(DirectXCommon* dxCommon)
{
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�v���C�V�[���̃A�N�^�[
	BaseActor* actor_;
	actor_ = new FirstStageActor();
	actor_->Initialize(dxCommon,camera,lightGroup);
	actor.reset(actor_);
}
//�X�V
void FirstStage::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void FirstStage::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//���
void FirstStage::Finalize()
{
	actor->Finalize();
}