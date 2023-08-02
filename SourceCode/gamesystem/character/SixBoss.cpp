#include "SixBoss.h"
#include <any>
#include <random>
#include "Collision.h"
#include "CsvLoader.h"
#include "Player.h"
#include "Easing.h"
#include "Helper.h"

void (SixBoss::* SixBoss::stateTable[])() = {
	&SixBoss::InterValMove,//動きの合間
	&SixBoss::Choice,//選択
	&SixBoss::LineSet,//ダメージエリアのセット
	&SixBoss::Debuff,//デバフ
	&SixBoss::Confu,//混乱
	&SixBoss::Barrage,//弾幕
	&SixBoss::EndMove,//行動の終わり
};
//生成
SixBoss::SixBoss() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::DJ);

	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);

	//CDの初期化
	cd[CD_BARRA].reset(new BarrangeCD);
	cd[CD_BARRA]->Initialize();

	cd[CD_LINE].reset(new LineCD);
	cd[CD_LINE]->Initialize();

	cd[CD_CONFU].reset(new ConfuCD);
	cd[CD_CONFU]->Initialize();

	cd[CD_DEBUFF].reset(new DebuffCD);
	cd[CD_DEBUFF]->Initialize();

	confueffect.reset(new ConfuEffect());
	confueffect->Initialize();

	noteeffect.reset(new NoteEffect());
	noteeffect->Initialize();
}
//初期化
bool SixBoss::Initialize() {
	m_Position = { 0.0f,3.0f,30.0f };
	m_Rotation = { 0.0f,90.0f,0.0f };
	m_Scale = { 0.3f,0.3f,0.3f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	//m_Rotation.y = -90.f;

	ActionTimer = 1;

	m_Radius = 5.2f;

	_charaState = STATE_INTER;
	m_AreaState = AREA_SET;
	//CSVロード
	CSVLoad();

	effects.clear();
	return true;
}
//スキップ時の初期化
void SixBoss::SkipInitialize() {
	m_Position = { 0.0f,3.0f,30.0f };
	m_Rotation = { 0.0f,90.0f,0.0f };
	m_Scale = { 0.3f,0.3f,0.3f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
}
//CSV
void SixBoss::CSVLoad() {
	auto LimitSize = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "LIMIT_NUM")));

	m_Limit.resize(LimitSize);
	LoadCSV::LoadCsvParam_Int("Resources/csv/chara/boss/Six/Sixboss.csv", m_Limit, "Interval");

	m_Magnification = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "Magnification")));
	m_HP = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "hp1")));
	m_BirthTarget = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "HeartTarget")));
	m_FollowSpeed = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "Speed")));
	m_MaxHp = m_HP;
}
//行動
void SixBoss::Action() {
	//状態移行(charastateに合わせる)
	if (m_HP > 0.0f) {
		//(this->*stateTable[_charaState])();
	}

	/*^^^^当たり判定^^^^*/
	//弾とボスの当たり判定
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
	CollideBul(_playerBulA, Type::CIRCLE);
	//プレイヤーの当たり判定
	ColPlayer();
	//OBJのステータスのセット
	Obj_SetParam();
	//リミット制限
	Helper::GetInstance()->Clamp(m_Position.x, -55.0f, 65.0f);
	Helper::GetInstance()->Clamp(m_Position.z, -60.0f, 60.0f);

	//CDの更新
	for (size_t i = 0; i < cd.size(); i++) {
		cd[i]->SetCatchPos({ m_Position.x,((m_Position.y + 2.0f) + (0.5f * i)),m_Position.z });
		cd[i]->Update();

		if (cd[i]->GetBreakCD()) {
			m_EndCount++;
			cd[i]->SetBreakCD(false);
		}
	}

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

	if (damagearea != nullptr) {
		damagearea->Update();
	}

	confueffect->SetBasePos(Player::GetInstance()->GetPosition());
	confueffect->Update();
	noteeffect->Update();
	//HPが半分切ったら強化
	if (m_HP < m_MaxHp / 2) {
		isStrong = true;
	}
}
//ポーズ
void SixBoss::Pause() {

}
//エフェクト描画
void SixBoss::EffecttexDraw(DirectXCommon* dxCommon)
{
	if (m_HP <= 0.0f)return;

	confueffect->Draw(dxCommon);
	noteeffect->Draw(dxCommon);
}
//描画
void SixBoss::Draw(DirectXCommon* dxCommon) {
	Obj_Draw();
	//CDの更新
	for (size_t i = 0; i < cd.size(); i++) {
		cd[i]->Draw(dxCommon);
	}
	if (m_HP > 0.0f) {
		//攻撃の音符
		for (AttackNote* newnote : attacknotes) {
			if (newnote != nullptr) {
				newnote->Draw(dxCommon);
			}
		}
		if (damagearea != nullptr) {
			damagearea->Draw(dxCommon);
		}
		EffecttexDraw(dxCommon);
	}
}
//ImGui
void SixBoss::ImGui_Origin() {
}
//インターバル
void SixBoss::InterValMove() {
	m_MoveInterVal++;
	m_AreaState = AREA_SET;
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandomMove(0, 3);
	//すべてが消えていたら復活させる
	if ((cd[CD_LINE]->GetCDState() == CD_DEATH) && (cd[CD_DEBUFF]->GetCDState() == CD_DEATH) &&
		(cd[CD_CONFU]->GetCDState() == CD_DEATH) && (cd[CD_BARRA]->GetCDState() == CD_DEATH)) {
		m_EndCount = 0;
		_charaState = STATE_END;
	}

	//
	if (m_MoveInterVal == m_Limit[LIMIT_BASE]) {
		//行動を決めて次の行動に移る
		m_AttackRand = int(l_RandomMove(mt));
		if (cd[m_AttackRand]->GetCDState() != CD_STAY) {
			m_MoveInterVal = 49;
		}
		else {
			m_AfterPos = cd[m_AttackRand]->GetPosition();
			m_AfterRot.y = Helper::GetInstance()->DirRotation(m_Position, cd[m_AttackRand]->GetPosition(), -PI_180);
			m_MoveInterVal = {};
			_charaState = STATE_CHOICE;
		}
	}
}
//動きの選択
void SixBoss::Choice() {
	const float l_AddAngle = 5.0f;
	float l_AddFrame = 0.001f;
	//二点間の距離計算
	m_Length = Helper::GetInstance()->ChechLength({ m_Position.x,0.0f,m_Position.z }, { m_AfterPos.x,0.0f,m_AfterPos.z });
	//次のCDを狙う
	if (m_Length > 0.5f) {
		Helper::GetInstance()->FollowMove(m_Position, m_AfterPos, m_FollowSpeed);
	}
	else {
		//行動を決めて次の行動に移る
		m_StopTimer++;
		if (m_StopTimer > m_Limit[LIMIT_BASE]) {
			m_StopTimer = 0;
			m_Angle = {};
			if (cd[m_AttackRand]->GetCDState() != CD_STAY) {
				_charaState = STATE_INTER;
			}
			else {
				cd[m_AttackRand]->SetAttackSetCD(true);
				_charaState = m_AttackRand + 2;
				cd[m_AttackRand]->SetCDState(CD_CATCH);
				m_EndCount++;
			}
		}
	}

	//sin波によって上下に動く
	m_Angle += l_AddAngle;
	m_Angle2 = m_Angle * (3.14f / 180.0f);
	m_Position.y = (sin(m_Angle2) * 2.0f + 2.0f);

	m_AfterRot.y = Helper::GetInstance()->DirRotation(m_Position, m_AfterPos, PI_90);

	m_Rotation.y = Ease(In, Cubic, 0.5f, m_Rotation.y, m_AfterRot.y);
}
//ダメージエリアのセット
void SixBoss::LineSet() {
	int l_BirthNum = {};
	const int l_StartLimit = 120;

	//HPの削り具合によって出す線の数を変える
	if (isStrong) {
		l_BirthNum = 4;
	}
	else {
		l_BirthNum = 2;
	}

	m_CheckTimer++;

	//最初にSE鳴らす
	if (m_CheckTimer == 70) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/DJSE1.wav", VolumManager::GetInstance()->GetSEVolum());
	}

	if (m_CheckTimer > l_StartLimit) {
		if (m_AreaState == AREA_SET) {
			damagearea.reset(new DamageArea(l_BirthNum));
			damagearea->Initialize();
			m_AreaState = AREA_STOP;
		}
		else if (m_AreaState == AREA_STOP) {
			cd[CD_LINE]->SetCDState(CD_DEATH);
			_charaState = STATE_INTER;
			m_CheckTimer = {};
		}
	}
}
//プレイヤーのデバフ
void SixBoss::Debuff() {
	const int l_StartLimit = 150;
	m_CheckTimer++;
	if (m_CheckTimer == 60) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/DJSE1.wav", VolumManager::GetInstance()->GetSEVolum());
	}
	else if (m_CheckTimer == 80) {
		noteeffect->SetAlive(true);
	}
	else if (m_CheckTimer == 110) {
		m_Check = true;
	}
	else if (m_CheckTimer == l_StartLimit) {
		cd[CD_DEBUFF]->SetCDState(CD_DEATH);
		_charaState = STATE_INTER;
		m_CheckTimer = {};
	}
}
//プレイヤー混乱
void SixBoss::Confu() {
	m_CheckTimer++;
	const int l_LimitConfu = 80;
	const int l_EndConfu = 120;
	int l_ConfuTimer = {};
	if (m_CheckTimer == 60) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/DJSE1.wav", VolumManager::GetInstance()->GetSEVolum());
	}

	if (m_CheckTimer == l_LimitConfu) {
		confueffect->SetAlive(true);
		Player::GetInstance()->SetConfu(true);
		if (isStrong) {
			l_ConfuTimer = m_Limit[LIMIT_STRONG_CONFU];
		}
		else {
			l_ConfuTimer = m_Limit[LIMIT_CONFU];
		}
		Player::GetInstance()->SetConfuTimer(l_ConfuTimer);
	}
	else if (m_CheckTimer == l_EndConfu) {
		cd[CD_CONFU]->SetCDState(CD_DEATH);
		_charaState = STATE_INTER;
		m_CheckTimer = {};
	}
}
//拡散(ふつう)
void SixBoss::Barrage() {
	const int l_StartLimit = 100;

	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandomMove(0, 1);

	m_CheckTimer++;
	if (m_CheckTimer == 70) {
		m_BarraRand = int(l_RandomMove(mt));
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/DJSE1.wav", VolumManager::GetInstance()->GetSEVolum());
	}
	else if (m_CheckTimer > l_StartLimit) {
		m_Rotation.y += 2.0f;
		m_RotTimer++;
		if (m_RotTimer % 5 == 0) {
			//乱数によって弾幕の種類が変わる
			if (m_BarraRand == 0) {
				if (!isStrong) {
					BirthNote("NORMAL");
				}
				else {
					BirthNote("ALTER");
				}
			}
			else if (m_BarraRand == 1) {
				BirthNote("RANDOM");
			}
		}

		//一定フレームで終了
		if (m_RotTimer == m_Limit[LIMIT_BARRA]) {
			cd[CD_BARRA]->SetCDState(CD_DEATH);
			m_RotTimer = {};
			m_CheckTimer = {};
			m_CircleScale = 30.0f;
			m_CircleSpeed = {};
			m_BarraRand = {};
			_charaState = STATE_INTER;
		}

		if (m_Rotation.y > 360.0f) {
			m_Rotation.y = 0.0f;
		}

		//強さによって回転が変わる
		if (isStrong) {
			m_CircleSpeed += 1.0f;
			m_CircleScale += 0.3f;
		}
		else {
			m_CircleSpeed += 0.5f;
			m_CircleScale += 0.2f;
		}
		m_AfterPos = Helper::GetInstance()->CircleMove({}, m_CircleScale, m_CircleSpeed);

		m_Position = {
			Ease(In,Cubic,0.2f,m_Position.x,m_AfterPos.x),
			m_Position.y,
			Ease(In,Cubic,0.2f,m_Position.z,m_AfterPos.z),
		};
	}
}
//行動の終わり(プレイヤーから逃げる)
void SixBoss::EndMove() {
	const int l_EndLimit = 100;
	const float l_AddAngle = 5.0f;
	float l_AddSpeed = {};
	const float l_FollowSpeed = 0.3f;
	m_EndTimer++;

	//二点間の距離計算
	m_Length = Helper::GetInstance()->ChechLength({ m_Position.x,0.0f,m_Position.z }, {});
	//次のCDを狙う
	if (m_Length > 0.5f) {
		Helper::GetInstance()->FollowMove(m_Position, {}, l_FollowSpeed);
		m_AfterRot.y = Helper::GetInstance()->DirRotation(m_Position,{}, PI_90);
		m_Rotation.y = Ease(In, Cubic, 0.5f, m_Rotation.y, m_AfterRot.y);
		m_AddPower -= m_Gravity;
		if (Helper::GetInstance()->CheckMax(m_Position.y, {}, m_AddPower)) {
			m_AddPower = 0.5f;
		}
	}
	else {
		_charaState = STATE_INTER;
		m_EndTimer = {};
		m_AddPower = {};
	}

	if (m_EndTimer == 30) {
		for (int i = 0; i < cd.size(); i++) {
			cd[i]->SetCDState(CD_RESPORN);
		}
	}
}
//ノーツの生成
void SixBoss::BirthNote(const std::string& BarrageName) {
	//ヒトツずつ生成されるパターン
	if (BarrageName == "NORMAL" || BarrageName == "RANDOM") {
		XMVECTOR move = { 0.0f, 0.0f, 0.1f, 0.0f };
		XMMATRIX matRot = {};
		if (BarrageName == "RANDOM") {		//ランダム
			mt19937 mt{ std::random_device{}() };
			uniform_int_distribution<int> l_RandomRot(0, 360);
			matRot = XMMatrixRotationY(XMConvertToRadians(float(l_RandomRot(mt))));
		}
		else if (BarrageName == "NORMAL") {	//ボスの向き依存
			matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
		}
		move = XMVector3TransformNormal(move, matRot);
		XMFLOAT2 l_Angle;
		l_Angle.x = move.m128_f32[0];
		l_Angle.y = move.m128_f32[2];
		//ノーツの発生
		AttackNote* newnote;
		newnote = new AttackNote();
		newnote->Initialize();
		newnote->SetPosition(m_Position);
		newnote->SetAngle(l_Angle);
		attacknotes.push_back(newnote);
	}
	else if (BarrageName == "ALTER") {		//同時に弾を出す
		for (int i = 0; i < BULLET_NUM; i++) {
			XMVECTOR move2 = { 0.0f, 0.0f, 0.1f, 0.0f };

			XMMATRIX matRot2;
			if (i == 0) {
				matRot2 = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
			}
			else if (i == 1) {
				matRot2 = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y + 180.0f));
			}
			move2 = XMVector3TransformNormal(move2, matRot2);
			XMFLOAT2 l_Angle2;
			l_Angle2.x = move2.m128_f32[0];
			l_Angle2.y = move2.m128_f32[2];

			//ノーツの発生
			AttackNote* newnote;
			newnote = new AttackNote();
			newnote->Initialize();
			newnote->SetPosition(m_Position);
			newnote->SetAngle(l_Angle2);
			attacknotes.push_back(newnote);
		}
	}
}
//登場シーン
void SixBoss::AppearAction() {
	Obj_SetParam();
}
//ボス撃破シーン
void SixBoss::DeadAction() {
	const float l_AddAngle = 5.0f;
	m_DeathTimer++;
	const int l_BaseTarget = 50;
	if (m_DeathTimer == 1) {
		m_Position = { 0.0f,30.0f,15.0f };
		m_Rotation = { 0.0f,0.0f,0.0f };
	}
	else if (m_DeathTimer >= 2 && m_DeathTimer < 300) {
		//sin波によって上下に動く
		m_Angle += l_AddAngle;
		m_Angle2 = m_Angle * (3.14f / 180.0f);
		m_Position.x = (sin(m_Angle2) * 5.0f + 5.0f);
		DeathParticle();
		for (int i = 0; i < cd.size(); i++) {
			cd[i]->DeathMove(m_DeathTimer, (i * l_BaseTarget) + l_BaseTarget);
		}
	}
	else {
		m_Gravity = 0.05f;
		//飛ぶような感じにするため重力を入れる
		m_AddPower -= m_Gravity;
		Helper::GetInstance()->CheckMax(m_Position.y, 2.0f, m_AddPower);
	}

	Obj_SetParam();
}
//ボス撃破シーン(スロー)
void SixBoss::DeadAction_Throw() {
	Obj_SetParam();
}
//撃破パーティクル
void SixBoss::DeathParticle() {
	const XMFLOAT4 s_color = { 1.0f,1.0f,1.0f,1.0f };
	const XMFLOAT4 e_color = { 0.0f,0.0f,1.0f,1.0f };
	float s_scale = 5.0f;
	float e_scale = 0.0f;
	float l_velocity = 0.5f;
	for (int i = 0; i < 3; ++i) {
		ParticleEmitter::GetInstance()->DeathEffect(50, { m_Position.x,(m_Position.y - 1.0f),m_Position.z }, s_scale, e_scale, s_color, e_color, l_velocity);
	}
}

void SixBoss::InitAwake() {

}

void SixBoss::EndRollAction() {
	const float l_AddFrame = 0.01f;
	m_EndTimer++;
	if (_EndState == END_SET) {
		if (m_EndTimer == 1) {
			m_Position = { 50.0f,20.0f,3.0f };
			m_Rotation = { 180.0f,90.0f,-90.0f };
			m_Scale = { 0.2f,0.2f,0.2f };
		}
		else if (m_EndTimer == 700) {
			_EndState = END_WALK;
			m_AfterPos = { 25.0f,1.5f,5.0f };
		}
	}
	else if (_EndState == END_WALK) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			_EndState = END_DIR_CAMERA;
			m_Frame = {};
			m_AfterRot.z = -180.0f;
		}

		m_Position = {
			Ease(In,Cubic,m_Frame,m_Position.x,m_AfterPos.x),
			Ease(In,Cubic,m_Frame,m_Position.y,m_AfterPos.y),
			Ease(In,Cubic,m_Frame,m_Position.z,m_AfterPos.z)
		};
	}
	else {
		for (int i = 0; i < cd.size(); i++) {
			cd[i]->EndMove((20 + (10 * i)));
		}
		Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame);
		m_Rotation.z = Ease(In, Cubic, m_Frame, m_Rotation.z, m_AfterRot.z);
		if (m_EndTimer == 1670) {
			m_EndStop = true;
		}
		if (!m_EndStop) {
			//sin波によって上下に動く
			m_Angle += 6.0f;
			m_Angle2 = m_Angle * (3.14f / 180.0f);
			m_Position.y = (sin(m_Angle2) * 0.5f + 2.0f);
		}
	}
	//OBJのステータスのセット
	Obj_SetParam();
}
//会話
void SixBoss::AfterAction() {

}