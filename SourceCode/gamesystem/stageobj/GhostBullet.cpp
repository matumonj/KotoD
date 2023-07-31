#include "GhostBullet.h"
#include "VariableCommon.h"
#include "CsvLoader.h"
#include "Easing.h"
#include "Helper.h"
GhostBullet::GhostBullet() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::GhostBullet);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}
//������
bool GhostBullet::Initialize() {
	//CSV����ǂݍ���
	m_AddSpeed = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/bullet/bullet.csv", "GhostSpeed")));
	m_TargetTimer = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/bullet/bullet.csv", "GhostLimit")));
	m_StartScale = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/bullet/bullet.csv", "StartScale")));
	m_EndScale = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/bullet/bullet.csv", "EndScale")));
	m_Position = { 0.0f,0.0f,0.0f };
	m_Scale = { m_StartScale,m_StartScale,m_StartScale };
	m_Color = { 0.0f,0.0f,0.0f,1.0f };
	return true;
}
//ImGui�`��
void GhostBullet::ImGui_Origin() {
}
//�I���W�i���`��
void GhostBullet::Draw_Origin() {
}
//�e�̓��L����
void GhostBullet::Action() {
	//�^�C�v�ɂ���ĐF����U�ς��Ă�
	if (m_BulletType == BULLET_FORROW) {
		m_Color = { 1.0f,0.3f,0.0f,1.0f };
	}
	else {
		m_Color = { 0.3f,1.0f,0.3f,1.0f };
	}
	if (m_Alive) {
		//�ړ������Z
		m_Position.x += m_Angle.x * m_AddSpeed;
		m_Position.z += m_Angle.y * m_AddSpeed;
		m_MatRot = m_Object->GetMatrot();
	}

	VanishBullet();

	if (m_Alive) {
		Obj_SetParam();
	}
}
//�e�����X�ɏ����鏈��
void GhostBullet::VanishBullet() {
	float l_AddFrame = 0.1f;
	if (m_Alive) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		}
		else {
			m_Frame = 1.0f;
			//��莞�ԗ��Ə�����
			if (Helper::GetInstance()->CheckMin(m_Timer, m_TargetTimer, 1)) {
				m_Timer = 0;
				m_Alive = false;
				m_Frame = {};
			}
		}

		m_Scale = {
			Ease(In, Cubic, m_Frame, 1.0f, 2.0f),
			Ease(In, Cubic, m_Frame, m_StartScale, m_EndScale),
			Ease(In, Cubic, m_Frame, m_StartScale, m_EndScale)
		};
	}
}