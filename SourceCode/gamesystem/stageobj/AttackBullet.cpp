#include "AttackBullet.h"
#include "VariableCommon.h"
#include "CsvLoader.h"
#include "Helper.h"
#include "Easing.h"
AttackBullet::AttackBullet() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Bullet);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);

	m_Model2 = ModelManager::GetInstance()->GetModel(ModelManager::Nucleus);
	m_Object2.reset(new IKEObject3d());
	m_Object2->Initialize();
	m_Object2->SetModel(m_Model2);
}
//èâä˙âª
bool AttackBullet::Initialize() {
	m_Position = { 0.0f,0.0f,0.0f };
	m_Color = { 1.0f,1.0f,1.0f,0.8f };
	//É`ÉÉÅ[ÉWÇÃÉ^ÉCÉvÇ…ÇÊÇ¡ÇƒíeÇÃå©ÇΩñ⁄ïœâª
	if (m_PowerState == POWER_NONE) {
		m_Scale = { 3.0f,3.0f,3.0f };
		m_Object2->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		m_Radius = 5.0f;
	}
	else if (m_PowerState == POWER_MIDDLE) {
		m_Scale = { 4.0f,4.0f,4.0f };
		m_Object2->SetColor({ 1.0f,1.0f,0.0f,1.0f });
		m_Radius = 5.5f;
	}
	else if (m_PowerState == POWER_STRONG) {
		m_Scale = { 5.0f,5.0f,5.0f };
		m_Object2->SetColor({ 1.0f,0.5f,0.0f,1.0f });
		m_Radius = 6.0f;
	}
	else {
		m_Scale = { 6.0f,6.0f,6.0f };
		m_Object2->SetColor({ 1.0f,0.0f,0.0f,1.0f });
		m_Radius = 6.5f;
	}
	//CSVÇ©ÇÁì«Ç›çûÇ›
	m_AddSpeed = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/bullet/bullet.csv", "AttackSpeed")));
	m_TargetTimer = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/bullet/bullet.csv", "AttackLimit")));
	auto PowerSize = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/bullet/bullet.csv", "POWER_MAX")));

	m_Power.resize(PowerSize);

	LoadCSV::LoadCsvParam_Float("Resources/csv/chara/bullet/bullet.csv", m_Power, "Power");
	return true;
}
//ImGuiï`âÊ
void AttackBullet::ImGui_Origin() {
	ImGui::Begin("Attack");
	ImGui::Text("Power:%f", m_DamagePower);
	ImGui::End();
}
//ÉIÉäÉWÉiÉãï`âÊ
void AttackBullet::Draw_Origin() {
	IKEObject3d::PreDraw();
	m_Object2->Draw();
	IKEObject3d::PostDraw();
}
//íeÇÃì¡óLèàóù
void AttackBullet::Action() {
	//ãzé˚Ç≥ÇÍÇƒÇ¢ÇÈÇ©Ç«Ç§Ç©Ç≈íeÇÃãììÆÇåàÇﬂÇÈ
	if (!m_BossCatch) {
		NormalShot();
	}
	else {
		AbsorptionShot();
	}


	if (m_Alive) {
		m_Object2->SetPosition(m_Position);
		m_Object2->SetRotation(m_Rotation);
		m_Object2->SetScale({ m_Scale.x - 0.3f, m_Scale.y - 0.3f, m_Scale.z - 0.3f });
		m_Object2->Update();
		Obj_SetParam();
		
	}
}
//í èÌÇÃíe
void AttackBullet::NormalShot() {
	const float l_AddRot = 20.0f;
	if (m_Alive) {
		if (m_PowerState == POWER_NONE) {
			m_DamagePower = m_Power[0] * m_NumMag;
			//m_Color = { 1.0f,1.0f,1.0f,1.0f };
		}
		else if (m_PowerState == POWER_MIDDLE) {
			m_DamagePower = m_Power[1] * m_NumMag;
			//m_Color = { 1.0f,0.0f,0.0f,1.0f };
		}
		else if (m_PowerState == POWER_STRONG) {
			m_DamagePower = m_Power[2] * m_NumMag;
			//m_Color = { 0.0f,1.0f,0.0f,1.0f };
		}
		else {
			m_DamagePower = m_Power[3] * m_NumMag;
			//m_Color = { 1.0f,0.0f,0.0f,1.0f };
		}
		//à⁄ìÆÇâ¡éZ
		m_Position.x += m_Angle.x * m_AddSpeed;
		m_Position.z += m_Angle.y * m_AddSpeed;
		m_Rotation.x += l_AddRot;
		m_Rotation.z += l_AddRot;
		//àÍíËéûä‘óßÇ¬Ç∆è¡Ç¶ÇÈ
		if (Helper::GetInstance()->CheckMin(m_Timer, m_TargetTimer, 1)) {
			m_Timer = 0;
			m_Alive = false;
		}

		m_MatRot = m_Object->GetMatrot();
	}

}
//ãzÇ¢çûÇ›ÇÃíe
void AttackBullet::AbsorptionShot() {
	const float l_SubSpeed = 4.0f;
	const float l_SubScale = 0.3f;

	m_CircleSpeed += l_SubSpeed;
	
	if (Helper::GetInstance()->CheckMax(m_CircleScale, 0.0f, -l_SubScale)) {
		m_Alive = false;
	}

	m_AfterPos = Helper::GetInstance()->CircleMove(m_TargetPos, m_CircleScale, m_CircleSpeed);

	m_Position = {
		Ease(In,Cubic,0.5f,m_Position.x,m_AfterPos.x),
		m_Position.y,
		Ease(In,Cubic,0.5f,m_Position.z,m_AfterPos.z),
	};
}