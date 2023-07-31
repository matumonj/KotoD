#pragma once
#include"BaseActor.h"
#include"BossText.h"
#include "MessageWindow.h"
#include<windows.h>
#include<vector>

class FiveStageActor:public BaseActor
{
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
private:

	void IntroUpdate(DebugCamera* camera)override;		//�o��V�[��
	void MainUpdate(DebugCamera* camera)override;		//�o�g���V�[��
	void FinishUpdate(DebugCamera* camera)override;		//���j�V�[��

	void ImGuiDraw();
private:
	static const int SPOT_NUM = 4;
private:
	float Rads;
	int textT;
	unique_ptr<MessageWindow> messagewindow_;
	unique_ptr<BossText> text_;
	unique_ptr<IKESprite> backScreen_ = nullptr;

	//�ۉe(�{�X)
	float BosscircleShadowDir[3] = { 0,-1,0 };
	float BosscircleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float BosscircleShadowFactorAngle[2] = { 0.0f, 2.0f };


	//���̓I�Ɏg��
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

