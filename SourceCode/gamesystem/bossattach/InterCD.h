#pragma once
#include "ObjCommon.h"
#include "InterBullet.h"
#include "BreakEffect.h"
enum CDState {
	CD_BIRTH,
	CD_STAY,
	CD_CATCH,
	CD_DEATH,
	CD_RESPORN,
};
using namespace std;         //  名前空間指定
//音符クラス
class InterCD :
public ObjCommon {
protected:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	//CDVロード
	void CsvLoad();
	//初期化
	virtual bool Initialize() = 0;
	//更新
	void Update();
	//描画
	void Draw(DirectXCommon* dxCommon);

	//描画(固有の)
	virtual void Origin_Draw(DirectXCommon* dxCommon) = 0;

	void ImGuiDraw();//ImGuiの描画

	void DeathMove(const int Timer,const int TargetTimer);

	void EndMove(const int TargetTimer);
protected:

	virtual void Action() = 0;//ボス特有の処理

	virtual void AudioAction() = 0;//ボス特有の処理(音楽)

	virtual void ImGui_Origin() = 0;//ボスそれぞれのImGui

	void CollideBul(vector<InterBullet*>bullet);

	//攻撃前のCDのセット
	void SetCD();
	//エフェクト発生
	void BirthEffect();
protected:
	//メンバ関数
	virtual void BirthCD() {};
	virtual void StayCD() {};
	virtual void CatchCD() {};
	virtual void DeathCD() {};
	virtual void ResPornCD() {};
public:
	//gettersetter
	const int& GetCDState() { return m_CDState; }
	const bool& GetBreakCD() { return m_BreakCD; }

	void SetCDState(const int CDState) { m_CDState = CDState; }

	void SetBreakCD(const bool BreakCD) { m_BreakCD = BreakCD; }

	void SetAttackSetCD(const bool AttackSetCD) { m_AttackSetCD = AttackSetCD; }

	void SetCatchPos(const XMFLOAT3 CatchPos) { m_CatchPos = CatchPos; }
protected:
	static void (InterCD::* stateTable[])();
private:
protected:
	vector<InterEffect*> effects;
	//CDの状態
	int m_CDState = CD_BIRTH;

	//上昇度
	float m_AddPower = 0.0f;
	//重力加速度
	float m_Gravity = 0.03f;
	//キャッチした後のポジション
	XMFLOAT3 m_CatchPos = {};
	
	//リスポーン時間
	int m_ResPornTimer = {};

	//壊れたかドウか
	bool m_BreakCD = false;

	//CDをキャッチした
	enum CatchState {
		CATCH_SET,
		CATCH_MOVE,
		CATCH_END,
	};

	bool m_AttackSetCD = false;
	int m_CatchState = CATCH_SET;

	float m_HP = {};

	bool m_DeathMove = false;
	XMFLOAT3 m_BoundPower = {};

	int m_BoundCount = {};

	int m_DeathTimer = {};

	enum AudioState {
		AUDIO_SET,
		AUDIO_END,
	}_AudioState = AUDIO_SET;

	bool m_AudioPlay = false;

	enum EndState {
		END_SET,
		END_MOVE,
		END_FINISH
	}_EndState;

	int m_EndTimer = {};
};