#pragma once
#include "FirstBoss.h"
#include "SecondBoss.h"
#include "ThirdBoss.h"
#include "FourthBoss.h"
#include"FiveBoss.h"
#include "SixBoss.h"
#include "SevenBoss.h"
#include<memory>
#include <array>
//エンドロールのOBJ
class EndRollObj {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	EndRollObj();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(DirectXCommon* dxCommon);
	//スプライト描画
	void SpriteDraw();
	//ImGui
	void ImGuiDraw();

private:
	static const int BOSS_NUM = 7;
private:

	//ボス
	std::array<unique_ptr<InterBoss>, BOSS_NUM>boss;

	enum Boss {
		BIN,
		BOOK,
		STAMP,
		CAMERA,
		DARK,
		DJ,
		LAST
	};
};