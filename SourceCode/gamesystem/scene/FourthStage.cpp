#include "FourthStage.h"
//������
void FourthStage::Initialize(DirectXCommon* dxCommon)
{
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�v���C�V�[���̃A�N�^�[
	BaseActor* actor_;
	actor_ = new FourthStageActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void FourthStage::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void FourthStage::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//���
void FourthStage::Finalize()
{
	actor->Finalize();
}