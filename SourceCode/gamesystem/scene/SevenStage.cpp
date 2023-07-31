#include "SevenStage.h"
//������
void SevenStage::Initialize(DirectXCommon* dxCommon)
{
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�v���C�V�[���̃A�N�^�[
	BaseActor* actor_;
	actor_ = new SevenStageActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void SevenStage::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void SevenStage::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//���
void SevenStage::Finalize()
{
	actor->Finalize();
}