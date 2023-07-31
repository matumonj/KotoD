#include "Heart.h"
#include "Easing.h"
#include "Collision.h"
#include "CsvLoader.h"
#include <random>
#include "Player.h"
#include "Helper.h"
#include "Audio.h"
Heart::Heart() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Heart);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}
//初期化
bool Heart::Initialize() {
	const int l_Division = 80;
	//乱数指定
	m_Scale = { 1.2f,1.2f,1.2f };
	m_Rotation = { 0.0f,0.0f,0.0f };
	m_Alive = true;
	m_AliveTimer = 0;
	_heartState = HeartState::HEART_JUMP;
	//乱数の範囲(-50から50)
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_bounddist(-80, 80);
	uniform_int_distribution<int> l_bounddist2(50, 80);

	m_AddPower = {
		(float)(l_bounddist(mt)) / l_Division,
		(float)(l_bounddist2(mt)) / l_Division,
		(float)(l_bounddist(mt)) / l_Division,
	};

	m_LimitTimer = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/heart/heart.csv", "Limit")));
	return true;
}
//状態遷移
/*CharaStateのState並び順に合わせる*/
void (Heart::* Heart::stateTable[])() = {
	&Heart::HeartJump,//飛ばす
	&Heart::HeartSet,//ストップ
	&Heart::HeartVanish,//ハートが消える
};
//更新
void Heart::Update() {
	//状態移行(charastateに合わせる)
	(this->*stateTable[_heartState])();
	m_Rotation.y += 1.0f;
	//リミット制限
	Helper::GetInstance()->Clamp(m_Position.x, -50.0f, 60.0f);
	Helper::GetInstance()->Clamp(m_Position.z, -55.0f, 55.0f);
	//タイプによって色を一旦変えてる
	Obj_SetParam();
	//パーティクル
	Particle();
	//当たり判定(プレイヤー)
	PlayerCollision();

	if (m_Vanish) {
		_heartState = HEART_VANISH;
	}
}
//描画
void Heart::Draw(DirectXCommon* dxCommon) {
	if (m_Alive) {
		Obj_Draw();
	}
}
//ImGui描画
void Heart::ImGuiDraw() {
}
//パーティクル
void Heart::Particle() {
	XMFLOAT4 s_color = { 0.5f,1.0f,0.1f,1.0f };
	XMFLOAT4 e_color = { 0.5f,1.0f,0.1f,1.0f };
	float s_scale = 5.0f;
	float e_scale = 0.0f;
	if (m_Alive) {
		ParticleEmitter::GetInstance()->HealEffect(100, { m_Position.x,m_Position.y,m_Position.z }, s_scale, e_scale, s_color, e_color);
	}
}
//当たり判定(プレイヤー)
bool Heart::PlayerCollision() {
	float l_VolumeMag = 0.4f;//音量の倍率
	if (!m_Alive) { return false; }
	if (Player::GetInstance()->PlayerCollide(m_Position) && (_heartState == HeartState::HEART_SET)) {
		if (Player::GetInstance()->GetHP() <= 0.0f) {
			return false; }
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Heal01.wav", VolumManager::GetInstance()->GetSEVolum() * l_VolumeMag);
		m_Vanish = true;
		if (Player::GetInstance()->GetHP() <= 4.0f) {
			Player::GetInstance()->SetHP(Player::GetInstance()->GetHP() + 1.0f);
		}
		return true;
	} else {
		return false;
	}

	return true;
}

//ハートを飛ばす
void Heart::HeartJump() {
	m_AddPower.y -= m_Gravity;
	m_Position = Helper::GetInstance()->Float3AddFloat3(m_Position, m_AddPower);

	if (m_Position.y <= 0.0f) {
		_heartState = HeartState::HEART_SET;
	}
}

//ハートの着地後
void Heart::HeartSet() {
	const float l_AddFrame = 0.05f;

	m_Rotation.y += 2.0f;
	m_AliveTimer++;

	//一定時間で消える
	if (m_AliveTimer >= m_LimitTimer) {
		m_Vanish = true;
	}
}

//ハートがなくなる時
void Heart::HeartVanish() {
	const float l_AddFrame = 0.05f;

	if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
		m_Alive = false;
		m_AliveTimer = {};
		m_Frame = {};
	}
	
	//徐々に小さくなる
	m_Scale = { Ease(In,Cubic,m_Frame,m_Scale.x,0.0f),
	Ease(In,Cubic,m_Frame,m_Scale.y,0.0f),
	Ease(In,Cubic,m_Frame,m_Scale.z,0.0f)
	};
}
