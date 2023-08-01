#pragma once
#include "BaseActor.h"
#include "IKESprite.h"
#include "IKEObject3d.h"
#include "EndRollObj.h"
/// �^�C�g���V�[��
class EndRollActor : public BaseActor {
public:
	/// ������
	void Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// �I��
	void Finalize() override;
	/// ���t���[���X�V
	void Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// �`��
	void Draw(DirectXCommon* dxCommon) override;
	void BackDraw(DirectXCommon* dxCommon);
	void ImGuiDraw(DirectXCommon* dxCommon);
	void FrontDraw();

	void IntroUpdate(DebugCamera* camera) override;
	void MainUpdate(DebugCamera* camera) override;
	void FinishUpdate(DebugCamera* camera) override;

private:
	bool ShutterEffect();
	bool ShutterFeed();
	void ShutterReset();
private://�����o�ϐ�
	unique_ptr<IKESprite> SkipUI = nullptr;
	unique_ptr<EndRollObj> endobj;
	//�|�X�g�G�t�F�N�g�̒l
	float m_Sepia = 0.0f;
	//�t���[��
	float m_Frame = 0.0f;

	int m_EndTimer = {};


	bool isShutter = false;
	float shutterTime = 0.0f;
	float shutterTimeMax = 30.0f;
	float stopTime = 0.0f;
	float stopTimerMax = 20.0f;
	float feedTimeMax = 10.0f;
	float feedTimer = 0.0f;
	float shutterHight[2] = { 0,0 };
	enum {
		Photo_Out_Top,
		Photo_Out_Under,
		SpriteMax,
	};
	array<unique_ptr<IKESprite>, SpriteMax> photo = {};
	unique_ptr<IKESprite> fin;
	XMFLOAT2 m_Size = {};
	bool m_Fin = false;
	bool m_Change = false;
};
