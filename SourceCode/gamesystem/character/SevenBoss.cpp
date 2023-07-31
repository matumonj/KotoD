#include "SevenBoss.h"
#include <any>
#include <random>
#include "Collision.h"
#include "CsvLoader.h"
#include "Helper.h"
#include "Player.h"
#include "Easing.h"
#include "HungerGauge.h"
//生成
SevenBoss::SevenBoss() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::LASTBOSS);

	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);

	bossstuneffect.reset(new BossStunEffect());
	bossstuneffect->Initialize();

	confueffect.reset(new ConfuEffect());
	confueffect->Initialize();
}
//初期化
bool SevenBoss::Initialize() {
	m_Position = { 0.0f,5.0f,30.0f };
	m_Rotation = { 0.0f,180.0f,0.0f };
	m_Scale = { 1.5f,1.5f,1.5f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	ActionTimer = 1;
	m_InterVal = 100;
	m_Radius = 3.0f;
	m_AfterAlpha = 1.0f;
	_charaState = STATE_INTER;
	_ReturnState = RETURN_SET;
	m_CircleSpeed = 90.0f;
	m_BullBoundPower = { 0.0f,0.0f };
	//CSVロード
	CSVLoad();
	effects.clear();
	return true;
}
//スキップ時の初期化
void SevenBoss::SkipInitialize() {
	m_Position = { 0.0f,5.0f,30.0f };
	m_Rotation = { 0.0f,180.0f,0.0f };
	m_Scale = { 1.5f,1.5f,1.5f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
}
//CSV
void SevenBoss::CSVLoad() {
	auto ActSize = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "ACT_NUM")));
	m_RandAct.resize(ActSize);
	LoadCSV::LoadCsvParam_Int("Resources/csv/chara/boss/Seven/Sevenboss.csv", m_RandAct, "RandAct");

	auto LimitSize = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "LIMIT_NUM")));

	m_Limit.resize(LimitSize);
	LoadCSV::LoadCsvParam_Int("Resources/csv/chara/boss/Seven/Sevenboss.csv", m_Limit, "Interval");

	auto StrongLimitSize = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "STRONG_LIMIT_NUM")));

	m_StrongLimit.resize(StrongLimitSize);
	LoadCSV::LoadCsvParam_Int("Resources/csv/chara/boss/Seven/Sevenboss.csv", m_StrongLimit, "StrongInterval");


	m_Magnification = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "Magnification")));
	m_HP = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "hp1")));
	m_BirthTarget = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "HeartTarget")));
	m_VanishTarget = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "VanishTarget")));
	m_AddSpeed = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "BossSpeed")));
	m_MaxHp = m_HP;
	m_CircleScale = 30.0f;
}
void (SevenBoss::* SevenBoss::stateTable[])() = {
	&SevenBoss::InterValMove,//動きの合間
	&SevenBoss::Polter,//ポルターガイスト
	&SevenBoss::ThrowBound,//投げる
	&SevenBoss::BirthAvatar,//偽物のボス
	&SevenBoss::Manipulate,//ゴーストを操る
	&SevenBoss::FireAttack,//炎を出す
	&SevenBoss::Confu,//混乱
	&SevenBoss::BlockAttack,//ダメージの攻撃
	&SevenBoss::BulletCatch,//弾を吸収
	&SevenBoss::Stun,//スタン
};
//行動
void SevenBoss::Action() {
	//状態移行(charastateに合わせる)
	if (m_HP > 0.0f) {
		(this->*stateTable[_charaState])();
	}
	/*^^^^当たり判定^^^^*/
	//弾とボスの当たり判定
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
	if (_charaState != STATE_CATCH) {
		if (m_Color.w > 0.9f) {
			CollideBul(_playerBulA, Type::CIRCLE);
		}
		if (_charaState != STATE_STUN) {
			VanishCollide(_playerBulA);
		}
	}

	//プレイヤーの当たり判定
	if (m_Color.w > 0.9f) {
		ColPlayer();
	}
	//OBJのステータスのセット
	Obj_SetParam();
	//ボスの消える判定
	VanishBoss();
	//リミット制限
	Helper::GetInstance()->Clamp(m_Position.x, -55.0f, 65.0f);
	Helper::GetInstance()->Clamp(m_Position.z, -60.0f, 60.0f);
	Helper::GetInstance()->Clamp(m_HP, -1.0f, m_MaxHp);

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

	//偽物のボス
	for (InterBoss* newboss : avatarboss) {
		if (newboss != nullptr) {
			newboss->Update();
		}
	}

	//偽物のボスの削除
	for (int i = 0; i < avatarboss.size(); i++) {
		if (avatarboss[i] == nullptr) {
			continue;
		}

		if (avatarboss[i]->GetHP() < 1.0f) {
			avatarboss.erase(cbegin(avatarboss) + i);
			m_AvatarCount--;
		}
	}

	//吸収エフェクト
	for (AbsorptionEffect* neweffect : abseffect) {
		if (neweffect != nullptr) {
			neweffect->Update();
		}
	}

	//吸収エフェクト
	for (int i = 0; i < abseffect.size(); i++) {
		if (abseffect[i] == nullptr) {
			continue;
		}

		if (!abseffect[i]->GetAlive()) {
			abseffect.erase(cbegin(abseffect) + i);
		}
	}

	//火の玉
	for (FireBoll* newfire : fireboll) {
		if (newfire != nullptr) {
			newfire->Update();
		}
	}

	//火の玉の削除
	for (int i = 0; i < fireboll.size(); i++) {
		if (fireboll[i] == nullptr) {
			continue;
		}

		if (!fireboll[i]->GetAlive()) {
			fireboll.erase(cbegin(fireboll) + i);
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

	bossstuneffect->SetBasePos({ m_Position.x,m_Position.y + 2.0f,m_Position.z });
	bossstuneffect->Update();

	confueffect->SetBasePos(Player::GetInstance()->GetPosition());
	confueffect->Update();

	//HPが半分切ったら強化
	if (m_HP < m_MaxHp / 2) {
		isStrong = true;
		AVATAR_NUM = 4;
	}

	if (!m_Stun) {
		m_Rotation.y = Helper::GetInstance()->DirRotation(m_Position, Player::GetInstance()->GetPosition(), -PI_180);
	}

}
//ポーズ
void SevenBoss::Pause() {
}
//エフェクト描画
void SevenBoss::EffecttexDraw(DirectXCommon* dxCommon)
{
	if (m_HP <= 0.0f)return;
	confueffect->Draw(dxCommon);
	if (_charaState == STATE_STUN) {
		bossstuneffect->Draw(dxCommon);
	}

	//吸収エフェクト
	for (AbsorptionEffect* neweffect : abseffect) {
		if (neweffect != nullptr) {
			neweffect->Draw(dxCommon);
		}
	}
}
//描画
void SevenBoss::Draw(DirectXCommon* dxCommon) {
	Obj_Draw();
	if (m_HP > 0.0f) {
		//障害物
		for (Poltergeist* newpolter : poltergeist) {
			if (newpolter != nullptr) {
				newpolter->Draw(dxCommon);
			}
		}

		//偽物のボス
		for (InterBoss* newboss : avatarboss) {
			if (newboss != nullptr) {
				newboss->Draw(dxCommon);
			}
		}
		//火の玉
		for (FireBoll* newfire : fireboll) {
			if (newfire != nullptr) {
				newfire->Draw(dxCommon);
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
void SevenBoss::ImGui_Origin() {
}
//インターバル
void SevenBoss::InterValMove() {
	int l_LimitTimer = {};
	//強化しているかどうかで時間が変わる
	if (!isStrong) {
		l_LimitTimer = m_Limit[STATE_INTER];
	}
	else {
		l_LimitTimer = m_StrongLimit[STATE_INTER];
	}
	m_InterVal++;

	//一定時間立った後にボスに近づくと消える
	if (m_InterVal >= 100) {
		m_Dir = Helper::GetInstance()->ChechLength(m_Position, Player::GetInstance()->GetPosition());
		if (m_Dir <= 18.0f) {
			m_Return = true;
		}
	}
	if (!m_Return) {
		RandMove();//一定フレームで動くで
	}
	else {
		ReturnBoss();//ボスが定位置に戻る
	}
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandomMove(0, 100);
	if (m_InterVal == l_LimitTimer) {
		if (m_AvatarCount != 0) {
			//行動を決めて次の行動に移る
			m_AttackRand = int(l_RandomMove(mt));

			if (m_AttackRand < m_RandAct[RAND_POLTER]) {
				_charaState = STATE_POLTER;
				m_InterVal = {};
			}
			else if (m_AttackRand >= m_RandAct[RAND_POLTER] && m_AttackRand < m_RandAct[RAND_BOUND]) {
				_charaState = STATE_BOUND;
				m_InterVal = {};
			}
			else if (m_AttackRand >= m_RandAct[RAND_BOUND] && m_AttackRand < m_RandAct[RAND_MANIPULATE]) {
				if (HungerGauge::GetInstance()->GetCatchCount() != 0) {
					_charaState = STATE_MANIPULATE;
					m_InterVal = {};
					m_StartMani = true;
				}
				else {
					m_InterVal = l_LimitTimer - 1;
				}
			}
			else if (m_AttackRand >= m_RandAct[RAND_MANIPULATE] && m_AttackRand < m_RandAct[RAND_FIRE]) {
				_charaState = STATE_FIRE;
				m_InterVal = {};
			}
			else if (m_AttackRand >= m_RandAct[RAND_FIRE] && m_AttackRand < m_RandAct[RAND_CONFU]) {
				_charaState = STATE_CONFU;
				m_InterVal = {};
			}
			else {
				_charaState = STATE_BLOCK;
				m_InterVal = {};
			}
		}
		else {
			_charaState = STATE_AVATAR;
			m_InterVal = {};
		}
		m_ChangeTimer = {};
	}
}
//ポルターガイスト
void SevenBoss::Polter() {
	int l_LimitTimer = {};
	//強化によって変わる
	if (!isStrong) {
		l_LimitTimer = m_Limit[STATE_POLTER];
	}
	else {
		l_LimitTimer = m_StrongLimit[STATE_POLTER];
	}
	m_MoveTimer++;
	if (m_MoveTimer == 1) {
		BirthPolter("Normal");
	}
	if (m_MoveTimer == l_LimitTimer) {
		m_MoveTimer = {};
		m_AttackCount++;
		//二回攻撃したら吸収行動に移行する
		if (m_AttackCount != 4) {
			_charaState = STATE_INTER;
			m_Return = true;
		}
		else {
			_charaState = STATE_CATCH;
		}
	}

	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);
}
//バウンド弾
void SevenBoss::ThrowBound() {
	int l_LimitTimer = {};
	//強化によって変わる
	if (!isStrong) {
		l_LimitTimer = m_Limit[STATE_BOUND];
	}
	else {
		l_LimitTimer = m_StrongLimit[STATE_BOUND];
	}
	m_MoveTimer++;
	if (m_MoveTimer == 1) {
		BirthPolter("Bound");
	}
	if (m_MoveTimer == l_LimitTimer) {
		m_MoveTimer = {};
		m_AttackCount++;

		//二回攻撃したら吸収行動に移行する
		if (m_AttackCount != 4) {
			_charaState = STATE_INTER;
			m_Return = true;
		}
		else {
			_charaState = STATE_CATCH;
		}
	}
	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);
}
//偽物のボスを生む
void SevenBoss::BirthAvatar() {
	int l_LimitTimer = {};
	//強化によって変わる
	if (!isStrong) {
		l_LimitTimer = m_Limit[STATE_AVATAR];
	}
	else {
		l_LimitTimer = m_StrongLimit[STATE_AVATAR];
	}
	m_MoveTimer++;
	if (m_MoveTimer == l_LimitTimer) {
		for (int i = 0; i < AVATAR_NUM; i++) {
			InterBoss* boss;
			boss = new AvatarBoss();
			boss->SetAvatarType(i * 1);
			boss->Initialize();
			avatarboss.push_back(boss);
			m_AvatarCount++;
		}
		m_AttackCount++;
		m_MoveTimer = {};
		//二回攻撃したら吸収行動に移行する
		if (m_AttackCount != 4) {
			_charaState = STATE_INTER;
			m_Return = true;
		}
		else {
			_charaState = STATE_CATCH;
		}
	}
	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);
}
//捕まえているゴーストを操る
void SevenBoss::Manipulate() {
	int l_LimitTimer = {};
	//強化によって変わる
	if (!isStrong) {
		l_LimitTimer = m_Limit[STATE_MANIPULATE];
	}
	else {
		l_LimitTimer = m_StrongLimit[STATE_MANIPULATE];
	}
	if (m_StartMani) {
		m_MoveTimer++;
		if (m_MoveTimer == 45) {
			confueffect->SetAlive(true);
			confueffect->SetColor({ 1.0f,0.0f,1.0f,1.0f });
		}
		//混乱の瞬間
		if (m_MoveTimer == 50) {
			m_Manipulate = true;
		}
		else if (m_MoveTimer == 51) {
			m_Manipulate = false;
		}

		if (m_MoveTimer == l_LimitTimer) {
			m_MoveTimer = {};
			m_Manipulate = false;
			m_StartMani = false;
			m_AttackCount++;

			//二回攻撃したら吸収行動に移行する
			if (m_AttackCount != 4) {
				_charaState = STATE_INTER;
				m_Return = true;
			}
			else {
				_charaState = STATE_CATCH;
			}
		}
	}
	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);
}
//火の玉攻撃
void SevenBoss::FireAttack() {
	int l_LimitTimer = {};
	//強化によって変わる
	if (!isStrong) {
		l_LimitTimer = m_Limit[STATE_FIRE];
	}
	else {
		l_LimitTimer = m_StrongLimit[STATE_FIRE];
	}
	m_MoveTimer++;
	if (m_MoveTimer == 1) {
		BirthFire();
	}
	if (m_MoveTimer == l_LimitTimer) {
		m_MoveTimer = {};
		m_AttackCount++;

		//二回攻撃したら吸収行動に移行する
		if (m_AttackCount != 4) {
			_charaState = STATE_INTER;
			m_Return = true;
		}
		else {
			_charaState = STATE_CATCH;
		}
	}
	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);
}
void SevenBoss::BirthFire() {
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandomMove(0, 7);
	m_DeleteNumber = l_RandomMove(mt);
	//火の玉
	for (int i = 0; i < FIRE_NUM; i++) {
		FireBoll* newfire;
		newfire = new FireBoll();
		newfire->Initialize();
		newfire->SetCircleSpeed(i * 45.0f);
		fireboll.push_back(newfire);
	}

	fireboll[m_DeleteNumber]->SetDelete(true);
}
//プレイヤー混乱
void SevenBoss::Confu() {
	m_MoveTimer++;
	const int l_LimitConfu = 80;
	int l_ConfuTimer = {};
	int l_EndTimer = {};
	//強化によって変わる
	if (!isStrong) {
		l_EndTimer = m_Limit[STATE_CONFU];
	}
	else {
		l_EndTimer = m_StrongLimit[STATE_CONFU];
	}
	if (m_MoveTimer == l_LimitConfu) {
		confueffect->SetAlive(true);
		confueffect->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		Player::GetInstance()->SetConfu(true);
		if (isStrong) {
			l_ConfuTimer = 600;
		}
		else {
			l_ConfuTimer = 300;
		}
		Player::GetInstance()->SetConfuTimer(l_ConfuTimer);
	}
	else if (m_MoveTimer == l_EndTimer) {
		m_MoveTimer = {};
		m_AttackCount++;

		//二回攻撃したら吸収行動に移行する
		if (m_AttackCount != 4) {
			_charaState = STATE_INTER;
			m_Return = true;
		}
		else {
			_charaState = STATE_CATCH;
		}
	}
	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);
}
//ダメージのブロック
void SevenBoss::BlockAttack() {
	int l_LimitTimer = {};
	//強化によって変わる
	if (!isStrong) {
		l_LimitTimer = m_Limit[STATE_BLOCK];
	}
	else {
		l_LimitTimer = m_StrongLimit[STATE_BLOCK];
	}
	m_MoveTimer++;
	if (m_MoveTimer == 1) {
		BirthBlock();
	}
	if (m_MoveTimer == l_LimitTimer) {
		m_MoveTimer = {};
		m_AttackCount++;
		//二回攻撃したら吸収行動に移行する
		if (m_AttackCount != 4) {
			_charaState = STATE_INTER;
			m_Return = true;
		}
		else {
			_charaState = STATE_CATCH;
		}
	}
	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);
}
//ブロックの生成
void SevenBoss::BirthBlock() {
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
		newblock->InitPos(i,{l_SetPosX,0.0f,l_SetPosZ});
		damageblock.push_back(newblock);
	}
}
//ポルターガイストの生成
void SevenBoss::BirthPolter(const std::string& PolterName) {
	const int l_LimitTimer = 20;//障害物が動くまでの時間
	const int l_LimitTimer2 = 50;//障害物が動くまでの時間2
	if (PolterName == "Normal") {
		for (int i = 0; i < POLTER_NUM; i++) {
			//障害物の発生
			Poltergeist* newpolter;
			newpolter = new Poltergeist();
			newpolter->Initialize();
			
			newpolter->SetPolterType(TYPE_FOLLOW);
			newpolter->SetTargetTimer(i * l_LimitTimer);
			if (i == 0) {
				newpolter->SetPosition({ m_Position.x + 6.0f,m_Position.y - 10.0f,m_Position.z });
			}
			else if (i == 1) {
				newpolter->SetPosition({ m_Position.x - 6.0f,m_Position.y - 10.0f,m_Position.z });
			}
			else if (i == 2) {
				newpolter->SetPosition({ m_Position.x,m_Position.y - 10.0f,m_Position.z + 6.0f});
			}
			else {
				newpolter->SetPosition({ m_Position.x,m_Position.y - 10.0f,m_Position.z - 6.0f });
			}
			poltergeist.push_back(newpolter);
		}
	}else if(PolterName == "Bound") {
		for (int i = 0; i < POLTER_NUM; i++) {
			//障害物の発生
			Poltergeist* newpolter;
			newpolter = new Poltergeist();
			newpolter->Initialize();

			newpolter->SetPolterType(TYPE_BOUND);
			newpolter->SetTargetTimer(i * l_LimitTimer2);
			newpolter->SetBasePos(m_Position);
			newpolter->SetPosition({ m_Position.x,m_Position.y - 10.0f,m_Position.z });
			newpolter->SetCircleSpeed(i * 90.0f);
			poltergeist.push_back(newpolter);
		}
	}
}
//弾を吸収
void SevenBoss::BulletCatch() {
	int l_LimitTimer = {};
	//強化によって変わる
	if (!isStrong) {
		l_LimitTimer = m_Limit[STATE_CATCH];
	}
	else {
		l_LimitTimer = m_StrongLimit[STATE_CATCH];
	}
	//弾とボスの当たり判定
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
	CatchBul(_playerBulA);
	m_MoveTimer++;
	m_Absorption = true;
	if (m_MoveTimer == l_LimitTimer) {
		m_MoveTimer = {};
		m_AttackCount = {};
		m_Absorption = false;
		_charaState = STATE_INTER;
		m_Return = true;
	}
	if (m_MoveTimer % 6 == 0) {
		BirthParticle();
	}

	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);
}
//スタンした時
void SevenBoss::Stun() {
	int l_LimitTimer = {};
	m_Rotation.y += 2.0f;
	//強化によって変わる
	if (!isStrong) {
		l_LimitTimer = m_Limit[STATE_STUN];
	}
	else {
		l_LimitTimer = m_StrongLimit[STATE_STUN];
	}
	m_MoveTimer++;
	if (m_MoveTimer % 80 == 0) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/piyopiyo.wav", VolumManager::GetInstance()->GetSEVolum() * 0.7f);
	}
	m_Absorption = false;
	if (m_MoveTimer == l_LimitTimer) {
		m_Stun = false;
		m_MoveTimer = {};
		m_InterVal = {};
		_charaState = STATE_INTER;
		bossstuneffect->SetAlive(false);
		m_Return = true;
	}
	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);

	//リバウンド処理
	Rebound();
}
//リバウンド
void SevenBoss::Rebound() {
	m_BullBoundPower = {
Ease(In,Cubic,0.5f,m_BullBoundPower.x,0.0f),
Ease(In,Cubic,0.5f,m_BullBoundPower.y,0.0f),
	};

	m_Position.x += m_BullBoundPower.x;
	m_Position.z += m_BullBoundPower.y;
}
//登場シーン
void SevenBoss::AppearAction() {
	//sin波によって上下に動く
	m_SinAngle += 2.0f;
	m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
	m_Position.y = (sin(m_SinAngle2) * 0.5f + 5.0f);
	m_AppearTimer++;
	if (m_AppearTimer >= 2250) {
		if (Helper::GetInstance()->CheckMax(m_Position.z, 25.0f, -0.05f)) {
		}
	}
	Obj_SetParam();
}
//ボス撃破シーン
void SevenBoss::DeadAction() {
	const float l_AddFrame = 0.01f;
	m_DeathTimer++;
	if (_DeathState == DEATH_SET) {
		if (m_DeathTimer == 1) {
			m_Position = { 0.0f,5.0f,-10.0f };
			m_Rotation = { 0.0f,180.0f,0.0f };
			m_BoundPower = 1.0f;
		}
		else if (m_DeathTimer == 20) {
			m_Bound = true;
		}
		else if (m_DeathTimer == 400) {
			m_AfterPos = { 0.0f,5.0f,-40.0f };
			m_Frame = {};
			_DeathState = DEATH_ATTACK;
		}

		if (m_Bound) {
			m_BoundPower = Ease(In, Cubic, 0.3f, m_BoundPower, 0.0f);
			if (Helper::GetInstance()->CheckMin(m_Position.z,22.0f,m_BoundPower)) {
				m_Bound = false;
			}
		}
	}
	else if(_DeathState == DEATH_ATTACK) {
		
		if (m_DeathTimer == 800) {
			m_Frame = {};
			_DeathState = DEATH_TALK;
		}
	}
	else if (_DeathState == DEATH_TALK) {
		//sin波によって上下に動く
		m_SinAngle += 6.0f;
		m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
		m_Position.y = (sin(m_SinAngle2) * 1.0f + 5.0f);

		if (m_DeathTimer == 1000 || m_DeathTimer == 1200 || m_DeathTimer == 1500) {
			_DeathState = DEATH_NO_TALK;
		}
	}
	else {
		if (m_DeathTimer == 1100 || m_DeathTimer == 1400 || m_DeathTimer == 1700) {
			_DeathState = DEATH_TALK;
		}
	}

	//向き
	if (_EndDir == DIR_NONE) {
		if (m_DeathTimer == 1050) {
			_EndDir = DIR_PLAYER;
		}
	}
	else if (_EndDir == DIR_PLAYER) {
		Helper::GetInstance()->CheckMax(m_Rotation.y, 135.0f, -2.0f);
		if (m_DeathTimer == 1650) {
			_EndDir = DIR_RETURN;
		}
	}
	else {
		Helper::GetInstance()->CheckMin(m_Rotation.y, 180.0f, 2.0f);
	}

	Obj_SetParam();
}
//ボス撃破シーン(スロー)
void SevenBoss::DeadAction_Throw() {
	Obj_SetParam();
}
//弾の吸収判定
void SevenBoss::CatchBul(vector<InterBullet*> bullet)
{
	const float l_CatchRadius = 10.0f;
	const float l_StunRadius = 1.0f;
	for (InterBullet* _bullet : bullet) {
		if (_bullet != nullptr && _bullet->GetAlive()) {
			_bullet->SetTargetPos(m_Position);
			if (Collision::CircleCollision(_bullet->GetPosition().x, _bullet->GetPosition().z, l_CatchRadius, m_Position.x, m_Position.z, l_CatchRadius)) {
				_bullet->SetBossCatch(true);
			}

			if (Collision::CircleCollision(_bullet->GetPosition().x, _bullet->GetPosition().z, _bullet->GetRadius(), m_Position.x, m_Position.z, l_StunRadius) && (!m_Stun)) {
				BirthExplosion();
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Explo.wav", VolumManager::GetInstance()->GetSEVolum() * 1.3f);
				bossstuneffect->SetAlive(true);
				m_Stun = true;
				m_AttackCount = {};
				//弾の大きさによって与えるダメージが違う
				m_HP -= _bullet->GetPower() * m_Magnification;
				m_Absorption = false;
				_charaState = STATE_STUN;
				m_MoveTimer = {};
				_bullet->SetAlive(false);
				XMFLOAT2 l_Distance;
				l_Distance.x = m_Position.x - _bullet->GetPosition().x + 0.1f;
				l_Distance.y = m_Position.z - _bullet->GetPosition().z;
				m_BullBoundPower.x = (sin(atan2f(l_Distance.x, l_Distance.y)) * 2.0f);
				m_BullBoundPower.y = (cos(atan2f(l_Distance.x, l_Distance.y)) * 2.0f);
			}
		}
	}
}
//爆発エフェクト
void SevenBoss::BirthExplosion() {
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
//パーティクル
void SevenBoss::BirthParticle() {
	//ノーツの発生
	AbsorptionEffect* neweffect;
	neweffect = new AbsorptionEffect();
	neweffect->Initialize();
	neweffect->SetBasePos(m_Position);
	neweffect->SetColor({ 0.5f,0.0f,0.5f,1.0f });
	neweffect->SetAddFrame(0.01f);
	abseffect.push_back(neweffect);
}
//ボスの消える判定
void SevenBoss::VanishCollide(vector<InterBullet*> bullet)
{
	int l_RandCount = {};
	XMFLOAT2 m_RandPos = {};
	int l_RandDir = {};
	const float l_VanishRadius = m_Radius + 5.0f;
	for (InterBullet* _bullet : bullet) {
		if (_bullet != nullptr && _bullet->GetAlive() && !_bullet->GetInArea()) {
			const float l_VanishRadius2 = _bullet->GetRadius() + 5.0f;
			if (Collision::CircleCollision(_bullet->GetPosition().x, _bullet->GetPosition().z, l_VanishRadius, m_Position.x, m_Position.z, l_VanishRadius2) && (!m_Stun) && (!m_Vanish)) {
				//乱数指定
				mt19937 mt{ std::random_device{}() };
				uniform_int_distribution<int> l_RandomRange(1, 100);
				l_RandCount = int(l_RandomRange(mt));
				_bullet->SetInArea(true);

				//乱数が一定の値より大きかったらハートが出る
				if (l_RandCount > m_VanishTarget) {
					m_Vanish = true;
					m_VanishFrame = {};
					m_AfterAlpha = {};
					_vanishState = VANISH_SET;
				}
			}
		}
	}
}
//ボスが一瞬消える
void SevenBoss::VanishBoss() {
	const float l_AddFrame = 0.05f;
	if (m_Vanish) {
		if (_vanishState == VANISH_SET) {
			if (m_VanishFrame < m_FrameMax) {
				m_VanishFrame += l_AddFrame;
			}
			else {
				m_VanishFrame = {};
				m_AfterAlpha = 1.0f;
				_vanishState = VANISH_END;
			}
		}
		else {
			if (m_VanishFrame < m_FrameMax) {
				m_VanishFrame += l_AddFrame;
			}
			else {
				m_VanishFrame = {};
				m_AfterAlpha = 0.0f;
				_vanishState = VANISH_SET;
				m_Vanish = false;
			}
		}
		m_Color.w = Ease(In, Cubic, m_VanishFrame, m_Color.w, m_AfterAlpha);

	}
}
//ランダムの動き
void SevenBoss::RandMove() {
	const int l_Division = 10;
	int l_Dir = {};
	m_ChangeTimer++;

	//一定フレームで進行方向を決める
	if (m_ChangeTimer == 50) {
		//乱数生成(加算力と大きさ)
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_RandScale(-5, 10);
		m_AddScale = float(l_RandScale(mt)) / l_Division;
		m_ChangeTimer = {};
	}
	
	Helper::GetInstance()->Clamp(m_CircleScale, 20.0f, 30.0f);
	m_CircleScale += m_AddScale;
	m_CircleSpeed += m_AddSpeed;
	m_AfterPos = Helper::GetInstance()->CircleMove(Player::GetInstance()->GetPosition(), m_CircleScale, m_CircleSpeed);
	
	m_Position = {
		Ease(In,Cubic,0.5f,m_Position.x,m_AfterPos.x),
		m_Position.y,
		Ease(In,Cubic,0.5f,m_Position.z,m_AfterPos.z),
	};

	m_Color.w = Ease(In, Cubic, 0.5f, m_Color.w, 1.0f);
}
void SevenBoss::ReturnBoss() {
	const float l_AddFrame = 0.05f;
	if (_ReturnState == RETURN_SET) {
		m_AfterAlpha = {};
		if (m_VanishFrame < m_FrameMax) {
			m_VanishFrame += l_AddFrame;
		}
		else {
			//乱数生成(円周のどこか)
			mt19937 mt{ std::random_device{}() };
			uniform_int_distribution<int> l_RandSpeed(0, 360);
			m_RandSpeed = float(l_RandSpeed(mt));
			_ReturnState = RETURN_PLAY;
			m_VanishFrame = {};
		}
	}
	else if (_ReturnState == RETURN_PLAY) {
		m_CircleScale = 30.0f;
		m_CircleSpeed = m_RandSpeed;
		m_Position = Helper::GetInstance()->CircleMove({ Player::GetInstance()->GetPosition().x,m_Position.y,Player::GetInstance()->GetPosition().z}, m_CircleScale, m_CircleSpeed);
		m_Rotation.y = Helper::GetInstance()->DirRotation(m_Position, Player::GetInstance()->GetPosition(), -PI_90);
		_ReturnState = RETURN_END;
		m_AfterAlpha = 1.0f;
	}
	else {
		if (m_VanishFrame < m_FrameMax) {
			m_VanishFrame += l_AddFrame;
		}
		else {
			_ReturnState = RETURN_SET;
			m_VanishFrame = {};
			m_Return = false;
		}
	}

	m_Color.w = Ease(In, Cubic, m_VanishFrame, m_Color.w, m_AfterAlpha);
}
//覚醒シーン後の初期化
void SevenBoss::InitAwake() {
	if (m_DeleteObj && !m_AwakeInit) {
		avatarboss.clear();
		poltergeist.clear();
		abseffect.clear();
		fireboll.clear();
		damageblock.clear();
		m_Position = { 0.0f,5.0f,30.0f };
		m_Rotation = { 0.0f,180.0f,0.0f };
		m_Scale = { 1.5f,1.5f,1.5f };
		m_InterVal = {};
		m_MoveTimer = {};
		//攻撃回数
		m_AttackCount = {};
		//スタンしたかどうか
		m_Stun = false;
		//敵が弾を避けるかどうか
		m_Vanish = false;
		_vanishState = VANISH_SET;
		//透明化する時間
		m_VanishFrame = {};
		//糖度
		m_AfterAlpha = {};
		m_AfterPos = {};
		_charaState = STATE_INTER;
		_ReturnState = RETURN_SET;
		m_CircleSpeed = 90.0f;
		m_RotTimer = {};
		m_StartMani = false;
		m_DeleteObj = false;
		m_AvatarCount = {};
		m_SinAngle = {};
		m_AwakeInit = true;
	}
}
void SevenBoss::EndRollAction() {
	const float l_AddFrame = 0.01f;
	m_EndTimer++;
	if (_EndState == END_SET) {
		if (m_EndTimer == 1) {
			m_Position = { 0.0f,5.0f,15.0f };
			m_Rotation = { 0.0f,180.0f,0.0f };
			m_Color = { 1.0f,1.0f,1.0f,0.0f };
		}
		else if (m_EndTimer == 900) {
			_EndState = END_WALK;
		}
	}
	else if (_EndState == END_WALK) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			_EndState = END_DIR_CAMERA;
			m_Frame = {};
		}

		m_Color.w = Ease(In, Cubic, m_Frame, m_Color.w, 1.0f);
	}
	else {
		if (m_EndTimer == 1670) {
			m_EndStop = true;
		}
	}

	//sin波によって上下に動く
	if (!m_EndStop) {
		m_SinAngle += 6.0f;
		m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
		m_Position.y = (sin(m_SinAngle2) * 0.5f + 5.0f);
	}
	//OBJのステータスのセット
	Obj_SetParam();
}
//会話
void SevenBoss::AfterAction() {
}