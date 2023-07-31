#include "FiveStage.h"
//������
void FiveStage::Initialize(DirectXCommon* dxCommon)
{
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�v���C�V�[���̃A�N�^�[
	BaseActor* actor_;
	actor_ = new FiveStageActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void FiveStage::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void FiveStage::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//���
void FiveStage::Finalize()
{
	actor->Finalize();
}
