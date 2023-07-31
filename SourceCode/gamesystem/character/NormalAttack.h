#pragma once
#include "InterAttack.h"
#include"Ghost.h"
class NormalAttack :
	public InterAttack
{
public:
	void Init() override;

	void Upda() override;

	void Draw(DirectXCommon* dxCommon) override;

	void SpriteDraw() override;

	void DeathUpdate(int Timer) override;
private:

};

