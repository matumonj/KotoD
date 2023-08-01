#pragma once
#include <map>
#include <string>
#include "IKESprite.h"
#include "IKETexture.h"
#include "ParticleManager.h"

//2Dの読み込み処理
class ImageManager final
{
private:
	ImageManager() = default;
	~ImageManager() = default;
	ImageManager(const ImageManager& r) = default;
	ImageManager& operator=(const ImageManager& r) = default;

public:
	//スプライトの名前
	enum texName
	{
		TITLEWORD,
		SELECT,
		LOADING_LO,
		LOADING_O,
		LOADING_DI,
		LOADING_N,
		LOADING_G,
		SUTOPONMOVE,
		PLAY,
		GAMEOVER,
		GAMEOVER_1,
		GAMEOVER_2,
		GAMEOVER_3,
		WHITE,
		WINDOW,
		BLACKWINDOW,
		HEART,
		PlayerHPGauge,
		BossHPGauge,
		UnderGauge,
		Gauge,
		EXGauge,
		GIRL,
		SUTOPON,
		FEED,
		MENU_HOME,
		MENU_RESET,
		MENU_RULE,
		MENU_SCENECHANGE,
		MENU_FRAME,
		CHECK_HOME,
		CHECK_OK,
		CHECK_NO,
		EXPLANATION_1,
		EXPLANATION_2,
		EXPLANATION_3,
		EXPLANATION_4,
		CLOSEMILK,
		CLOSESYTOPON,
		CLOSEKIDO,
		CLOSEDJ,
		CLOSEDARK,
		CLOSECAMERA,
		CLOSELAST,
		CLOSEHOME,
		CLOSEBRO,
		PHOTO_IN,
		PHOTO_IN_Change,
		PHOTO_IN_Control,
		PHOTO_IN_Sub,
		PHOTO_IN_Ult,
		PHOTO_OUT,
		CIRCLE,
		CIRCLECOVER,
		RBBUTTON,
		LBBUTTON,
		tip1,
		tip2,
		tip3,
		tip4,
		tip5,
		tip6,
		tip7,
		tip8,
		Challenge,
		Cancel,
		BOSS_ARROW,
		NOTE,
		NOTE2,
		HITODAMA,
		TITLESELECT,
		SELQUEsT,
		SELQUEStsuB,
		CLEARLOGO,
		CREDIT,
		FIN,
		DESCRIPTION1,
		DESCRIPTION2,
		DESCRIPTION3,
		DESCRIPTION4,
		SKIPUI,
	};
	//テクスチャの名前
	enum tex2dName
	{
		IMPACT,
		IMPACT2,
		ANGER,
		JOY,
		DAMAGEAREA,
		GROUND,
		PHOTOSPOT,
		STUN,
		CATCH,
		SLASHAREA,
		DARKBOM,
		SHADOW,
		TITLETIPS
	};
	enum particleName
	{
		Normal,
		Smoke,
		Heal,
		Photo,
	};

public:
	static ImageManager* GetInstance(); //取得用
	void StartLoad2D();
	void SecondLoad2D();
	void StartLoadTex2D();
	void SecondLoadTex2D();
	void LoadParticle();
};
