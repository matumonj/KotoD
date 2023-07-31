#include "SevenStage.h"
//初期化
void SevenStage::Initialize(DirectXCommon* dxCommon)
{
	//共通の初期化
	BaseInitialize(dxCommon);
	//プレイシーンのアクター
	BaseActor* actor_;
	actor_ = new SevenStageActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//更新
void SevenStage::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//描画
void SevenStage::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//解放
void SevenStage::Finalize()
{
	actor->Finalize();
}