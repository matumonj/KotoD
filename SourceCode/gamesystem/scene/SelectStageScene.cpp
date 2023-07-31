#include "SelectStageScene.h"
#include "SelectSceneActor.h"
//初期化
void SelectStageScene::Initialize(DirectXCommon* dxCommon)
{
	//共通の初期化
	BaseInitialize(dxCommon);
	//プレイシーンのアクター
	BaseActor* actor_;
	actor_ = new SelectSceneActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//更新
void SelectStageScene::Update(DirectXCommon* dxCommon)
{
	actor->Update(dxCommon, camera, lightGroup);
}
//描画
void SelectStageScene::Draw(DirectXCommon* dxCommon)
{
	actor->Draw(dxCommon);
}
//解放
void SelectStageScene::Finalize()
{
	actor->Finalize();
}