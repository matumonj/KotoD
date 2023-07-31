#pragma once
#include<array>
//飢餓ゲージを管理するクラス
class HungerGauge {
public:
	static HungerGauge* GetInstance();
	bool Initialize();//初期化
	void Update();
	/// <summary>
	/// ImGui描画
	/// </summary>
	void ImGuiDraw();
public:
	//gettersetter
	float GetNowHunger() { return   m_NowHunger; }
	float GetHungerMax() { return   m_HungerMax; }
	float GetAdditional() { return   m_Additional; }
	int GetCatchCount() { return   m_CatchCount; }
	bool GetFirstCarry() { return   carriedFood; }
	void ResetFirstCarry() { carriedFood = false; }

	float GetPercentage();

	float GetPercentageExtra();

	void AddNowHunger(float m_NowHunger);

	void RecoveryNowHunger(float AddHunger);

	void SetIsStop(bool isStop) { this->isStop= isStop; }
	void SetNowHunger(float m_NowHunger) { this->m_NowHunger = m_NowHunger; }
	void SetHungerMax(float m_HungerMax) { this->m_HungerMax = m_HungerMax; }
	void SetCatchCount(int m_CatchCount) { this->m_CatchCount = m_CatchCount; }
	void SetSubVelocity(float m_SubVelocity) { this->m_SubVelocity = m_SubVelocity; }
	void SetAdditional(float m_Additional) { this->m_Additional = m_Additional; }
public:
	static float m_Hungervalue;//加算や余算の値
private:
	static const int SUB_MAX = 5;
private:
	float m_NowHunger = {};//現在の飢餓ゲージ
	float m_HungerMax = {};//飢餓ゲージの最大数(変化する)
	int m_CatchCount = {};//ゴーストの数
	float m_SubVelocity = 1.0f;//減る飢餓ゲージ(チャージ中のみ減る）
	std::array<float,SUB_MAX> m_SubHunger;//減る量
	bool carriedFood = false;
	bool isStop = false;

	//超過する飢餓ゲージ(これが亡くならないと飢餓ゲージは減らない)
	float m_Additional = {};
	float m_SubExtra = {};
};
