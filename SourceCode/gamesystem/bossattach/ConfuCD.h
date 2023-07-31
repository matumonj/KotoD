#pragma once
#include "InterCD.h"

class ConfuCD :
	public InterCD {
public:
	ConfuCD();
	bool Initialize() override;//������

	void Origin_Draw(DirectXCommon* dxCommon) override;//�`��

	void BirthCD() override;
	void StayCD() override;
	void CatchCD() override;
	void DeathCD() override;
	void ResPornCD() override;
public:

private:
	void Action() override;//�s��

	void AudioAction() override;//�s��

	void ImGui_Origin() override;
private:

};