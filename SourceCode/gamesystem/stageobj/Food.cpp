#include "Food.h"
#include "Collision.h"
#include "HungerGauge.h"
#include "Player.h"
#include <random>
#include "Easing.h"
#include "Helper.h"
Food::Food() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Food);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}
//初期化
bool Food::Initialize() {
	//乱数指定
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_distX(-50, 60);
	uniform_int_distribution<int> l_distY(10, 15);
	uniform_int_distribution<int> l_distZ(-55, 55);
	m_Position = { float(l_distX(mt)), float(l_distY(mt)),float(l_distZ(mt)) };
	uniform_int_distribution<int> l_degree(0, 360);
	m_Degree = float(l_degree(mt));
	m_Scale = { 1.0f,1.0f,1.0f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_LockOn = false;
	m_Alive = true;
	return true;
}
//更新
void Food::Update() {
	if (FallSpawn()) { return; }
	if (CarriedGhost()) { return; }
	//タイプによって色を一旦変えてる
	Obj_SetParam();
	//食料生成
	BirthFood();
	//パーティクル
	Particle();
	//当たり判定
	//Collision();

	//時間制限による削除(今のところ2個目のステージのみ)
	DeleteFood();
}
//描画
void Food::Draw(DirectXCommon* dxCommon) {
	if (m_Alive) {
		Obj_Draw();
	}
}
//ImGui描画
void Food::ImGuiDraw() {
}
void Food::CarryStart(Ghost* ghost) {
	m_ghost = ghost;
	m_IsCarried = true;
	m_LockOn = false;
}
//パーティクル
void Food::Particle() {
	XMFLOAT4 s_color = { 1.0f,0.0f,0.0f,1.0f };
	XMFLOAT4 e_color = { 0.0f,0.0f,1.0f,1.0f };
	float s_scale = 3.0f;
	float e_scale = 0.0f;
	if (m_Alive) {
		//ParticleEmitter::GetInstance()->FireEffect(10, m_Position, s_scale, e_scale, s_color, e_color);
	}
}
//当たり判定
bool Food::Collision() {
	float l_Radius = 1.2f;//当たり判定の範囲
	float l_AddHunger = 1.0f;//加算される飢餓ゲージ
	XMFLOAT3 m_PlayerPos = Player::GetInstance()->GetPosition();
	if (Collision::CircleCollision(m_Position.x, m_Position.z, l_Radius, m_PlayerPos.x, m_PlayerPos.z, l_Radius) && m_Alive) {
		m_Alive = false;
		m_LockOn = false;
		HungerGauge::GetInstance()->AddNowHunger(HungerGauge::GetInstance()->GetNowHunger() + l_AddHunger);
		return true;
	} else {
		return false;
	}

	return true;
}
//食料生成
void Food::BirthFood() {
	if (!m_Alive) {
		m_Timer++;
		//描画バグ起きるから先に座標セット
		if (m_Timer == 20) {
			//乱数指定
			mt19937 mt{ std::random_device{}() };
			uniform_int_distribution<int> l_distX(-50, 60);
			uniform_int_distribution<int> l_distY(10, 15);
			uniform_int_distribution<int> l_distZ(-55, 55);
			m_Position = { float(l_distX(mt)), float(l_distY(mt)),float(l_distZ(mt)) };
			uniform_int_distribution<int> l_degree(0, 360);
			m_Degree = float(l_degree(mt));
		}
		//一定時間で生成される
		if (m_Timer == 100) {
			m_LightSet = true;
			m_Alive = true;
			m_Spawn = true;
			m_Timer = 0;
		}
	}
}
//食料削除
void Food::DeleteFood() {
	const int l_LimitTimer = 300;
	if (m_Limit) {
		m_DeleteTimer++;
		if (m_DeleteTimer >= l_LimitTimer) {
			m_Alive = false;
		}
	}
}

bool Food::CarriedGhost() {
	if (!m_IsCarried) { return false; }
	if (!m_ghost) { return false; }
	XMFLOAT3 pos = m_ghost->GetPosition();
	m_Position = { pos.x,3.0f,pos.z };
	m_Object->SetPosition(m_Position);
	m_Object->Update();

	if (m_ghost->GetVanish()) {
		m_Alive = false;
		m_IsCarried = false;
		m_ghost = false;
	}
	return true;
}

bool Food::FallSpawn() {
	if (!m_Spawn) { return false; }
	if (m_Position.y == 0.0f && !m_Jump) {
		m_Jump = true;
	} else {
		m_Position.y -= 0.5f;
		Helper::GetInstance()->Clamp(m_Position.y, 0.0f, 30.0f);
	}
	if (m_Jump) {
		m_SpawnTimer += 1.0f / m_SpawnTimerMax;
		Helper::GetInstance()->Clamp(m_SpawnTimer, 0.0f, 1.0f);
		m_Position.x+= sinf(m_Degree * (PI_180 / XM_PI)) * 0.1f;
		if (m_SpawnTimer > 0.5f) {
			m_Position.y = Ease(InOut,Linear, m_SpawnTimer,5.f,0.f);
		} else {
			m_Position.y = Ease(InOut, Linear, m_SpawnTimer, 0.f, 5.f);
		}
		m_Position.z += cosf(m_Degree * (PI_180 / XM_PI)) * 0.1f;
		if (m_SpawnTimer==1.0f) {
			m_Jump = false;
			m_Spawn = false;
		}
	}

	m_Object->SetPosition(m_Position);
	m_Object->Update();


	return true;
}
