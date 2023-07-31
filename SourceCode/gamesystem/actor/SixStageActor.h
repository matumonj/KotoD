﻿#pragma once
#include "BaseActor.h"
#include "Audio.h"
#include"Easing.h"
#include "SceneManager.h"
#include "imgui.h"
#include "ParticleEmitter.h"
#include "ImageManager.h"
#include <algorithm>
#include "BackObj.h"
#include "Menu.h"
#include "SelectScene.h"
#include "BossText.h"
#include "LoadStageObj.h"
#include"Font.h"
#include"Feed.h"
#include "MessageWindow.h"
#include"Spline.h"

/// タイトルシーン
class SixStageActor : public BaseActor {

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

	void MoveSpotLight();
	void SpotSet(XMFLOAT3& Pos, const XMFLOAT3& AfterPos, const float AddFrame);

private:
	static const int SPOT_NUM = 4;
private:
	float Rads;
	int textT;
	unique_ptr<MessageWindow> messagewindow_;
	unique_ptr<BossText> text_;
	unique_ptr<IKESprite> backScreen_ = nullptr;

	//丸影(ボス)
	float BosscircleShadowDir[3] = { 0,-1,0 };
	float BosscircleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float BosscircleShadowFactorAngle[2] = { 0.0f, 2.0f };


	//棘の的に使う
	float m_Angle[SPOT_NUM] = {};
	float m_Angle2[SPOT_NUM] = {};

	float m_AddPos = {};

	int m_AppTimer = 0;


	enum AppState {
		APP_START,
		APP_NOTICE,
		APP_VANISH,
		APP_END,
	}_AppState = APP_START;
};