#pragma once
#include "BaseActor.h"
#include <vector>
#include<windows.h>
#include "BossText.h"
/// タイトルシーン
class ThirdStageActor : public BaseActor {
public:
	/// 初期化
	void Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// 終了
	void Finalize() override;
	/// 毎フレーム更新
	void Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// 描画
	void Draw(DirectXCommon* dxCommon) override;
	void FrontDraw(DirectXCommon* dxCommon);
	void BackDraw(DirectXCommon* dxCommon);
	void ImGuiDraw(DirectXCommon* dxCommon);
private:

	void IntroUpdate(DebugCamera* camera)override;		//登場シーン
	void MainUpdate(DebugCamera* camera)override;		//バトルシーン
	void FinishUpdate(DebugCamera* camera)override;		//撃破シーン

	unique_ptr<BossText> text_;
	unique_ptr<IKESprite> SkipUI = nullptr;
	unique_ptr<IKESprite> backScreen_ = nullptr;
private:

	//導入シーン
	enum class AppState : int {
		ANGER_START,
		ANGER_Third,
		JOY_START,
		JOY_Third,
		JOY_THIRD,
		SELECT_EMO,
		EMO_JOY,
		EMO_JOY2,
		EMO_ANGER,
		EMO_ANGER2,
	};

	//シーンでの遷移
	AppState m_AppState = AppState::ANGER_START;

	int m_AppTimer = 0;
	//丸影(ボス)
	float BosscircleShadowDir[3] = { 0,-1,0 };
	float BosscircleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float BosscircleShadowFactorAngle[2] = { 1.8f, 1.8f };
};