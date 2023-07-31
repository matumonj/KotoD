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
//�G���h���[����OBJ
class EndRollObj {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	EndRollObj();
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw(DirectXCommon* dxCommon);
	//�X�v���C�g�`��
	void SpriteDraw();
	//ImGui
	void ImGuiDraw();

private:
	static const int BOSS_NUM = 7;
private:

	//�{�X
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