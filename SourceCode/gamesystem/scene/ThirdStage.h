#pragma once
#include "BaseScene.h"
#include "ThirdStageActor.h"
//�Q�[���v���C�V�[��
class ThirdStage : public BaseScene
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