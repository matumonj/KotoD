#include "AttackNote.h"
#include "Collision.h"
#include "CsvLoader.h"
#include "Player.h"
#include "Helper.h"
AttackNote::AttackNote() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::NOTE);
	m_Model2 = ModelManager::GetInstance()->GetModel(ModelManager::THORN);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}
//‰Šú‰»
bool AttackNote::Initialize() {
	m_Position = { 0.0f,0.0f,0.0f };
	m_Rotation.y = 270.0f;
	m_Object->SetBillboard(true);
	m_Scale = { 1.0f,1.0f,1.0f };
	m_Color = { 1.0f,0.2f,0.2f,1.0f };
	m_AddSpeed = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "BulSpeed")));
	m_TargetTimer = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "Timer")));
	m_Damage = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "BarrangeDamage")));

	if (m_Change) {
		m_Object->SetModel(m_Model2);
		m_Scale = { 1.3f,1.3f,1.3f };
	}
	return true;
}
//ó‘Ô‘JˆÚ
/*CharaState‚ÌState•À‚Ñ‡‚É‡‚í‚¹‚é*/
void (AttackNote::* AttackNote::stateTable[])() = {
	&AttackNote::Follow,
	&AttackNote::Diffusion,
};
//XV
void AttackNote::Update() {
	//ó‘ÔˆÚs(charastate‚É‡‚í‚¹‚é)
	(this->*stateTable[_charaState])();
	_charaState = STATE_DIFF;
	//ƒ^ƒCƒv‚É‚æ‚Á‚ÄF‚ðˆê’U•Ï‚¦‚Ä‚é
	Obj_SetParam();

	Collide();
}
//•`‰æ
void AttackNote::Draw(DirectXCommon* dxCommon) {
	Obj_Draw();
}
//ImGui•`‰æ
void AttackNote::ImGuiDraw() {
	/*ImGui::Begin("Note");
	ImGui::Text("Timer:%d", m_Timer);
	ImGui::Text("m_TargetTimer:%d", m_TargetTimer);
	ImGui::End();*/
}

//ƒp[ƒeƒBƒNƒ‹
void AttackNote::Particle() {
	
}

//’Ç]
void AttackNote::Follow() {
	m_Timer++;
	float l_Vel = 0.35f;//‘¬“x
	XMFLOAT3 l_playerPos = Player::GetInstance()->GetPosition();
	if (m_Timer < m_TargetTimer) {
		Helper::GetInstance()->FollowMove(m_Position, l_playerPos, l_Vel);
	}
	else {
		m_Timer = {};
		m_Alive = false;
	}
}

//ŠgŽU’e
void AttackNote::Diffusion() {
	m_Position.x += m_Angle.x * m_AddSpeed;
	m_Position.z += m_Angle.y * m_AddSpeed;

	if (Helper::GetInstance()->CheckNotValueRange(m_Position.x, -60.0f, 70.0f) || Helper::GetInstance()->CheckNotValueRange(m_Position.z, -65.0f, 65.0f)) {
		m_Alive = false;
	}
}

//“–‚½‚è”»’è
bool AttackNote::Collide() {
	XMFLOAT3 l_PlayerPos = Player::GetInstance()->GetPosition();

	const float l_Radius = 1.0f;
	if (Collision::CircleCollision(m_Position.x, m_Position.z, l_Radius, l_PlayerPos.x, l_PlayerPos.z, l_Radius) && (Player::GetInstance()->GetDamageInterVal() == 0)) {
		Player::GetInstance()->RecvDamage(m_Damage);
		Player::GetInstance()->PlayerHit(m_Position);
		m_Alive = false;
		return true;
	}
	else {
		return false;
	}

	return false;
}