#pragma once
#include "BaseScene.h"
#include "FourthStageActor.h"
//�Q�[���v���C�V�[��
class FourthStage : public BaseScene
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