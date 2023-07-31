#include "IntroductionScene.h"
//初期化
void IntroductionScene::Initialize(DirectXCommon* dxCommon) {
	//共通の初期化
	BaseInitialize(dxCommon);
	//導入のアクター
	BaseActor* actor_;
	actor_ = new IntroductionSceneActor();
	actor_->Initialize(dxCommon, camera, lightGroup);
	actor.reset(actor_);
}
//更新
void IntroductionScene::Update(DirectXCommon* dxCommon) {
	actor->Update(dxCommon, camera, lightGroup);
}
//描画
void IntroductionScene::Draw(DirectXCommon* dxCommon) {
	actor->Draw(dxCommon);
}
//解放
void IntroductionScene::Finalize() {
	actor->Finalize();
}