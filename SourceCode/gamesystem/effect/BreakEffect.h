#pragma once
#include "InterEffect.h"

//敵を攻撃したときのエフェクト
class BreakEffect : public InterEffect {
public:
	BreakEffect();
	bool Initialize() override;//初期化
private:
	void Action() override;//行動

	void ImGuiOrigin() override;

	void BirthEffect();//エフェクトの発生
};