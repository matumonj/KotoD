#pragma once
#include "BaseScene.h"
#include "SevenStageActor.h"
//�Q�[���v���C�V�[��
class SevenStage : public BaseScene
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