#pragma once
#include "BaseScene.h"

#include "EditorSceneActor.h"
//�Q�[���v���C�V�[��
class EditorScene : public BaseScene
{
public:
	//������
	void Initialize(DirectXCommon* dxCommon) override;
	//�X�V
	void Update(DirectXCommon* dxCommon) override;
	//�`��
	void Draw(DirectXCommon* dxCommon) override;
	//�J��
	void Finalize() override;
};