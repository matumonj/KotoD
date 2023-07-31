#include "Poltergeist.h"
#include "Collision.h"
#include "CsvLoader.h"
#include "Player.h"
#include "Helper.h"
#include "Easing.h"
#include <random>
Poltergeist::Poltergeist() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::CD);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}
//������
bool Poltergeist::Initialize() {
	m_Position = { 0.0f,0.0f,0.0f };
	m_Rotation.y = 270.0f;
	m_Scale = { 0.3f,0.3f,0.3f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_AddSpeed = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "Speed")));
	m_Alive = true;
	m_AfterPos.y = 6.0f;
	m_ThrowType = THROW_SET;
	m_AliveTimer = {};

	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_distX(-5, 5);
	uniform_int_distribution<int> l_distZ(-5, 5);
	m_Power.x = float(l_distX(mt)) / 10.0f;
	m_Power.y = float(l_distZ(mt)) / 10.0f;

	if (m_Power.x == 0.0f) {
		m_Power.x = 0.1f;
	}

	if (m_Power.y == 0.0f) {
		m_Power.y = 0.1f;
	}
	return true;
}
//��ԑJ��
/*CharaState��State���я��ɍ��킹��*/
void (Poltergeist::* Poltergeist::stateTable[])() = {
	&Poltergeist::Follow,//�Ǐ]
	&Poltergeist::Bound,//���˒e
};
//�X�V
void Poltergeist::Update() {
	//��Ԉڍs(charastate�ɍ��킹��)
	(this->*stateTable[m_PolterType])();
	//�^�C�v�ɂ���ĐF����U�ς��Ă�
	Obj_SetParam();

	Collide();
	Particle();
}
//�`��
void Poltergeist::Draw(DirectXCommon* dxCommon) {
	Obj_Draw();
}
//ImGui�`��
void Poltergeist::ImGuiDraw() {
	ImGui::Begin("Polter");
	ImGui::Text("Timer:%d", m_AliveTimer);
	ImGui::Text("POSY:%f", m_Position.y);
	ImGui::End();
}

//�p�[�e�B�N��
void Poltergeist::Particle() {
	XMFLOAT4 s_color = { 0.0f,0.4f,1.0f,1.0f };
	XMFLOAT4 s_color2 = { 0.4f,0.0f,1.0f,1.0f };
	XMFLOAT4 e_color = { 1.0f,1.0f,1.0f,1.0f };
	float s_scale = 2.0f;
	float e_scale = 0.0f;
	const int m_Life = 50;
	if (m_Alive) {
		if (m_PolterType == TYPE_FOLLOW) {
			ParticleEmitter::GetInstance()->FireEffect(m_Life, m_Position, s_scale, e_scale, s_color, e_color);
		}
		else {
			ParticleEmitter::GetInstance()->FireEffect(m_Life, m_Position, s_scale, e_scale, s_color2, e_color);
		}
	}
}

//�����蔻��
bool Poltergeist::Collide() {
	XMFLOAT3 l_PlayerPos = Player::GetInstance()->GetPosition();
	const float l_Damage = 0.5f;
	const float l_Radius = 1.0f;
	if (Collision::CircleCollision(m_Position.x, m_Position.z, l_Radius, l_PlayerPos.x, l_PlayerPos.z, l_Radius) && (Player::GetInstance()->GetDamageInterVal() == 0)) {
		Player::GetInstance()->RecvDamage(l_Damage);
		Player::GetInstance()->PlayerHit(m_Position);
		m_Alive = false;
		return true;
	}
	else {
		return false;
	}

	return false;
}
//�Ǐ]
void Poltergeist::Follow() {
	const float l_AddFrame = 0.01f;
	const int l_BaseTimer = 40;

	//�e�̃Z�b�g(���񂾂񕂂��ш��ӂ���悤�Ȋ���)
	if (m_ThrowType == THROW_SET) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		}
		else {
			m_Frame = {};
			m_ThrowType = THROW_INTER;
		}
	
		m_Position.y = Ease(In, Cubic, m_Frame, m_Position.y, m_AfterPos.y);
	}
	//�_�����������߂�
	else if (m_ThrowType == THROW_INTER) {
		m_ThrowTimer++;
		if (m_ThrowTimer == l_BaseTimer + m_TargetTimer) {
			double sb, sbx, sbz;
			sbx = Player::GetInstance()->GetPosition().x - m_Position.x;
			sbz = Player::GetInstance()->GetPosition().z - m_Position.z;
			sb = sqrt(sbx * sbx + sbz * sbz);
			m_SpeedX = sbx / sb * 0.8;
			m_SpeedZ = sbz / sb * 0.8;
			m_ThrowTimer = 0;
			m_ThrowType = THROW_PLAY;
		}
	}
	//���ۂɑ_�����Ⴄ
	else {
		//�v���C���[�ɃX�s�[�h���Z
		m_Position.x += (float)m_SpeedX;
		m_Position.z += (float)m_SpeedZ;

		if (Helper::GetInstance()->CheckNotValueRange(m_Position.x, -70.0f, 80.0f) || Helper::GetInstance()->CheckNotValueRange(m_Position.z, -75.0f, 75.0f)) {
			m_Alive = false;
		}
	}

}
//���˒e
void Poltergeist::Bound() {
	const float l_AddFrame = 0.01f;
	const float l_AddCircle = 2.0f;
	const int l_BaseTimer = 50;
	const int l_LimitTimer = 1000;
	//�e�̃Z�b�g(���񂾂񕂂��ш��ӂ���悤�Ȋ���)
	if (m_ThrowType == THROW_SET) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		}
		else {
			m_Frame = {};
			m_ThrowType = THROW_INTER;
		}

		m_Position.y = Ease(In, Cubic, m_Frame, m_Position.y, m_AfterPos.y);
		m_Position = Helper::GetInstance()->CircleMove({ m_BasePos.x,m_Position.y,m_BasePos.z }, m_CircleScale, m_CircleSpeed);
	}
	//���Ԃ�����܂ŉ�]����
	else if (m_ThrowType == THROW_INTER) {
		XMVECTOR move = { 0.0f, 0.0f, 0.1f, 0.0f };
		XMMATRIX matRot = {};
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_RandomRot(0, 360);
		m_ThrowTimer++;
		m_CircleSpeed += l_AddCircle;
		if (m_ThrowTimer == l_BaseTimer + m_TargetTimer) {
			matRot = XMMatrixRotationY(XMConvertToRadians(float(l_RandomRot(mt))));
			move = XMVector3TransformNormal(move, matRot);
			m_Angle.x = move.m128_f32[0];
			m_Angle.y = move.m128_f32[2];
			m_ThrowType = THROW_PLAY;
		}

		m_Position = Helper::GetInstance()->CircleMove({ m_BasePos.x,m_Position.y,m_BasePos.z }, m_CircleScale, m_CircleSpeed);
	}
	//�K���ɓ�����
	else {
		m_AliveTimer++;
		//�v���C���[�ɃX�s�[�h���Z
		m_Position.x += m_Angle.x * m_AddSpeed;
		m_Position.z += m_Angle.y * m_AddSpeed;

		//���ˏ���
		if (Helper::GetInstance()->CheckNotValueRange(m_Position.x, -65.0f, 75.0f)) {
			m_Angle.x *= -1.0f;
		}

		if (Helper::GetInstance()->CheckNotValueRange(m_Position.z, -70.0f, 70.0f)) {
			m_Angle.y *= -1.0f;
		}

		if (m_AliveTimer == l_LimitTimer) {
			m_Alive = false;
		}
	}
}