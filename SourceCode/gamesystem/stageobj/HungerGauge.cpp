#include "HungerGauge.h"
#include "CsvLoader.h"
#include "Helper.h"
#include "WinApp.h"
float HungerGauge::m_Hungervalue = 5.0f;
HungerGauge* HungerGauge::GetInstance() {
	static HungerGauge instance;

	return &instance;
}

//初期化
bool HungerGauge::Initialize() {
	m_CatchCount = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/hunger/hungergauge.csv", "Catch")));
	m_NowHunger = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/hunger/hungergauge.csv", "Now")));
	m_HungerMax = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/hunger/hungergauge.csv", "Max")));
	m_SubExtra = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/hunger/hungergauge.csv", "SubExtra")));
	//だるいんで一旦これで
	m_SubHunger[0] = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/hunger/hungergauge.csv", "Sub1")));
	m_SubHunger[1] = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/hunger/hungergauge.csv", "Sub2")));
	m_SubHunger[2] = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/hunger/hungergauge.csv", "Sub3")));
	m_SubHunger[3] = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/hunger/hungergauge.csv", "Sub4")));
	m_SubHunger[4] = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/hunger/hungergauge.csv", "Sub5")));

	return true;
}

//更新
void HungerGauge::Update() {
	float l_Limit = 50.0f;
	float l_AdditionalLimit = 10.0f;
	//一定ずつで減少していく(超過分がなくなった場合)
	if (Helper::GetInstance()->CheckMax(m_Additional, 0.0f, -m_SubExtra)) {
		if (!isStop) {
			if (m_CatchCount <= 5 && m_CatchCount > 0) {
				m_NowHunger -= m_SubHunger[m_CatchCount - 1];
			}
			else {
				m_NowHunger -= m_SubHunger[SUB_MAX - 1];
			}
		}
		else {
			if (m_CatchCount <= 5 && m_CatchCount > 0) {
				m_NowHunger -= m_SubHunger[m_CatchCount - 1] * 0.8f;
			}
			else {
				m_NowHunger -= m_SubHunger[SUB_MAX - 1] * 0.8f;
			}
		}
	}
	//飢餓ゲージの最大数が決まっている
	Helper::GetInstance()->Clamp(m_NowHunger, 0.0f, m_HungerMax);
	m_NowHunger = min(m_NowHunger, m_HungerMax);
	m_HungerMax = min(m_HungerMax, l_Limit);
	Helper::GetInstance()->Clamp(m_Additional, 0.0f, l_AdditionalLimit);
}

//ImGui
void HungerGauge::ImGuiDraw() {
	ImGui::Begin("Hunger");
	ImGui::Text("NowHunger:%f", m_NowHunger);
	ImGui::Text("Additional:%f", m_Additional);
	ImGui::End();
}

float HungerGauge::GetPercentage() {
	float temp = m_NowHunger / 50.f;
	Helper::GetInstance()->Clamp(temp, 0.0f, 1.0f);
	return temp;
}


float HungerGauge::GetPercentageExtra() {
	float temp = (m_NowHunger + m_Additional) / 50.f;
	Helper::GetInstance()->Clamp(temp, 0.0f, 1.0f);
	return temp;
}

void HungerGauge::AddNowHunger(float m_NowHunger) {
	float temp = m_NowHunger;
	temp = min(m_NowHunger, m_HungerMax);
	this->m_NowHunger = temp;
}

void HungerGauge::RecoveryNowHunger(float AddHunger) {
	carriedFood = true;
	float add = m_NowHunger + AddHunger;
	Helper::GetInstance()->Clamp(add, 0.f, m_HungerMax);
	SetNowHunger(add);
}
