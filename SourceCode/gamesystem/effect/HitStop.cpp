#include "HitStop.h"
#include "imgui.h"
#include "CsvLoader.h"
HitStop* HitStop::GetInstance()
{
	static HitStop instance;

	return &instance;
}

void HitStop::LoadCSV() {
	m_TargetTimer = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/effect/HitStop.csv", "Timer")));
}
void HitStop::Update() {
	if (m_HitStop) {
		m_StopTimer++;
		if (m_StopTimer == m_TargetTimer) {
			m_StopTimer = {};
			m_HitStop = false;
		}
	}
}
void HitStop::ImGuiDraw() {
	ImGui::Begin("Hit");
	ImGui::Text("Hit:%d", m_HitStop);
	ImGui::Text("Timer:%d", m_TargetTimer);
	ImGui::End();
}