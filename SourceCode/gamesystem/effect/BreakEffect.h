#pragma once
#include "InterEffect.h"

//�G���U�������Ƃ��̃G�t�F�N�g
class BreakEffect : public InterEffect {
public:
	BreakEffect();
	bool Initialize() override;//������
private:
	void Action() override;//�s��

	void ImGuiOrigin() override;

	void BirthEffect();//�G�t�F�N�g�̔���
};