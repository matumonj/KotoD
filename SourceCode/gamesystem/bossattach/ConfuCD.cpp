#include "ConfuCD.h"
#include "ModelManager.h"
#include "Helper.h"
#include "Player.h"
#include <random>
ConfuCD::ConfuCD() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::CD);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}
//������
bool ConfuCD::Initialize() {
	//�����w��
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_distX(-50, -20);
	uniform_int_distribution<int> l_distZ(-50, -20);
	m_Position = { float(l_distX(mt)),30.0f,float(l_distZ(mt)) };
	m_Scale = { 0.8f,0.8f,0.8f };
	m_Color = { 0.3f,0.8f,1.0f,1.0f };
	m_CDState = CD_BIRTH;
	CsvLoad();
	return true;
}
//�s��
void ConfuCD::Action() {
	(this->*stateTable[static_cast<size_t>(m_CDState)])();
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
	CollideBul(_playerBulA);
	Obj_SetParam();
	SetCD();
}
//�`��
void ConfuCD::Origin_Draw(DirectXCommon* dxCommon) {
	Obj_Draw();
}
//ImGui
void ConfuCD::ImGui_Origin() {
}

//����
void ConfuCD::BirthCD() {
	//��Ԃ悤�Ȋ����ɂ��邽�ߏd�͂�����
	m_AddPower -= m_Gravity;
	Helper::GetInstance()->CheckMax(m_Position.y, 0.0f, m_AddPower);

	if (m_Position.y == 0.0f) {
		m_CDState = CD_STAY;
	}
}

//�X�e�[�W�ɂ�����
void ConfuCD::StayCD() {

}

//�{�X����ɓ��ꂽ���
void ConfuCD::CatchCD() {
	if (m_CatchState == CATCH_END) {
		m_Rotation.y += 3.0f;
		m_Position = { m_CatchPos.x,m_CatchPos.y,m_CatchPos.z };
	}
}

//������
void ConfuCD::DeathCD() {

}

void ConfuCD::ResPornCD() {
	m_ResPornTimer++;
	const int l_SetTimer = 5;
	const int l_LimitTimer = 10;

	if (m_ResPornTimer == l_SetTimer) {
		//�����w��
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_distX(-50, -20);
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

void ConfuCD::AudioAction() {
	if (_AudioState == AUDIO_SET) {
		if (m_AudioPlay) {
			m_AudioPlay = false;
		}
	}
	else if (_AudioState == AUDIO_END) {
	}
}