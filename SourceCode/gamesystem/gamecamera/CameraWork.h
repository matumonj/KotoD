#pragma once
#include "DebugCamera.h"
#include "Shake.h"
#include <memory>
#include "Feed.h"
#include "InterBoss.h"
#include"Spline.h"
using namespace std;         //  名前空間指定

//カメラの種類
enum CameraState {
	CAMERA_NORMAL,
	CAMERA_LOAD,
	CAMERA_BOSSAPPEAR,
	CAMERA_BOSSDEAD_BEFORE,
	CAMERA_BOSSDEAD_AFTER_FIRST,
	CAMERA_BOSSDEAD_AFTER_SECOND,
	CAMERA_BOSSDEAD_AFTER_THIRD,
	CAMERA_BOSSDEAD_AFTER_FOURTH,
	CAMERA_BOSSDEAD_AFTER_FIVE,
	CAMERA_BOSSDEAD_AFTER_SIX,
	CAMERA_BOSSDEAD_AFTER_SEVEN,
	CAMERA_BOSS_STRONG,
	CAMERA_TITLE,
};

//ボス登場シーンのカメラの動き(2個目のボスがこの量なだけでボスによってはへらしてもいいよ)
enum AppearCameraType {
	APPEAR_START,
	APPEAR_SECOND,
	APPEAR_THIRD,
	APPEAR_FOURTH,
	APPEAR_FIVE,
	APPEAR_SIX,
	APPEAR_SEVEN,
	APPEAR_EIGHT,
	APPEAR_END,
};

//ゲームのカメラの動き
class CameraWork {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private:
	static void (CameraWork::* stateTable[])();
public:
	CameraWork(XMFLOAT3 eye = { 2.0f, 30.0f, 2.0f }, XMFLOAT3 target = { 2.0f, 0.0f, 3.0f });
	void SplineSet();
	void Update(DebugCamera* camera);//更新
	void ImGuiDraw();

public:
	void EditorCamera();

private:
	void DefaultCam();//ふつうのカメラ
	void BossAppear();//登場
	void SetBossDead_Before();//撃破
	void SetBossDead_AfterFirst();//撃破時（フェード後）
	void SetBossDead_AfterSecond();//撃破時（フェード後）
	void SetBossDead_AfterThird();//撃破時（フェード後）
	void SetBossDead_AfterFourth();//撃破時（フェード後）
	void SetBossDead_AfterFive();//撃破時（フェード後）
	void SetBossDead_AfterSix();//撃破時（フェード後）
	void SetBossDead_AfterSeven();//撃破時（フェード後）
	//void SetBoss(InterBoss* boss) { this->boss = boss; }
	//ゲームシーン以外で使うカメラ更新(this変数で変更可能)
	void SpecialUpdate();//更新
	void StrongCamera();
	void TitleCamera();//タイトルカメラ
public:
	//void EditorCamera();
	void ClearDraw();
	void BossDefeatcamera();
	void feedDraw();

	bool GetFeedEnd() { return FeedEndF; }
private:
	bool FeedEndF = false;
	int DeathTimer = 0;
	bool FeedF = false;;
	bool AppearEndF = false;

private://各ボスの登場カメラ
	Spline* spline;
	vector<XMFLOAT3>pointsList;
	//各ボスの登場ムービー
	void FirstBossAppear();
	void SecondBossAppear();
	void ThirdBossAppear();
	void FourthBossAppear();
	void FiveBossAppear();
	void SixBossAppear();
	void SevenBossAppear();

	void FirstBossDead_AfterFeed();
	//円運動のカメラセットの際のやつ
	void SetCircleCameraTarget();
	//円運動のカメラセットの際のやつ
	void SetCircleCameraEye(const XMFLOAT3& target,const XMFLOAT3& basepos);

private:
	void SetEaseCamera();
public:
	bool Finish;
	int Timer_first = 1;
	bool StartSpl;
	float RadEffect;
	bool PlusRad;
	float SplineSpeed;
	float GetEffectPower() { return RadEffect; }
	enum FirstBossCamState {
		ONE,
		TWO,
		THREE

	}_firstState;
	//getter setter
	bool Feed_Spline;
	bool FinishAppear() { if (spline->GetIndex() >= static_cast<int>(static_cast<unsigned long long>(pointsList.size()) - 1))return true; return false; }

	void SetBoss(InterBoss* boss) { this->boss = boss; }

	void SetEye(const XMFLOAT3& eye) { m_eyePos = eye; }
	XMFLOAT3& GetEye() { return m_eyePos; }
	void SetTarget(const XMFLOAT3& target) { m_targetPos = target; }
	XMFLOAT3& GetTarget() { return m_targetPos; }

	void SetFeedF(const bool Feed) { FeedF = Feed; }

	bool GetFeedF() { return FeedF; }

	bool GetCameraStrong() { return m_CameraStrong; }

	void SetCameraStrong(const bool CameraStrong) { m_CameraStrong = CameraStrong; }

	bool GetEndDeath() { return m_EndDeath; }
	void SetEndDeath(const bool EndDeath) { m_EndDeath = EndDeath; }

	bool GetAppearEndF() { return AppearEndF; }

	bool GetEndStrong() { return m_EndStrong; }

	bool GetChangeStrong() { return m_ChangeStrong; }

	int GetAppearType() { return m_AppearType; }

	//
	void SetCameraState(const int CameraState) { m_CameraState = CameraState; }

	void SetLastTimer(const int LastTimer) { m_LastTimer = LastTimer; }
	void SetEndTimer(const int EndTimer) { m_EndTimer = EndTimer; }

	//シーンネームの取得
	void SetSceneName(std::string name) { SceneName = name; }

	void SetApproach(const bool Approach) { m_Approach = Approach; }

	void SetCameraSkip(const bool CameraSkip) { m_CameraSkip = CameraSkip; }

	bool CameraStateisNormal() { if (m_CameraState != CAMERA_BOSSAPPEAR)return true; return false; }

	int GetCameraState() { return m_CameraState; }

	void DefUpda(DebugCamera* camera);
private:
	//クラス
	unique_ptr<Shake> shake = nullptr;
	//イージングの変数
	float m_Frame = 0.0f;
	float m_FrameMax = 30.0f;
	XMFLOAT3 m_BeforeEye = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_AfterEye = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_BeforeTarget = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_AfterTarget = { 0.0f,0.0f,0.0f };
	//視点座標
	XMFLOAT3 m_eyePos = { 0.0f,0.0f,0.0f };
	//注視点座標
	XMFLOAT3 m_targetPos = { 0.0f,0.0f,0.0f };

	InterBoss* boss = nullptr;

	//カメラの状態
	int m_CameraState = 0;

	unique_ptr<Feed> feed;
	//シーンネーム
	std::string SceneName;

	//カメラのタイマー
	int m_CameraTimer = 0;

	//円運動のため
	float m_PI = 3.14f;
	float m_CameraRadius = 0.0f;
	float m_CameraSpeed = 90.0f;
	float m_CameraScale = 20.0f;
	float m_CameraCircleX = 0.0f;
	float m_CameraCircleZ = 0.0f;

	float m_AfterSpeed = {};
	float m_AfterScale = 5.0f;

	int m_AppearType = {};

	//カメラが寄る
	bool m_Approach = false;

	//撃破カメラ終了
	bool m_EndDeath = false;

	//カメラをスキップするか
	bool m_CameraSkip = false;

	bool m_CameraStrong = false;

	bool m_ChangeStrong = false;

	enum StrongState {
		STRONG_ONE,
		STRONG_SECOND,
		STRONG_THIRD,
	}_StrongState;

	int m_StrongTimer = 0;

	bool m_EndStrong = false;
	bool m_Finish = false;
	bool m_NearBoss = false;

	//ラスボスのカメラ時間
	int m_LastTimer = 0;

	//撃破時間
	int m_EndTimer = 0;

	enum LastState {
		LAST_SET,
		LAST_BOSS,
		LAST_PLAYER,
		LAST_UPBOSS,
		LAST_ZOOMBOSS,
		LAST_FARBOSS,
		LAST_BATTLE,
		LAST_BATTLE2
	}_LastState;

	enum DeathCamera {
		DEATH_SET,
		DEATH_BOSS_UP,
		DEATH_PLAYER,
		DEATH_LOOK_BOSS,
		DEATH_END
	}_DeathCamera = DEATH_SET;

	bool m_LookPlayer = false;
};