
#pragma once
#include "BaseActor.h"
#include "BossText.h"
#include "LoadStageObj.h"
#include"Font.h"
#include"Feed.h"
#include "MessageWindow.h"
#include"Spline.h"

/// タイトルシーン
class FourthStageActor : public BaseActor {

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
	void ColEnemy(std::vector<InterEnemy*> enelist);

	void IntroUpdate(DebugCamera* camera)override;		//登場シーン
	void MainUpdate(DebugCamera* camera)override;		//バトルシーン
	void FinishUpdate(DebugCamera* camera)override;		//撃破シーン


private:
	bool ShutterEffect();
	bool ShutterFeed();
	void ShutterReset();

	bool isShutter = false;
	float shutterTime = 0.0f;
	float shutterTimeMax = 30.0f;
	float stopTime = 0.0f;
	float stopTimerMax = 0.0f;
	float feedTimeMax = 10.0f;
	float feedTimer = 0.0f;
	float shutterHight[2] = { 0,0 };
	enum {
		Photo_Out_Top,
		Photo_Out_Under,
		SpriteMax,
	};
	array<unique_ptr<IKESprite>, SpriteMax> photo = {};
	IKEModel* m_Model = nullptr;
	unique_ptr<IKEObject3d> apple = nullptr;
	bool isVisible = false;

	int m_AppTimer = 0;

	unique_ptr<MessageWindow> messagewindow_;
	unique_ptr<BossText> text_;
	unique_ptr<IKESprite> SkipUI = nullptr;
	unique_ptr<IKESprite> backScreen_ = nullptr;

	Spline* spline=nullptr;
	vector<XMFLOAT3> pointsList = {};

	enum class TextScene {
		NON,
		TIEYOSHI_EXP,
		STOPON_SPK,
		KILL_TIEYOSHI,
		LET_GO,
		ENDTEXT
	}_Tscne = TextScene::ENDTEXT;

	//丸影(ボス)
	float BosscircleShadowDir[3] = { 0,-1,0 };
	float BosscircleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float BosscircleShadowFactorAngle[2] = { 0.0f, 2.0f };
};