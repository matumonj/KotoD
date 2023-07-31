#pragma once
#include "BaseActor.h"
#include "IKESprite.h"
/// �N���A�V�[��
class ClearSceneActor : public BaseActor {
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
private://�����o�ϐ�
	string str="";
	//�X�v���C�g
	unique_ptr<IKESprite> ClearSprite;
};
