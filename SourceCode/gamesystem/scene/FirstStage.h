#pragma once
#include "BaseScene.h"
#include "FirstStageActor.h"
//�Q�[���v���C�V�[��
class FirstStage : public BaseScene
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