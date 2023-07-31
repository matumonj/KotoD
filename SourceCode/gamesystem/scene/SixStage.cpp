#include "SixStage.h"
//������
void SixStage::Initialize(DirectXCommon* dxCommon)
{
	//���ʂ̏�����
	BaseInitialize(dxCommon);
	//�v���C�V�[���̃A�N�^�[
	BaseActor* actor_;
	actor_ = new SixStageActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//�X�V
void SixStage::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//�`��
void SixStage::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//���
void SixStage::Finalize()
{
	actor->Finalize();
}