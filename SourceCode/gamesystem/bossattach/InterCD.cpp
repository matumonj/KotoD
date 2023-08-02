#include "InterCD.h"
#include "Collision.h"
#include "CsvLoader.h"
#include <random>
#include "Helper.h"
void (InterCD::* InterCD::stateTable[])() = {
	&InterCD::BirthCD,//生成
	&InterCD::StayCD, //放置
	&InterCD::CatchCD,//所持
	&InterCD::DeathCD,//なくなった
	&InterCD::ResPornCD,//りすぽーん
};

//CDV読み込み
void InterCD::CsvLoad() {
	m_HP = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/CD.csv", "hp")));
}

void InterCD::Update() {
	Action();

	//エフェクト
	for (InterEffect* effect : effects) {
		if (effect != nullptr) {
			effect->Update();
		}
	}

	//マークの削除
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i] == nullptr) {
			continue;
		}

		if (!effects[i]->GetAlive()) {
			effects.erase(cbegin(effects) += i);
		}
	}
}
void InterCD::Draw(DirectXCommon* dxCommon) {
	if(m_CDState != CD_DEATH && m_CDState != CD_RESPORN)
	Obj_Draw();
}
void InterCD::ImGuiDraw() {
	ImGui::Begin("CD");
	ImGui::Text("POSY:%f", m_Position.y);
	ImGui::End();
	ImGui_Origin();
}

//弾との当たり判定
void InterCD::CollideBul(vector<InterBullet*> bullet)
{
	if (m_CDState != CD_STAY)return;
	const float l_Radius = 1.5f;
	for (InterBullet* _bullet : bullet) {
		if (_bullet != nullptr && _bullet->GetAlive()) {
			if (Collision::CircleCollision(_bullet->GetPosition().x, _bullet->GetPosition().z, _bullet->GetRadius(), m_Position.x, m_Position.z, l_Radius)) {
				BirthEffect();
				_bullet->SetAlive(false);
				m_HP -= _bullet->GetPower();
				if (m_HP <= 0.0f) {
					m_CDState = CD_DEATH;
					m_BreakCD = true;
				}
			}
		}
	}	
}

void InterCD::SetCD() {
	if (m_AttackSetCD) {
		if (m_CatchState == CATCH_SET) {
			m_AddPower = 0.5f;
			m_CatchState = CATCH_MOVE;
		}
		else if (m_CatchState == CATCH_MOVE) {
			m_AddPower -= m_Gravity;
			if (Helper::GetInstance()->CheckMax(m_Position.y, m_CatchPos.y, m_AddPower) && m_AddPower < -1.0f) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/CDSet.wav", VolumManager::GetInstance()->GetSEVolum());
				m_CatchState = CATCH_END;
				m_AttackSetCD = false;
			}
		}
	}
}

//エフェクトの発生
void InterCD::BirthEffect() {
	InterEffect* neweffect;
	neweffect = new BreakEffect();
	neweffect->Initialize();
	neweffect->SetPosition(m_Position);
	neweffect->SetDiviSpeed(1.0f);
	neweffect->SetLife(50);
	neweffect->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	effects.push_back(neweffect);
}

//死んだときの動き
void InterCD::DeathMove(const int Timer, const int TargetTimer) {
	m_CDState = CD_STAY;
	const int l_Division = 50;
	//
	if (!m_DeathMove) {
		m_DeathTimer++;
		if (m_DeathTimer == 1) {
			m_AudioPlay = true;
		}
		if (Timer < TargetTimer) {
			m_Position = m_CatchPos;
		}
		else{
			//乱数生成(加算力と大きさ)
			mt19937 mt{ std::random_device{}() };
			uniform_int_distribution<int> l_bounddist(-80, 80);
			uniform_int_distribution<int> l_bounddist2(30, 60);
			m_BoundPower = {
		(float)(l_bounddist(mt)) / l_Division,
		(float)(l_bounddist2(mt)) / l_Division,
		(float)(l_bounddist(mt)) / l_Division,
			};
			m_DeathMove = true;
			_AudioState = AUDIO_END;
			m_DeathTimer = {};
		}
	}
	else {
		//飛ぶような感じにするため重力を入れる
		m_BoundPower.y -= m_Gravity;
		Helper::GetInstance()->Float3AddFloat3(m_Position, m_BoundPower);
		if (Helper::GetInstance()->CheckMax(m_Position.y, 0.0f, m_BoundPower.y)) {
			m_BoundPower = {};
		}
	}

	AudioAction();
	Obj_SetParam();
}

void InterCD::EndMove(const int TargetTimer) {
	m_EndTimer++;
	if (_EndState == END_SET) {
		if (m_EndTimer == 1) {
			m_Position = { 25.0f,50.0f,5.0f };
			m_Scale = { 0.6f,0.6f,0.6f };
		}
		else if (m_EndTimer == TargetTimer) {
			_EndState = END_MOVE;
		}
	}
	else if (_EndState == END_MOVE) {
		//飛ぶような感じにするため重力を入れる
		m_BoundPower.y -= m_Gravity;
		Helper::GetInstance()->Float3AddFloat3(m_Position, m_BoundPower);
		if (Helper::GetInstance()->CheckMax(m_Position.y, 2.0f, m_BoundPower.y)) {
			m_Position.x = 500.0f;
			m_BoundPower = {};
		}
	}
	Obj_SetParam();
}