#include "ThirdStage.h"
//初期化
void ThirdStage::Initialize(DirectXCommon* dxCommon)
{
	//共通の初期化
	BaseInitialize(dxCommon);
	//プレイシーンのアクター
	BaseActor* actor_;
	actor_ = new ThirdStageActor();
	actor_->Initialize(dxCommon,camera,lightGroup);
	actor.reset(actor_);
}
//更新
void ThirdStage::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//描画
void ThirdStage::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//解放
void ThirdStage::Finalize()
{
	actor->Finalize();
}