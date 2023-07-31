#pragma once
#include "BaseActor.h"
#include "IKESprite.h"
#include "IKEObject3d.h"

/// タイトルシーン
class GameOverSceneActor : public BaseActor {
public:
	/// 初期化
	void Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// 終了
	void Finalize() override;
	/// 毎フレーム更新
	void Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// 描画
	void Draw(DirectXCommon* dxCommon) override;
	void BackDraw(DirectXCommon* dxCommon);
	void ImGuiDraw(DirectXCommon* dxCommon);
	void FrontDraw();

	void IntroUpdate(DebugCamera* camera) override;
	void MainUpdate(DebugCamera* camera) override;
	void FinishUpdate(DebugCamera* camera) override;


	string NextStageName();
private://メンバ変数
	string str = "";
	//スプライト
	unique_ptr<IKESprite> ClearSprite = nullptr;
	
	array<unique_ptr<IKESprite>, 3>FrontSprite = { };
	array<float, 3> alphas = {};
	unique_ptr<IKEObject3d> SutoponObj = nullptr;
	XMFLOAT3 m_eye = { 0,10,20 };
	float frame = 0.0f;
	float heartframe = 0.0f;
	const float kFrameMax = 120.0f;
	float radius = 0.f;
	float sca = 0.0f;
};
