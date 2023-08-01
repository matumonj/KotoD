#pragma once
#include "BaseActor.h"
#include "IKESprite.h"
#include "JsonLoader.h"
#include <map>
#include <array>
/// タイトルシーン
class LoadSceneActor : public BaseActor {
public:
	/// 初期化
	void Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// 終了
	void Finalize() override;
	/// 毎フレーム更新
	void Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// 描画
	void Draw(DirectXCommon* dxCommon) override;
	void BackDraw(DirectXCommon* dxCommon);
	void ImGuiDraw(DirectXCommon* dxCommon);
	void SpriteDraw();
private:
	void IntroUpdate(DebugCamera* camera)override;
	void MainUpdate(DebugCamera* camera)override;
	void FinishUpdate(DebugCamera* camera)override;


private://メンバ変数
	int m_LoadTimer = 0;
	const int LoadTimerMax = 60;
	//上下運動の中心
	const XMFLOAT2 CenterPos = { 130.f,630.0f-32.f };
	//加速度
	const float AddMovingVal = 4.0f;
	//文字の間隔
	const float WordsInter = 100.f;
	//縦軸の間隔
	const float space = 30.0f;

	string str = "SELECT";

	bool feedf=true;
	//Loading
	enum {
		text_L = 0,
		text_O,
		text_A,
		text_D,
		text_I,
		Sutopon_1,
		Sutopon_2,
		Sutopon_3,
		Sutopon_4,
		BackScreen,
		SpriteMax
	};

	int sutoponNow = Sutopon_1;
	int sutoponTimer = 0;

	std::array<std::unique_ptr<IKESprite>, SpriteMax> m_Sprites = {};
	std::array<XMFLOAT2, SpriteMax> m_SpritesPos = {};
	std::array<float, SpriteMax> m_SpritesAngle = {};
	std::array<XMFLOAT2, SpriteMax> m_StopPos = {};
};