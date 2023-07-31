#pragma once
#include "InterStamp.h"

class AngerStamp : public InterStamp {
public:
	AngerStamp();
private:
	bool Initialize(const XMFLOAT3& pos) override;//������
	void Action() override;//�s��
	void ImGui_Origin() override;//ImGui
	void Origin_Draw(DirectXCommon* dxCommon) override;//

	void BirthParticle();	//�p�[�e�B�N���̐���

	bool Collide();	//�����蔻��

private:
	void AngerMove();

private:
	float m_DamagePower = {};

	enum StampState {
		STAMP_SET,
		STAMP_WIDE,
	}_StampState;

	enum ExploState {
		EXPLO_SMALL,
		EXPLO_MIDIUM,
		EXPLO_LARGE
	};

	int m_ExploType = {};

	int m_DeadTimer = {};

	bool m_Explo = false;

	float m_AlphaFrame = {};

	bool m_Audio = false;
};