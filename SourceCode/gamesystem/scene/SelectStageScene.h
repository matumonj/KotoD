#pragma once
#include "BaseScene.h"
class SelectStageScene :
    public BaseScene
{
public:
	//‰Šú‰»
	void Initialize(DirectXCommon* dxCommon) override;
	//XV
	void Update(DirectXCommon* dxCommon) override;
	//•`‰æ
	void Draw(DirectXCommon* dxCommon) override;
	//ŠJ•ú
	void Finalize() override;
};

