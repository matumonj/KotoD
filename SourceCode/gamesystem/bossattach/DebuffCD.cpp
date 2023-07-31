#include "DebuffCD.h"
#include "ModelManager.h"
#include "Helper.h"
#include "Player.h"
#include <random>
DebuffCD::DebuffCD() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::CD);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}
//初期化
bool DebuffCD::Initialize() {
	//乱数指定
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_distX(20, 50);
	uniform_int_distribution<int> l_distZ(-50, -20);
	m_Position = { float(l_distX(mt)),30.0f,float(l_distZ(mt)) };
	m_Scale = { 0.8f,0.8f,0.8f };
	m_Color = { 0.3f,1.0f,0.4f,1.0f };
	m_CDState = CD_BIRTH;
	CsvLoad();
	return true;
}
//特有の動き
void DebuffCD::Action() {
	(this->*stateTable[static_cast<size_t>(m_CDState)])();
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
	CollideBul(_playerBulA);
	Obj_SetParam();
	SetCD();
}
//描画
void DebuffCD::Origin_Draw(DirectXCommon* dxCommon) {
	Obj_Draw();
}
//ImGui
void DebuffCD::ImGui_Origin() {

}

//生成
void DebuffCD::BirthCD() {
	//飛ぶような感じにするため重力を入れる
	m_AddPower -= m_Gravity;
	Helper::GetInstance()->CheckMax(m_Position.y, 0.0f, m_AddPower);

	if (m_Position.y == 0.0f) {
		m_CDState = CD_STAY;
	}
}

//ステージにいる状態
void DebuffCD::StayCD() {

}

//ボスが手に入れた状態
void DebuffCD::CatchCD() {
	if (m_CatchState == CATCH_END) {
		m_Rotation.y += 3.0f;
		m_Position = { m_CatchPos.x,m_CatchPos.y,m_CatchPos.z };
	}
}

//消えた
void DebuffCD::DeathCD() {

}

//復活
void DebuffCD::ResPornCD() {
	m_ResPornTimer++;
	const int l_SetTimer = 5;
	const int l_LimitTimer = 10;

	if (m_ResPornTimer == l_SetTimer) {
		//乱数指定
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_distX(20, 50);
		uniform_int_distribution<int> l_distZ(-50, -20);
		m_Position = { float(l_distX(mt)),30.0f,float(l_distZ(mt)) };
		m_AddPower = {};
		CsvLoad();
	}
	else if (m_ResPornTimer == l_LimitTimer) {
		m_CDState = CD_BIRTH;
		m_CatchState = CATCH_SET;
		m_ResPornTimer = {};
	}
}

void DebuffCD::AudioAction() {
	if (_AudioState == AUDIO_SET) {
		if (m_AudioPlay) {
			m_AudioPlay = false;
		}
	}
	else if (_AudioState == AUDIO_END) {
	}
}