
#pragma once
#include "BaseActor.h"
#include "BossText.h"
#include "LoadStageObj.h"
#include"Font.h"
#include"Feed.h"
#include "MessageWindow.h"
#include"Spline.h"

/// �^�C�g���V�[��
class FourthStageActor : public BaseActor {

public:
	/// ������
	void Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// �I��
	void Finalize() override;
	/// ���t���[���X�V
	void Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// �`��
	void Draw(DirectXCommon* dxCommon) override;
	void FrontDraw(DirectXCommon* dxCommon);
	void BackDraw(DirectXCommon* dxCommon);
	void ImGuiDraw(DirectXCommon* dxCommon);
	void ColEnemy(std::vector<InterEnemy*> enelist);

	void IntroUpdate(DebugCamera* camera)override;		//�o��V�[��
	void MainUpdate(DebugCamera* camera)override;		//�o�g���V�[��
	void FinishUpdate(DebugCamera* camera)override;		//���j�V�[��


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

	//�ۉe(�{�X)
	float BosscircleShadowDir[3] = { 0,-1,0 };
	float BosscircleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float BosscircleShadowFactorAngle[2] = { 0.0f, 2.0f };
};