#pragma once
#include "BaseScene.h"
#include "TutorialSceneActor.h"
/// �^�C�g���V�[��
class TutorialScene : public BaseScene {
public:
	/// ������
	void Initialize(DirectXCommon* dxCommon) override;
	/// �I��
	void Finalize() override;
	/// ���t���[���X�V
	void Update(DirectXCommon* dxCommon) override;
	/// �`��
	void Draw(DirectXCommon* dxCommon) override;
};