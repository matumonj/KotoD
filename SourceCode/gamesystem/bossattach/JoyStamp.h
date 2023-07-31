#pragma once
#include "InterStamp.h"

class JoyStamp : public InterStamp {
public:
	JoyStamp();
private:
	bool Initialize(const XMFLOAT3& pos) override;//‰Šú‰»
	void Action() override;//s“®
	void ImGui_Origin() override;//ImGui
	void Origin_Draw(DirectXCommon* dxCommon) override;//
};