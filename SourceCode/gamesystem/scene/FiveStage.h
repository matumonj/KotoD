#pragma once
#include"BaseScene.h"
#include"FiveStageActor.h"

class FiveStage:public BaseScene
{
public:
	//������
	void Initialize(DirectXCommon* dxCommon) override;
	//�X�V
	void Update(DirectXCommon* dxCommon)override;
	//�`��
	void Draw(DirectXCommon* dxCommon)override;
	//���
	void Finalize()override;
};

