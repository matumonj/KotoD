#pragma once
#include "InterCD.h"

class BarrangeCD :
	public InterCD {
public:
	BarrangeCD();
	bool Initialize() override;//‰Šú‰»
	
	void Origin_Draw(DirectXCommon* dxCommon) override;//•`‰æ

	void BirthCD() override;
	void StayCD() override;
	void CatchCD() override;
	void DeathCD() override;
	void ResPornCD() override;
public:

private:
	void Action() override;//s“®

	void AudioAction() override;//s“®
	
	void ImGui_Origin() override;
private:

};