#pragma once
#include"IKESprite.h"
#include<memory>
#include<array>
#include<vector>
#include<DirectXMath.h>
#include"IKEObject3d.h"
#include "IKETexture.h"
#include "SceneManager.h"
enum SelectState {
	SELECT_FIRST,//最初の牛乳のみ
	SELECT_SECOND,//らすぼす以外開放済み
	SELECT_LAST,//ラスボス開放
};

using namespace DirectX;
using namespace std;
class SelectScene {
public:
	static SelectScene* GetIns();
	SelectScene() {};
	~SelectScene() {};
	bool titf;
	void SetTitF(bool f) { titf = f; }
	bool GetTitF() { return titf; }
	enum Stage {
		FIRST,
		SECOND,
		THIRD,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		TITLE,
		MAX,
	}_stages = MAX;
public:
	/**
	 * \brief 初期化
	 */
	void Init();

	/**
	 * \brief 更新
	 */
	void Upda();

	/**
	 * \brief 描画
	 */
	void Draw_Sprite();
	void Draw_SpriteBack();
	void Draw_Obj(DirectXCommon* dxcomn);
	/**
	 * \brief 諸々リセット
	 */

	void CloseIconView(bool closeF);

	void ViewTips();

	/**
	 * \brief NONのとき初期化用
	 */
	void ResetParama();

	void TipsPosUpda(Stage stage);

	void BirthParticle();

	void ImGuiDraw();

private:
	//状況に応じて管理する
	void StateManager();
private:
	//土台
	unique_ptr<IKEObject3d>Pedestal = nullptr;
	XMFLOAT3 PedestalRot = { 0,0,0 };
	std::unique_ptr<IKETexture>TitleTex;
	std::unique_ptr<IKEObject3d>kotokoObj,sutoponObj;
	bool JumpK, JumpS;
	float JFrameK, JFrameS;
	float KotokoYPos, SutoYPos;
	void Jump();
	//オブジェクト数（ステージ数）
	static constexpr int ObjNum = 8;

	array<unique_ptr<IKEObject3d>, ObjNum>StageObjs = { nullptr };
	array<unique_ptr<IKESprite>, ObjNum>StageObj = { nullptr };
	array<XMFLOAT3, ObjNum>StageObjPos = {};
	array<XMFLOAT3, ObjNum>m_Scale = {};
	array<float, ObjNum>OldObjAngle;
	array<XMFLOAT3, ObjNum>AfterScale = {};
	array<float, ObjNum>TipsPosY = {};
	array<float, ObjNum>ObjColEase = {};
	array<bool, ObjNum>TipsAct = {};
	array<bool, ObjNum>m_Birth = {};
	array<bool, ObjNum>m_BirthFinish = {};
	array<XMFLOAT3, ObjNum>StageObjRot = {};
	array<float, ObjNum>StageObjRotAngle = {};
	array<float, ObjNum>StageObjEaseT = {};
	array<float, ObjNum>IconColor = {};
	array<float, ObjNum> NowRotAngle = {};
	array<XMFLOAT3, ObjNum> m_Color = {};
	float IconRotAngle_EaseT = 0.0f;

	enum Select {
		NOINP,
		RB, LB
	}TrigerSelect = NOINP;
private:
	//スプライト
	unique_ptr<IKESprite> BackSprite;
	unique_ptr<IKESprite> SelQuestSub[2];
	float SelSubAlpha[2];
	float SelQuestalpha;
	unique_ptr<IKESprite> SelQuestSprite;
	array<unique_ptr<IKESprite>, 2>ButtonNav_RBLB = {};
	array<unique_ptr<IKESprite>, 2>ButtonNav_Challenge_Cancel = {};
	array<unique_ptr<IKESprite>, ObjNum>StageName = {};
	array<unique_ptr<IKESprite>, ObjNum>BossIcon = {};

	array<XMFLOAT2, 2>ButtonNav_Challenge_CancelScl;
	array<float, 2>ButtonNav_Challenge_CancelColAlpha;
	array<XMFLOAT2, 2>ButtonNav_Pos = { };

	XMFLOAT3 bossColor;
private:
	bool ChangeF = false;
	bool JudgChal = false;
	bool sin = false;
	bool CloseF = false;

	int SelIndex = 0;

	float SkydomeRotY = 0;
	float closeScl = 6500.f;
	float closeRad = 1500.f;
	float SclingSpeed = 55.f, CorrSpeed = 0.48f;

	float MaxScl, MaxSpeed;
	
	void RotPedestal();
	void ChangeEffect(std::string name, Stage stage, UINT iconnum);
public:
	void SetTexScl(float valscl) { MaxScl = valscl; }
	void SetTexSpeed(float valscl) { MaxSpeed =valscl; }

	float GetCloseIconRad() { return closeRad; }
	float GetCloseScl() { return closeScl; }
	XMFLOAT3 GetPedestalPos() { return Pedestal->GetPosition(); }
	IKEObject3d* GetPedestal() { return Pedestal.get(); }
private:
	
	int m_SelectState = SELECT_FIRST;

public:
	void SetFirstClear() { m_Wide = true; m_SelectState = SELECT_SECOND; }
	const XMFLOAT3& GetNowSelePos() { return m_NowSelePos; }
	void SetSelectState(int SelectState) { m_SelectState = SelectState; };
private:
	int m_BirthTimer = {};
	bool m_Wide = false;
	XMFLOAT3 m_NowSelePos = {};

	bool ChangeLastF;
	float CLastEaseTime;
	void ChangeStageRot();
};