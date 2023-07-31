#pragma once
//ヒットストップようなのでリソースは要らない
class HitStop {
public:
	static HitStop* GetInstance();
	void LoadCSV();
	//更新処理のみ
	void Update();
	void ImGuiDraw();
public:
	//getter setter
	bool GetHitStop() { return  m_HitStop; }
	void SetHitStop(bool HitStop) { this->m_HitStop = HitStop; }
private:
	bool m_HitStop = false;//ヒットストップ
	int m_StopTimer = 0;//ヒットストップの時間
	int m_TargetTimer = 0;//指定の時間
};