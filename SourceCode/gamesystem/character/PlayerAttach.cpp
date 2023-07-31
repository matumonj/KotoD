#include "PlayerAttach.h"
#include "Helper.h"
#include "Player.h"
#include "CsvLoader.h"
#include <Easing.h>
PlayerAttach::PlayerAttach() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Sutopon);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}
//èâä˙âª
bool PlayerAttach::Initialize() {
	m_Position = { 0.0f,0.0f,0.0f };
	m_Scale = { 0.0f,0.0f,0.0f };
	m_Color = { 1.0f,1.0f,1.0f,0.0f };
	m_AddSpeed = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/bullet/bullet.csv", "Stopon")));
	return true;
}
//çXêV
void PlayerAttach::Update() {
	m_Color.w = 0.0f;
	m_SinAngle = {};
	m_SinAngle2 = {};
}
//ï`âÊ
void PlayerAttach::Draw(DirectXCommon* dxCommon) {
	//
	if (m_Color.w > 0.1f) {
		Obj_Draw();
	}
}
//ImGui
void PlayerAttach::ImGuiDraw() {
	ImGui::Begin("SU");
	ImGui::Text("PosX:%f", m_Position.x);
	ImGui::Text("PosY:%f", m_Position.y);
	ImGui::Text("PosZ:%f", m_Position.z);
	ImGui::End();
}
void PlayerAttach::Particle() {
	XMFLOAT4 s_color = { 1.0f,0.0f,0.0f,1.0f };
	XMFLOAT4 e_color = { 1.0f,1.0f,1.0f,1.0f };
	float s_scale = 3.0f;
	float e_scale = 0.0f;
	if (m_Particle) {
		for (int i = 0; i < 3; i++) {
			ParticleEmitter::GetInstance()->FireEffect(50, m_Position, s_scale, e_scale, s_color, e_color);
		}
	}
}
//èeÇÃèoåª
void PlayerAttach::BirthObj() {
	float l_AfterAlpha = 0.0f;
	float l_AfterScale = 0.0f;
	//èeÇ™å©Ç¶ÇÈ
	if (m_Alive) {
		m_AliveTimer++;
		if (m_AliveTimer > 10) {
			l_AfterAlpha = 0.0f;
			l_AfterScale = 0.0f;
		} else {
			l_AfterAlpha = 1.0f;
			l_AfterScale = 0.7f;
		}

		if (m_AliveTimer == 20) {
			m_Alive = false;
			m_AliveTimer = 0;
			m_Particle = true;
		}
	}

	if (m_Particle) {
		if (Helper::GetInstance()->CheckMin(m_ParticleTimer, 5, 1)) {
			m_Particle = false;
			m_ParticleTimer = 0;
		}
	}

	m_Color.w = Ease(In, Cubic, 0.75f, m_Color.w, l_AfterAlpha);

	m_Scale = { Ease(In,Cubic,0.75f,m_Scale.x,l_AfterScale),
		Ease(In,Cubic,0.75f,m_Scale.y,l_AfterScale),
		Ease(In,Cubic,0.75f,m_Scale.z,l_AfterScale),
	};
}
void PlayerAttach::AppearUpdate(int Timer) {
	m_Scale = { 0.7f,0.7f,0.7f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	if (_AppearState == APPEAR_SET) {
		if (Timer == 1) {
			m_Position = { 0.0f,3.0f,-35.0f };
			m_Rotation = { 0.0f,90.0f,0.0f };
			_AppearState = APPEAR_WALK;
		}
	}
	else if (_AppearState == APPEAR_WALK) {
		m_Position.z += 0.2f;
		if (Helper::GetInstance()->CheckMin(m_Position.z, 10.0f, 0.025f)) {
			_AppearState = APPEAR_STOP;
		}
	}
	else if(_AppearState == APPEAR_WALK2) {
		if (Helper::GetInstance()->CheckMin(m_Position.z, 15.0f, 0.025f)) {
			_AppearState = APPEAR_STOP;
		}
	}
	else {
		if (Timer == 2500) {
			_AppearState = APPEAR_WALK2;
		}
	}
	//sinîgÇ…ÇÊÇ¡Çƒè„â∫Ç…ìÆÇ≠
	m_SinAngle += 3.5f;
	m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
	m_Position.y = (sin(m_SinAngle2) * 0.5f + 3.0f);

	Obj_SetParam();
}
void PlayerAttach::LastDeadUpdate(int Timer) {
	m_Scale = { 0.7f,0.7f,0.7f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	const float l_AddFrame = 0.01f;
	if (_DeathState == DEATH_SET) {
		if (Timer == 1) {
			m_Rotation = { 0.0f,90.0f,0.0f };
			m_Position = { 0.0f,3.0f,-20.0f };
		}
		else if (Timer == 200) {
			m_Position = { 0.0f,3.0f,0.0f };
			_DeathState = DEATH_MOVE;
		}
	}
	else if(_DeathState == DEATH_MOVE) {
		if (Timer == 400) {
			_DeathState = DEATH_TALK;
		}
	}
	else if (_DeathState == DEATH_TALK) {
		if (Timer == 790 || Timer == 1700) {
			_DeathState = DEATH_NO_TALK;
		}

		//sinîgÇ…ÇÊÇ¡Çƒè„â∫Ç…ìÆÇ≠
		m_SinAngle += 6.0f;
		m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
		m_Position.y = (sin(m_SinAngle2) * 0.5f + 3.0f);
	}
	else {
		if (Timer == 1600) {
			_DeathState = DEATH_TALK;
		}
	}

	Obj_SetParam();
}
void PlayerAttach::EndRollUpdate(int Timer) {
	const float l_AddPosX = 0.2f;
	const float l_AddFrame = 0.01f;
	if (_EndState == END_SET) {
		if (Timer == 1) {
			m_Scale = { 0.4f,0.4f,0.4f };
			m_Color = { 1.0f,1.0f,1.0f,1.0f };
			m_Position = { 50.0f,3.0f,-24.0f };
			m_Rotation = { 0.0f,0.0f,0.0f };
			_EndState = END_WALK;
		}
	}
	else if (_EndState == END_WALK) {
		Helper::GetInstance()->CheckMax(m_Position.x, 4.0f, -l_AddPosX);
		if (Timer == 1670) {
			m_EndStop = true;
		}
		if (!m_EndStop) {
			if (Timer % 200 == 0) {
				m_Rot = true;
				m_AfterRot = m_Rotation.y + 360.0f;
			}

			if (m_Rot) {
				if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
					m_Frame = {};
					m_Rotation.y = 0.0f;
					m_Rot = false;
				}
				m_Rotation.y = Ease(In, Cubic, m_Frame, m_Rotation.y, m_AfterRot);
			}
		}
	}

	//sinîgÇ…ÇÊÇ¡Çƒè„â∫Ç…ìÆÇ≠
	if (!m_EndStop) {
		m_SinAngle += 3.0f;
		m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
		m_Position.y = (sin(m_SinAngle2) * 0.5f + 3.0f);
	}
	Obj_SetParam();
}
void PlayerAttach::DarkAppear(int Timer) {
	m_Scale = { 0.7f,0.7f,0.7f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	if (_AppearState == APPEAR_SET) {
		if (Timer == 1) {
			m_Position = { 5.0f,3.0f,-25.0f };
			m_Rotation = { 0.0f,90.0f,0.0f };
			_AppearState = APPEAR_WALK;
		}
	}
	else if (_AppearState == APPEAR_WALK) {
		m_Position.z += 0.2f;
		if (Helper::GetInstance()->CheckMin(m_Position.z, -1.0f, 0.025f)) {
			_AppearState = APPEAR_STOP;
		}
	}
	else if (_AppearState == APPEAR_WALK2) {
		if (Helper::GetInstance()->CheckMin(m_Position.z, 15.0f, 0.025f)) {
			_AppearState = APPEAR_STOP;
		}
	}
	else {
		if (Timer == 2500) {
			_AppearState = APPEAR_WALK2;
		}
	}
	//sinîgÇ…ÇÊÇ¡Çƒè„â∫Ç…ìÆÇ≠
	m_SinAngle += 3.5f;
	m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
	m_Position.y = (sin(m_SinAngle2) * 0.5f + 3.0f);

	Obj_SetParam();
}
void PlayerAttach::DeathUpdate() {
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_Scale = { 0.7f,0.7f,0.7f };
	m_Rotation.y = 135.0f;
	m_Position.x = 30.0f;
	m_Position.z = -50.0f;
	//sinîgÇ…ÇÊÇ¡Çƒè„â∫Ç…ìÆÇ≠
	m_SinAngle += 3.5f;
	m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
	m_Position.y = (sin(m_SinAngle2) * 0.5f + 3.0f);
	Obj_SetParam();
}
void PlayerAttach::TitleUpdate() {
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_Scale = { 0.4f,0.4f,0.4f };

	m_Position.x = -2.0f;
	m_Position.z = -12.0f;
	//sinîgÇ…ÇÊÇ¡Çƒè„â∫Ç…ìÆÇ≠
	m_SinAngle += 3.5f;
	m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
	m_Position.y = (sin(m_SinAngle2) * 0.5f + (1.0f));
	Obj_SetParam();
}