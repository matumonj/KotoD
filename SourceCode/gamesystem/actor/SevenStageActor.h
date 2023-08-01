#pragma once
#include "BaseActor.h"
#include "BossText.h"
#include "LoadStageObj.h"
#include"Font.h"
#include"Feed.h"
#include "MessageWindow.h"
#include"Spline.h"

/// タイトルシーン
class SevenStageActor : public BaseActor {

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
	
	void TextRead();
	void AwakeText();
	void DeathText();

	void WhoRead(const string& name);
private:
private:
	float Rads;
	int textT;
	unique_ptr<MessageWindow> messagewindow_;
	unique_ptr<BossText> text_;
	unique_ptr<IKESprite> SkipUI = nullptr;
	unique_ptr<IKESprite> backScreen_ = nullptr;

	//丸影(ボス)
	float BosscircleShadowDir[3] = { 0,-1,0 };
	float BosscircleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float BosscircleShadowFactorAngle[2] = { 0.0f, 2.0f };

	int m_AppTimer = 0;
	int m_EndTimer = 0;
	int m_AwakeTimer = 0;

	enum AppState {
		APP_START,
		APP_NOTICE,
		APP_VANISH,
		APP_END,
	}_AppState = APP_START;
};