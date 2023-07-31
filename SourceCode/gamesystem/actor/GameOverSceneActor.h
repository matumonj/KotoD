#pragma once
#include "BaseActor.h"
#include "IKESprite.h"
#include "IKEObject3d.h"

/// �^�C�g���V�[��
class GameOverSceneActor : public BaseActor {
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


	string NextStageName();
private://�����o�ϐ�
	string str = "";
	//�X�v���C�g
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
