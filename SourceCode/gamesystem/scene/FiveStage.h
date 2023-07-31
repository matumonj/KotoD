#pragma once
#include"BaseScene.h"
#include"FiveStageActor.h"

class FiveStage:public BaseScene
{
public:
	//‰Šú‰»
	void Initialize(DirectXCommon* dxCommon) override;
	//XV
	void Update(DirectXCommon* dxCommon)override;
	//•`‰æ
	void Draw(DirectXCommon* dxCommon)override;
	//‰ğ•ú
	void Finalize()override;
};

