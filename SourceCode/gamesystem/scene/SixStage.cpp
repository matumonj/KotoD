#include "SixStage.h"
//初期化
void SixStage::Initialize(DirectXCommon* dxCommon)
{
	//共通の初期化
	BaseInitialize(dxCommon);
	//プレイシーンのアクター
	BaseActor* actor_;
	actor_ = new SixStageActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//更新
void SixStage::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//描画
void SixStage::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//解放
void SixStage::Finalize()
{
	actor->Finalize();
}