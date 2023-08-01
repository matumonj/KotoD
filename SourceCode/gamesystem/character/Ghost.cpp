#include "Ghost.h"
#include "Collision.h"
#include "Easing.h"
#include <random>
#include "Player.h"
#include "HungerGauge.h"
#include "Helper.h"
Ghost::Ghost() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Ghost);
	model_follow.reset(ModelManager::GetInstance()->GetModel(ModelManager::Buddy));
	m_Object = make_unique<IKEObject3d>();
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}
//初期化
bool Ghost::Initialize() {
	//乱数指定
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_distX(-50, 60);
	uniform_int_distribution<int> l_distZ(-55, 55);
	m_Position = { float(l_distX(mt)),0.0f,float(l_distZ(mt)) };
	m_Rotation.y = -PI_90;
	m_Scale = { 0.5f,0.5f,0.5f };
	m_Color = { 1.0f,1.0f,1.0f,0.7f };
	uniform_int_distribution<int> spawn(60, 90);
	kSpawnTimerMax = float(spawn(mt));
	_charaState = CharaState::STATE_SPAWN;
	_searchState = SearchState::SEARCH_NO;
	_followState = FollowState::Follow_NO;
	return true;
}

//状態遷移
/*CharaStateのState並び順に合わせる*/
void (Ghost::* Ghost::stateTable[])() = {
	&Ghost::None,//待機
	&Ghost::Spawm,
	&Ghost::Search,//
	&Ghost::Follow,//移動
	&Ghost::DarkSide,
	&Ghost::Jack,
	&Ghost::HyperJack,
	&Ghost::Manipulate,//操る
	&Ghost::Vanish,
};
//更新
void Ghost::Update() {
	float l_AddScale = 2.0f;//OBB用に少し大きめのスケールを取る
	m_OldPos = m_Position;
	m_OBBScale = { m_Scale.x + l_AddScale,m_Scale.y + l_AddScale, m_Scale.z + l_AddScale };
	if (m_IsPostionCheck) { _charaState = CharaState::STATE_JACK; }
	if (m_IsAllPostionCheck) { _charaState = CharaState::STATE_HYPERJACK; }
	//状態移行(charastateに合わせる)
	(this->*stateTable[_charaState])();
	//タイプによって色を一旦変えてる
	Obj_SetParam();
	//食料生成
	BirthGhost();
	//当たり判定(プレイヤー)
	PlayerCollision();
	//食べ物をはこぶ
	CarryFood();
	Particle();
	//消える
	if (m_Vanish) {
		_charaState = STATE_VANISH;
	}
	//当たり判定（弾）
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_ghost();
	CollideBullet(_playerBulA);

	//if(_charaState == CharaState::STATE_NONE)
	//{
	if (Collide)
		_charaState = CharaState::STATE_DARKOTI;

	//攻撃の音符
	for (AttackNote* newnote : attacknotes) {
		if (newnote != nullptr) {
			newnote->Update();
		}
	}

	//攻撃の音符の削除
	for (int i = 0; i < attacknotes.size(); i++) {
		if (attacknotes[i] == nullptr) {
			continue;
		}

		if (!attacknotes[i]->GetAlive()) {
			attacknotes.erase(cbegin(attacknotes) + i);
		}
	}
}
//描画
void Ghost::Draw(DirectXCommon* dxCommon) {
	if (m_Alive) {
		Obj_Draw();
	}

	//攻撃の音符
	for (AttackNote* newnote : attacknotes) {
		if (newnote != nullptr) {
			newnote->Draw(dxCommon);
		}
	}
}
//ImGui描画
void Ghost::ImGuiDraw() {
	ImGui::Begin("Ghost");
	ImGui::Text("Abso:%d", m_Absorption);
	ImGui::Text("PosZ:%f", m_Position.z);
	ImGui::End();
}

//パーティクル
void Ghost::Particle() {
	XMFLOAT4 s_color = { 1.0f,1.0f,1.0f,1.0f };
	XMFLOAT4 e_color = { 1.0f,1.0f,1.0f,1.0f };
	XMFLOAT4 s_color2 = { 1.0f,0.0f,0.0f,1.0f };
	XMFLOAT4 e_color2 = { 1.0f,0.0f,0.0f,1.0f };
	XMFLOAT4 s_color3 = { 0.0f,1.0f,0.0f,1.0f };
	XMFLOAT4 e_color3 = { 0.0f,1.0f,0.0f,1.0f };
	float s_scale = 3.0f;
	float e_scale = 0.0f;
	if (m_Alive) {
		if (m_IsRefer && _charaState < CharaState::STATE_JACK) {
			flash += flashVel * flashAdd;
			flash = clamp(flash, 0.0f, 1.0f);
			if (flash == 1.0f) {
				flashAdd = sub;
			}
			if (flash == 0.0f) {
				flashAdd = add;
			}
			m_Color = { 1.0f,flash,1.0f,1.0f };
		} else if (_charaState == CharaState::STATE_NONE) {
			m_Color = { 1.0f,1.0f,1.0f,0.7f };
			m_Scale = { 0.6f,0.6f,0.6f };
		} else if (_charaState == CharaState::STATE_FOLLOW) {
			m_Color = { 1.0f,1.0f,1.0f,1.0f };
			m_Scale = { 0.6f,0.6f,0.6f };
			m_IsRefer = false;
			ParticleEmitter::GetInstance()->FireEffect(20, m_Position, s_scale, e_scale, s_color2, e_color2);
		} else if (_charaState == CharaState::STATE_SEARCH) {
			m_Color = { 1.0f,1.0f,1.0f,1.0f };
			m_Scale = { 0.6f,0.6f,0.6f };
			m_IsRefer = false;
			ParticleEmitter::GetInstance()->FireEffect(20, m_Position, s_scale, e_scale, s_color2, e_color2);
		} else if (_charaState == CharaState::STATE_JACK) {
			m_Color = { 1.0f,0.0f,1.0f,1.0f };
			m_Scale = { 0.6f,0.6f,0.6f };
			//ParticleEmitter::GetInstance()->FireEffect(20, m_Position, s_scale, e_scale, s_color2, e_color2);
		} else if (_charaState == CharaState::STATE_HYPERJACK) {
			m_Color = { 1.0f,1.0f,0.0f,1.0f };
			m_Scale = { 0.6f,0.6f,0.6f };
			//ParticleEmitter::GetInstance()->FireEffect(20, m_Position, s_scale, e_scale, s_color2, e_color2);
		}
	}
}
//当たり判定(プレイヤー)
bool Ghost::PlayerCollision() {
	if (Player::GetInstance()->PlayerCollide(m_Position) && (_charaState == CharaState::STATE_FOLLOW)) {
		m_Position = m_OldPos;
		return true;
	} else {
		return false;
	}

	return true;
}
//当たり判定(ゴースト同士)
void Ghost::GhostCollision(const XMFLOAT3& pos) {
	m_Position.x += sin(atan2f((m_Position.x - pos.x), (m_Position.z - pos.z))) * 0.3f;
	m_Position.z += cos(atan2f((m_Position.x - pos.x), (m_Position.z - pos.z))) * 0.3f;
}
//ゴースト生成
void Ghost::BirthGhost() {
	if (m_Alive) { return; }
	if (!VerseCheck()) { return; }
	m_ResPornTimer++;
	//描画バグ起きるから先に座標セット
	if (m_ResPornTimer == 20) {
		_charaState = CharaState::STATE_SPAWN;
		_searchState = SearchState::SEARCH_NO;
		//乱数指定
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_distX(-50, 60);
		uniform_int_distribution<int> l_distZ(-55, 55);
		m_Object->SetModel(m_Model);
		m_Position = { float(l_distX(mt)),0.0f,float(l_distZ(mt)) };
		uniform_int_distribution<int> spawn(30, 45);
		kSpawnTimerMax = float(spawn(mt));
		m_Color = { 1.0f,1.0f,1.0f,0.7f };
		m_IsRefer = false;
		m_IsHyperRefer = false;
		RottoPlayer = 0.0f;
		m_Vanish = false;
		m_Catch = false;
		m_Search = false;
		m_Follow = false;
		m_DFollow = false;
		m_Absorption = false;
		m_SearchTimer = 0;
		m_SpawnTimer = 0;
	}
	//一定時間で生成される
	if (m_ResPornTimer == 100) {
		m_Alive = true;
		m_ResPornTimer = 0;
		m_Scale = { 0.5f,0.5f,0.5f };
	}
}
bool Ghost::VerseCheck() {
	if (isVerse) { return true; }
	m_VerseCureTimer--;
	m_VerseCureTimer = clamp(m_VerseCureTimer, 0, 1000);
	if (m_VerseCureTimer <= 0) {
		isVerse = true;
		return true;
	} else {
		return false;
	}
}
//何もない状態
void Ghost::None() {
	//浮遊状態
	m_BirthPos = m_Position;
	DarkOtiClean = false;
	float scales = 0.5f;
	m_Scale = { scales,scales,scales };
	noneTimer += 0.05f;
	float size = sinf(noneTimer) * 0.05f;
	if (!m_Absorption) {
		m_Position.x += cosf(m_Rotation.y * (PI_180 / XM_PI)) * size;
		m_Position.y = sinf(noneTimer) * 1.2f;
		m_Position.z += sinf(m_Rotation.y * (PI_180 / XM_PI)) * size;
	} else {
		Absorption();
	}
}
//生まれる状態
void Ghost::Spawm() {
	m_SpawnTimer += 1.0f / kSpawnTimerMax;

	m_Rotation.y = Ease(In, Quad, m_SpawnTimer, -(PI_360 + PI_90), -PI_90);
	m_DarkC = 0;

	float scale = Ease(Out, Elastic, m_SpawnTimer, 0.0f, 0.5f);
	m_Scale = { scale,scale,scale };

	Helper::GetInstance()->Clamp(m_SpawnTimer, 0.0f, 1.0f);
	if (m_SpawnTimer == 1.0f) {
		_charaState = CharaState::STATE_NONE;
	}
}
//追従
void Ghost::Follow() {
	float l_Vel = 0.35f;//速度
	XMFLOAT3 l_playerPos = Player::GetInstance()->GetPosition();
	//操られているか
	if (!m_Manipulate) {
		Helper::GetInstance()->FollowMove(m_Position, l_playerPos, l_Vel);
		m_Rotation.y = Helper::GetInstance()->DirRotation(m_Position, l_playerPos, -PI_90);
	} else {
		_ManiState = MANI_SET;
		_charaState = STATE_MANIPULATE;
	}
}
//操り攻撃
void Ghost::Manipulate() {
	const float l_AddFrame = 0.01f;
	const float l_AddRot = 10.0f;
	const int l_Limit = 300;
	m_Color = { Ease(In,Cubic,0.3f,m_Color.x,1.0f),
	Ease(In,Cubic,0.3f,m_Color.y,0.0f),
	Ease(In,Cubic,0.3f,m_Color.z,1.0f),
	Ease(In,Cubic,0.3f,m_Color.w,0.7f),
	};
	//移動先を決める
	if (_ManiState == MANI_SET) {
		//乱数指定
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_distX(-50, 60);
		uniform_int_distribution<int> l_distZ(-55, 55);
		m_AfterPos = { float(l_distX(mt)),0.0f,float(l_distZ(mt)) };
		m_AfterRotY = Helper::GetInstance()->DirRotation(m_Position, m_AfterPos, -PI_90);
		_ManiState = MANI_MOVE;
	}
	//移動する
	else if (_ManiState == MANI_MOVE) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		} else {
			m_Frame = {};
			m_Rotation.y = -90.0f;
			_ManiState = MANI_ATTACK;
		}

		m_Position = { Ease(In,Cubic,m_Frame,m_Position.x,m_AfterPos.x),
		Ease(In,Cubic,m_Frame,m_Position.y,m_AfterPos.y),
		Ease(In,Cubic,m_Frame,m_Position.z,m_AfterPos.z),
		};

		m_Rotation.y = Ease(In, Cubic, m_Frame, m_Rotation.y, m_AfterRotY);
	} else if (_ManiState == MANI_ATTACK) {
		m_Rotation.y += l_AddRot;
		m_RotTimer++;

		if (m_RotTimer % 10 == 0) {
			BirthBullet();
		}

		if (m_RotTimer == l_Limit) {
			m_RotTimer = {};
			_ManiState = MANI_END;
		}
	} else {
		m_Manipulate = false;
		m_Vanish = true;
	}
}

void Ghost::DarkSide() {
	if (Collide)m_DFollow = true;
	//m_Color.x -= 0.05f;
	m_Color.y -= 0.05f;
	//m_Color.z -= 0.05f;
	float l_Vel = 0.5f;//速度
	if (Collision::GetLength(m_Position, bossPos) > 5) {
		Helper::GetInstance()->FollowMove(m_Position, bossPos, l_Vel);
		m_Rotation.y = Helper::GetInstance()->DirRotation(m_Position, bossPos, -PI_90);
	}

	if (DarkOtiClean) {
		m_DarkC = 0;
		m_DFollow = false;
		Collide = false;
		DarkOtiClean = false;

		Collide = false;
		stateSpawn = true;
		m_Vanish = true;
	}


	Helper::GetInstance()->Clamp(m_Scale.x, 0.f, 5.f);

	Helper::GetInstance()->Clamp(m_Scale.y, 0.f, 5.f);
	Helper::GetInstance()->Clamp(m_Scale.z, 0.f, 5.f);
}

//探索
void Ghost::Search() {
	const int l_LimitTimer = 170;
	const float l_Vel = 0.8f;
	const float l_Vel2 = 0.8f;
	XMFLOAT3 l_playerPos = Player::GetInstance()->GetPosition();
	//サーチ状態から一定時間立つと存在消去
	m_SearchTimer++;
	if (m_SearchTimer >= l_LimitTimer) {
		m_Vanish = true;
	}
	//追従
	if (_searchState == SearchState::SEARCH_START) {
		Helper::GetInstance()->FollowMove(m_Position, m_SearchPos, l_Vel2);
		m_Rotation.y = Helper::GetInstance()->DirRotation(m_Position, m_SearchPos, -PI_90);
	} else if (_searchState == SearchState::SEARCH_END) {
		Helper::GetInstance()->FollowMove(m_Position, l_playerPos, l_Vel);
		m_Rotation.y = Helper::GetInstance()->DirRotation(m_Position, l_playerPos, -PI_90);
	}
}
void Ghost::Jack() {
	if (m_IsPostionCheck) {
		f_pos = m_Position;
		m_angle = 0;
		m_radius = 0;
		mt19937 mt{ std::random_device{}() };
		// 0以上9以下の値を等確率で発生させる
		std::uniform_int_distribution<> dist(0, 1);
		if (dist(mt) > 0) {
			m_dir = addDir;
		} else {
			m_dir = subDir;
		}
		m_IsPostionCheck = false;
	}
	const int l_LimitTimer = 800;
	m_SearchTimer++;
	if (m_SearchTimer >= l_LimitTimer) {
		m_Vanish = true;
	}
	m_angle += 0.04f;
	m_radius += 0.08f * m_dir;
	XMFLOAT3 e_pos = { f_pos.x + sinf(m_angle) * m_radius ,0,f_pos.z + cosf(m_angle) * m_radius };
	m_Rotation.y = Helper::GetInstance()->DirRotation(m_Position, e_pos, -PI_90);
	m_Position.x = e_pos.x;
	m_Position.z = e_pos.z;
	if (Player::GetInstance()->PlayerCollide(m_Position) &&
		Player::GetInstance()->GetDamageInterVal() == 0) {
		Player::GetInstance()->PlayerHit(m_Position);
		Player::GetInstance()->RecvDamage(0.75f);
	}

	if (m_Position.x <= -55.0f || m_Position.x >= 65.0f ||
		m_Position.z <= -60.0f || m_Position.z >= 60.0f) {
		m_Alive = false;
		m_Scale = { 0.0f,0.0f,0.0f };
		m_IsHyperRefer = false;
		m_IsRefer = false;
		isVerse = true;
		_charaState = CharaState::STATE_NONE;
	}
}

void Ghost::HyperJack() {
	if (m_IsAllPostionCheck) {
		GetRotation2Player();
		f_pos = m_Position;
		m_radius = 0;
		m_dir = subDir;
		m_IsAllPostionCheck = false;
	}
	m_radius += 0.8f * m_dir;
	XMFLOAT3 e_pos = { f_pos.x + sinf(RottoPlayer) * m_radius ,0,f_pos.z + cosf(RottoPlayer) * m_radius };
	m_Position.x = e_pos.x;
	m_Position.z = e_pos.z;
	if (Player::GetInstance()->PlayerCollide(m_Position) &&
		Player::GetInstance()->GetDamageInterVal() == 0) {
		Player::GetInstance()->PlayerHit(m_Position);
		Player::GetInstance()->RecvDamage(1.5f);
	}
	if (m_Position.x <= -55.0f || m_Position.x >= 65.0f ||
		m_Position.z <= -60.0f || m_Position.z >= 60.0f) {
		m_Alive = false;
		m_IsHyperRefer = false;
		m_IsRefer = false;
		m_Scale = { 0.0f,0.0f,0.0f };
		isVerse = false;
		m_VerseCureTimer = 120;
		_charaState = CharaState::STATE_NONE;
	}
}

//ゴーストが消える
void Ghost::Vanish() {
	const float l_AddFrame = 0.01f;
	const float l_AfterScale = 0.0f;
	if (m_Vanish) {
		m_Rotation.y += 5.0f;
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		} else {
			m_Vanish = false;
			m_IsRefer = false;
			m_IsHyperRefer = false;
			m_Alive = false;
			m_Frame = {};
		}

		m_Scale = { Ease(In,Cubic,m_Frame,m_Scale.x,l_AfterScale),
		Ease(In,Cubic,m_Frame,m_Scale.y,l_AfterScale),
		Ease(In,Cubic,m_Frame,m_Scale.z,l_AfterScale), };
	}
}
//探索スタート
void Ghost::StartSearch(const XMFLOAT3& pos) {
	_searchState = SearchState::SEARCH_START;
	m_Search = true;
	m_SearchPos = pos;
}
//探索終了
void Ghost::EndSearch() {
	_searchState = SearchState::SEARCH_END;
}
//食べ物を運ぶ
void Ghost::CarryFood() {
	float l_Radius = 1.0f;//当たり判定
	float l_AddHunger = HungerGauge::m_Hungervalue;//加算される気がゲージ
	float l_AddExtra = 2.5f;
	XMFLOAT3 l_playerPos = Player::GetInstance()->GetPosition();
	if ((_searchState == SearchState::SEARCH_END) && (!m_Vanish) && (m_Catch) && (m_Search)) {
		if (Collision::CircleCollision(m_Position.x, m_Position.z, l_Radius, l_playerPos.x, l_playerPos.z, l_Radius)) {
			m_Vanish = true;
			m_Search = false;
			m_Catch = false;
			m_Limit = {};
			HungerGauge::GetInstance()->RecoveryNowHunger(0.0f);
			HungerGauge::GetInstance()->SetAdditional(HungerGauge::GetInstance()->GetAdditional() + l_AddExtra);
			Audio::GetInstance()->PlayWave("Resources/Sound/SE/Get_Food.wav", VolumManager::GetInstance()->GetSEVolum());
		}
	}
}
//当たり判定
bool Ghost::CollideBullet(vector<InterBullet*>bullet) {
	float l_AddHungerMax = HungerGauge::m_Hungervalue;//加算される最大飢餓ゲージ
	m_OBB1.SetParam_Pos(m_Position);
	m_OBB1.SetParam_Rot(m_Object->GetMatrot());
	m_OBB1.SetParam_Scl(m_OBBScale);
	//弾の更新
	for (InterBullet* _bullet : bullet) {
		if (_bullet != nullptr && _bullet->GetAlive()) {
			m_OBB2.SetParam_Pos(_bullet->GetPosition());
			m_OBB2.SetParam_Rot(_bullet->GetMatRot());
			m_OBB2.SetParam_Scl({ 2.0f,2.0f,_bullet->GetScale().z + 3.0f });

			if ((Collision::OBBCollision(m_OBB1, m_OBB2)) && (_bullet->GetAlive()) && (!m_Catch) && (m_Alive)) {
				if (_charaState > STATE_SPAWN) { return false; }
				if (m_IsRefer) { return false; }
				m_Catch = true;
				if (_bullet->GetBulletType() == BULLET_FORROW) {
					Audio::GetInstance()->PlayWave("Resources/Sound/SE/Get_Follower.wav", VolumManager::GetInstance()->GetSEVolum() / 2.5f);
					HungerGauge::GetInstance()->SetHungerMax(HungerGauge::GetInstance()->GetHungerMax() + l_AddHungerMax);
					HungerGauge::GetInstance()->SetNowHunger(HungerGauge::GetInstance()->GetNowHunger() + l_AddHungerMax);
					HungerGauge::GetInstance()->SetCatchCount(HungerGauge::GetInstance()->GetCatchCount() + 1);
					_charaState = CharaState::STATE_FOLLOW;
					_followState = FollowState::Follow_START;
					m_Object->SetModel(model_follow.get());
					m_Follow = true;
				} else {
					_charaState = CharaState::STATE_SEARCH;
					Audio::GetInstance()->PlayWave("Resources/Sound/SE/Get_Searcher.wav", VolumManager::GetInstance()->GetSEVolum() / 2.5f);
				}
				return true;
			} else {
				return false;
			}
		}
	}

	return false;
}

bool Ghost::CollideDarkBul(OBB obb) {
	m_OBB1.SetParam_Pos(m_Position);
	m_OBB1.SetParam_Rot(m_Object->GetMatrot());
	m_OBB1.SetParam_Scl(m_OBBScale);
	//弾の更新
	if ((Collision::OBBCollision(m_OBB1, obb))) {
		return true;
	} else {
		return false;
	}

}

void Ghost::GetRotation2Player() {
	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();

	//角度の取得 プレイヤーが敵の索敵位置に入ったら向きをプレイヤーの方に
	XMVECTOR PositionA = { l_player.x,l_player.y,l_player.z };
	XMVECTOR PositionB = { m_Position.x,m_Position.y,m_Position.z };
	//プレイヤーと敵のベクトルの長さ(差)を求める
	XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;

	RottoPlayer = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);
	m_Rotation.y = RottoPlayer * 60.0f + (PI_90 + PI_180);
}
//吸収
void Ghost::Absorption() {
	float l_Vel = 0.35f;//速度
	Helper::GetInstance()->FollowMove(m_Position, m_TargetPos, l_Vel);
	m_Rotation.y = Helper::GetInstance()->DirRotation(m_Position, m_TargetPos, -PI_90);
}
//攻撃
void Ghost::BirthBullet() {
	XMVECTOR move = { 0.0f, 0.0f, 0.1f, 0.0f };
	XMMATRIX matRot = {};
	matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
	move = XMVector3TransformNormal(move, matRot);
	XMFLOAT2 l_Angle;
	l_Angle.x = move.m128_f32[0];
	l_Angle.y = move.m128_f32[2];
	//ノーツの発生
	AttackNote* newnote;
	newnote = new AttackNote();
	newnote->SetChange(true);
	newnote->Initialize();
	newnote->SetPosition(m_Position);
	newnote->SetAngle(l_Angle);
	attacknotes.push_back(newnote);
}
void Ghost::DeleteBullet() {
	attacknotes.clear();
}