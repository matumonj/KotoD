#pragma once
#include "BaseScene.h"
#include "TitleSceneActor.h"
/// �^�C�g���V�[��
class TitleScene : public BaseScene {
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