#pragma once
#include "BaseScene.h"
#include "SixStageActor.h"
//�Q�[���v���C�V�[��
class SixStage : public BaseScene
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