#include "AvatarBoss.h"
#include <any>
#include <random>
#include "Collision.h"
#include "CsvLoader.h"
#include "Helper.h"
#include "Player.h"
#include "Easing.h"
#include "ImageManager.h"
//生成
AvatarBoss::AvatarBoss() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::LASTBOSS);

	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);

	tex.reset(IKETexture::Create(ImageManager::DAMAGEAREA, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	tex->TextureCreate();
	tex->SetPosition({ 0.0f,-500.0f,0.0f });
	tex->SetTiling(2.0f);
}
//初期化
bool AvatarBoss::Initialize() {
	m_Position = { 0.0f,3.0f,0.0f };
	m_Rotation = { 0.0f,180.0f,0.0f };
	m_Scale = { 0.7f,0.7f,0.7f };
	m_Color = { 1.0f,1.0f,1.0f,0.0f };
	//m_Rotation.y = -90.f;

	ActionTimer = 1;

	m_Radius = 2.2f;

	_charaState = STATE_INTER;
	m_CircleScale = 15.0f;
	//CSVロード
	CSVLoad();

	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandomSpeed(10, 20);

	m_AddSpeed = float(l_RandomSpeed(mt)) / 10;
	//アバタータイプによって指定位置が変わる
	if (m_AvatarType == AVATAR_ONE) {
		m_TargetPos = { 0.0f,3.0f,40.0f };
	}
	else if (m_AvatarType == AVATAR_SECOND) {
		m_TargetPos = { 0.0f,3.0f,-40.0f };
	}
	else if (m_AvatarType == AVATAR_THIRD) {
		m_TargetPos = { 45.0f,3.0f,0.0f };
	}
	else {
		m_TargetPos = { -35.0f,3.0f,0.0f };
	}

	m_TexColor = { 1.0f,1.0f,1.0f,0.0f };
	m_TexRot = { 90.0f,0.0f,0.0f };
	m_TexScale = { 0.2f,3.0f,0.6f };
	m_Slow = false;
	return true;
}
//スキップ時の初期化
void AvatarBoss::SkipInitialize() {
	m_Position = { 0.0f,3.0f,30.0f };
	m_Rotation = { 0.0f,180.0f,0.0f };
	m_Scale = { 0.7f,0.7f,0.7f };
	m_Color = { 1.0f,1.0f,1.0f,0.0f };
}
//CSV
void AvatarBoss::CSVLoad() {

	m_Magnification = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Avatar/Avatarboss.csv", "Magnification")));
	m_HP = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Avatar/Avatarboss.csv", "hp1")));
	m_BirthTarget = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Avatar/Avatarboss.csv", "HeartTarget")));

	m_MaxHp = m_HP;
}
void (AvatarBoss::* AvatarBoss::stateTable[])() = {
	&AvatarBoss::InterValMove,//動きの合間
	&AvatarBoss::Polter,//ポルターガイスト
	&AvatarBoss::ThrowBound,//投げる
	&AvatarBoss::BlockAttack,//範囲攻撃
	&AvatarBoss::Vanish,//ボス消える
};
void (AvatarBoss::* AvatarBoss::avatarTable[])() = {
	&AvatarBoss::AvatarNormal,//ふつうの円運動
	&AvatarBoss::AvatarAround,//外周
	&AvatarBoss::AvatarRight,//右上
	&AvatarBoss::AvatarLeft,//左下
};
//行動
void AvatarBoss::Action() {
	//状態移行(charastateに合わせる)
	if (m_HP > 0.0f) {
		(this->*stateTable[_charaState])();
	}
	//リミット制限
	Helper::GetInstance()->Clamp(m_Position.x, -55.0f, 65.0f);
	Helper::GetInstance()->Clamp(m_Position.z, -60.0f, 60.0f);
	/*^^^^当たり判定^^^^*/
	//弾とボスの当たり判定
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
	//プレイヤーの当たり判定
		//プレイヤーの当たり判定
	if (m_Color.w > 0.9f) {
		CollideBul(_playerBulA, Type::CIRCLE);
		ColPlayer();
	}
	//OBJのステータスのセット
	Obj_SetParam();
	//障害物
	for (Poltergeist* newpolter : poltergeist) {
		if (newpolter != nullptr) {
			newpolter->Update();
		}
	}

	//障害物の削除
	for (int i = 0; i < poltergeist.size(); i++) {
		if (poltergeist[i] == nullptr) {
			continue;
		}

		if (!poltergeist[i]->GetAlive()) {
			poltergeist.erase(cbegin(poltergeist) + i);
		}
	}

	//ダメージブロック
	for (DamageBlock* newblock : damageblock) {
		if (newblock != nullptr) {
			newblock->Update();
		}
	}
	//ダメージブロックの削除
	for (int i = 0; i < damageblock.size(); i++) {
		if (damageblock[i] == nullptr) {
			continue;
		}

		if (!damageblock[i]->GetAlive()) {
			damageblock.erase(cbegin(damageblock) + i);
		}
	}
	m_Color.w = Ease(In, Cubic, 0.4f, m_Color.w, 1.0f);
	m_Rotation.y = Helper::GetInstance()->DirRotation(m_Position, Player::GetInstance()->GetPosition(), -PI_180);
}
//ポーズ
void AvatarBoss::Pause() {

}
//エフェクト描画
void AvatarBoss::EffecttexDraw(DirectXCommon* dxCommon)
{
	if (m_HP < 0.0f)return;
	if (m_TexAlive) {
		IKETexture::PreDraw2(dxCommon, AlphaBlendType);
		tex->Draw();
		IKETexture::PostDraw();
	}
}
//描画
void AvatarBoss::Draw(DirectXCommon* dxCommon) {
	Obj_Draw();
	if (m_HP > 0.0f) {
		//障害物
		for (Poltergeist* newpolter : poltergeist) {
			if (newpolter != nullptr) {
				newpolter->Draw(dxCommon);
			}
		}
		//ダメージブロック
		for (DamageBlock* newblock : damageblock) {
			if (newblock != nullptr) {
				newblock->Draw(dxCommon);
			}
		}

		EffecttexDraw(dxCommon);
	}
}
//ImGui
void AvatarBoss::ImGui_Origin() {
}
//インターバル
void AvatarBoss::InterValMove() {
	const float l_AddScale = 0.5f;
	const float l_AddSpeed = 1.5f;
	int l_LimitTimer = 400;
	const int l_LimitAttack = 5;
	(this->*avatarTable[m_AvatarType])();
	m_InterVal++;
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandomMove(0, 2);
	if (m_InterVal == l_LimitTimer) {
		if (m_AttackCount != l_LimitAttack) {
			//行動を決めて次の行動に移る
			m_AttackRand = int(l_RandomMove(mt));

			if (m_AttackRand == 0) {
				m_AttackCount++;
				_charaState = STATE_BOUND;
				m_InterVal = {};
			}
			else if (m_AttackRand == 1) {
				m_AttackCount++;
				_charaState = STATE_POLTER;
				m_InterVal = {};
			}
			else {
				m_AttackCount++;
				_charaState = STATE_BLOCK;
				m_InterVal = {};
			}
		}
		else {
			_charaState = STATE_VANISH;
			m_InterVal = {};
			m_AttackCount = {};
			m_AfterAlpha = 0.5f;
		}
	}
}
//ポルターガイスト
void AvatarBoss::Polter() {
	const int l_LimitTimer = 300;
	m_MoveTimer++;
	if (m_MoveTimer == 1) {
		BirthPolter("Normal");
	}
	if (m_MoveTimer == l_LimitTimer) {
		m_MoveTimer = {};
		_charaState = STATE_INTER;
		m_Return = true;
	}
}
//バウンド弾
void AvatarBoss::ThrowBound() {
	const int l_LimitTimer = 300;
	m_MoveTimer++;
	if (m_MoveTimer == 1) {
		BirthPolter("Bound");
	}
	if (m_MoveTimer == l_LimitTimer) {
		m_MoveTimer = {};
		_charaState = STATE_INTER;
		m_Return = true;
	}
}
//ダメージのブロック
void AvatarBoss::BlockAttack() {
	const int l_LimitTimer = 200;
	m_MoveTimer++;
	if (m_MoveTimer == 1) {
		BirthBlock();
	}
	if (m_MoveTimer == l_LimitTimer) {
		m_MoveTimer = {};
		_charaState = STATE_INTER;
		m_Return = true;
	}
}
//ブロックの生成
void AvatarBoss::BirthBlock() {
	float l_SetPosX = {};
	float l_SetPosZ = {};
	int l_RandDir = {};
	//ランダムで進行方向決める
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandomDir(0, 3);
	uniform_int_distribution<int> l_RandomX(-55, 45);
	uniform_int_distribution<int> l_RandomZ(-60, 40);
	l_SetPosX = float(l_RandomX(mt));
	l_SetPosZ = float(l_RandomZ(mt));
	l_RandDir = int(l_RandomDir(mt));

	for (int i = 0; i < BLOCK_NUM; i++) {
		DamageBlock* newblock;
		newblock = new DamageBlock();
		newblock->Initialize();
		newblock->SetAttackDir(l_RandDir);
		newblock->InitPos(i, { l_SetPosX,0.0f,l_SetPosZ });
		damageblock.push_back(newblock);
	}
}
//ポルターガイストの生成
void AvatarBoss::BirthPolter(const std::string& PolterName) {
	const int l_LimitTimer = 20;//障害物が動くまでの時間
	const int l_LimitTimer2 = 50;//障害物が動くまでの時間2
	if (PolterName == "Normal") {
		for (int i = 0; i < POLTER_NUM; i++) {
			//ノーツの発生
			Poltergeist* newpolter;
			newpolter = new Poltergeist();
			newpolter->Initialize();

			newpolter->SetPolterType(TYPE_FOLLOW);
			newpolter->SetTargetTimer(i * l_LimitTimer);
			if (i == 0) {
				newpolter->SetPosition({ m_Position.x + 4.0f,m_Position.y - 10.0f,m_Position.z });
			}
			else if (i == 1) {
				newpolter->SetPosition({ m_Position.x - 4.0f,m_Position.y - 10.0f,m_Position.z });
			}
			poltergeist.push_back(newpolter);
		}
	}
	else if (PolterName == "Bound") {
		for (int i = 0; i < POLTER_NUM; i++) {
			//ノーツの発生
			Poltergeist* newpolter;
			newpolter = new Poltergeist();
			newpolter->Initialize();

			newpolter->SetPolterType(TYPE_BOUND);
			newpolter->SetTargetTimer(i * l_LimitTimer2);
			newpolter->SetBasePos(m_Position);
			newpolter->SetPosition({ m_Position.x,m_Position.y - 10.0f,m_Position.z });
			newpolter->SetCircleSpeed(i * 180.0f);
			poltergeist.push_back(newpolter);
		}
	}
}
//ボスが消える
void AvatarBoss::Vanish() {
	const int l_LimitTimer = 200;
	const int l_LimitTimer2 = 30;
	const float l_AddFrame = 0.025f;
	if (_VanishType == VANISH_SET) {
		m_MoveTimer++;
		if (m_MoveTimer == 120) {
			m_TexAlive = true;
			m_AfterPos = Player::GetInstance()->GetPosition();
		}
		else if (m_MoveTimer == l_LimitTimer) {
			m_AfterAlpha = {};
			_VanishType = VANISH_ATTACK;
			m_MoveTimer = {};
			m_Frame = {};
		}
		if (m_TexAlive) {
			if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
				m_Frame = 1.0f;
			}
		}
	}
	else if (_VanishType == VANISH_ATTACK) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = {};
			_VanishType = VANISH_EXPLO;
		}

		m_Position = {
			Ease(In,Cubic,m_Frame,m_Position.x,m_AfterPos.x),
			Ease(In,Cubic,m_Frame,m_Position.y,m_AfterPos.y),
			Ease(In,Cubic,m_Frame,m_Position.z,m_AfterPos.z),
		};
	}
	else if (_VanishType == VANISH_EXPLO) {
		BirthExplosion();
		m_MoveTimer++;
		if (m_MoveTimer == l_LimitTimer2) {
			m_HP = 0.0f;
		}
	}

	m_Alpha = Ease(In, Cubic, m_Frame, m_Alpha, m_AfterAlpha);
	m_TexPos = {
	m_TexPos.x = (m_Position.x + m_AfterPos.x) / 2,
	0.0f,
	m_TexPos.z = (m_Position.z + m_AfterPos.z) / 2,
	};
	m_TexRot.y = Helper::GetInstance()->DirRotation(m_Position, m_AfterPos, -PI_180);
	m_TexScale.y = Helper::GetInstance()->ChechLength(m_Position, m_AfterPos) * 0.1f;
	tex->Update();
	tex->SetPosition(m_TexPos);
	tex->SetRotation(m_TexRot);
	tex->SetScale(m_TexScale);
	tex->SetColor({ m_TexColor.x,m_TexColor.y,m_TexColor.z,m_Alpha });
}
//爆発パーティクル
void AvatarBoss::BirthExplosion() {
	float l_AddSize = 2.5f;
	const float RandScale = 3.0f;
	float s_scale = 0.3f * l_AddSize;
	float e_scale = (4.0f + (float)rand() / RAND_MAX * RandScale - RandScale / 2.0f) * l_AddSize;

	//色
	const float RandRed = 0.2f;
	const float red = 0.2f + (float)rand() / RAND_MAX * RandRed;
	const XMFLOAT4 s_color = { 0.9f, red, 0.1f, 1.0f }; //濃い赤
	const XMFLOAT4 e_color = { 0, 0, 0, 1.0f }; //無色

	//乱数指定
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_Randlife(10, 40);
	int l_Life = int(l_Randlife(mt));

	ParticleEmitter::GetInstance()->Explosion(l_Life, m_Position, l_AddSize, s_scale, e_scale, s_color, e_color,0);
}
//登場シーン
void AvatarBoss::AppearAction() {
	Obj_SetParam();
}
//ボス撃破シーン
void AvatarBoss::DeadAction() {

	Obj_SetParam();
}
//ボス撃破シーン(スロー)
void AvatarBoss::DeadAction_Throw() {
	Obj_SetParam();
}
void AvatarBoss::AvatarNormal() {
	m_CircleSpeed += m_AddSpeed;
	m_CircleScale = 20.0f;

	m_AfterPos = Helper::GetInstance()->CircleMove(m_TargetPos, m_CircleScale, m_CircleSpeed);
	m_Position = {
		Ease(In,Cubic,0.5f,m_Position.x,m_AfterPos.x),
		m_Position.y,
		Ease(In,Cubic,0.5f,m_Position.z,m_AfterPos.z),
	};
}
void AvatarBoss::AvatarAround() {
	m_CircleSpeed += m_AddSpeed;
	m_CircleScale = 20.0f;

	m_AfterPos = Helper::GetInstance()->CircleMove(m_TargetPos, m_CircleScale, m_CircleSpeed);
	m_Position = {
		Ease(In,Cubic,0.5f,m_Position.x,m_AfterPos.x),
		m_Position.y,
		Ease(In,Cubic,0.5f,m_Position.z,m_AfterPos.z),
	};
}
void AvatarBoss::AvatarRight() {
	m_CircleSpeed += m_AddSpeed;
	m_CircleScale = 20.0f;

	m_AfterPos = Helper::GetInstance()->CircleMove(m_TargetPos, m_CircleScale, m_CircleSpeed);
	m_Position = {
		Ease(In,Cubic,0.5f,m_Position.x,m_AfterPos.x),
		m_Position.y,
		Ease(In,Cubic,0.5f,m_Position.z,m_AfterPos.z),
	};
}
void AvatarBoss::AvatarLeft() {
	m_CircleSpeed += m_AddSpeed;
	m_CircleScale = 20.0f;

	m_AfterPos = Helper::GetInstance()->CircleMove(m_TargetPos, m_CircleScale, m_CircleSpeed);
	m_Position = {
		Ease(In,Cubic,0.5f,m_Position.x,m_AfterPos.x),
		m_Position.y,
		Ease(In,Cubic,0.5f,m_Position.z,m_AfterPos.z),
	};
}
void AvatarBoss::InitAwake() {

}
void AvatarBoss::EndRollAction() {
	//OBJのステータスのセット
	Obj_SetParam();
}
//会話
void AvatarBoss::AfterAction() {

}