#include "FiveStage.h"
//初期化
void FiveStage::Initialize(DirectXCommon* dxCommon)
{
	//共通の初期化
	BaseInitialize(dxCommon);
	//プレイシーンのアクター
	BaseActor* actor_;
	actor_ = new FiveStageActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//更新
void FiveStage::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//描画
void FiveStage::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//解放
void FiveStage::Finalize()
{
	actor->Finalize();
}
