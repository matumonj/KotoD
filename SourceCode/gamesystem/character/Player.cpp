#include "Player.h"
#include "CsvLoader.h"
#include "Helper.h"
#include "HungerGauge.h"
#include "Input.h"
#include "Easing.h"
#include "Collision.h"
Player* Player::GetInstance()
{
	static Player instance;

	return &instance;
}
//リソース読み込み
void Player::LoadResource() {

	fbxmodels.reset(new IKEFBXObject3d());
	fbxmodels->Initialize();
	fbxmodels->SetModel(ModelManager::GetInstance()->GetFBXModel(ModelManager::PLAYER));
	fbxmodels->LoadAnimation();

	viewbullet.reset(new ViewBullet());

	skirtobj.reset(new IKEObject3d());
	skirtobj->Initialize();
	skirtobj->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::SKIRT));

	sutoobj.reset(new IKEObject3d());
	sutoobj->Initialize();
	sutoobj->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Sutopon));
}
//初期化
bool Player::Initialize()
{
	fbxmodels->PlayAnimation(0);

	//飢餓ゲージはプレイヤーで管理する
	HungerGauge::GetInstance()->Initialize();

	viewbullet->Initialize();

	playerattach.reset(new PlayerAttach());
	playerattach->Initialize();

	//11
	skirtobj->SetRotation({ 0,0,-90});
	skirtobj->SetScale({2,2,2 });

	//11
	sutoobj->SetRotation({ 0,0,-90 });
	sutoobj->SetScale({ 0.1f,0.1f,0.1f });
	index2 = 9;
	LoadCSV();


	//CSV読み込み
	return true;
}
//CSV読み込み
void Player::LoadCSV() {
	auto LimitSize = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/player/player.csv", "POWER_NUM")));

	m_PowerLimit.resize(LimitSize);
	LoadCSV::LoadCsvParam_Float("Resources/csv/chara/player/player.csv", m_PowerLimit, "PowerLimit");
	for (auto i = 0; i < ABS_NUM; i++) {
		m_Birthabs[i] = false;
	}
	/*CSV読み込み(CSVファイル名,読み込むパラメータの名前,受け取る値)　今は単一の方のみ対応(int float double charとか)*/

//spから間接的にアクセスする方法 (Update()内で専用の変数に代入する必要あり)
	/*①*/LoadCSV::LoadCsvParam("Resources/csv/chara/player/player.csv", "speed1", sp);/*m_AddSpeedにspを代入*/

	//関数の戻り値から直接値を取る方法(こっちのほうが楽ではある　ただ行数が少し長くなる)
	/*②*/m_AddSpeed = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/player/player.csv", "speed2")));
	m_HP = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/player/player.csv", "HP")));
	m_MaxHP = m_HP;
	m_TargetInterVal = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/player/player.csv", "InterVal")));
	m_TargetRigidityTime = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/player/player.csv", "Rigidity")));
}
//ステータスの初期化
void Player::InitState(const XMFLOAT3& pos) {
	m_Position = pos;
	m_Rotation = { 0.0f,0.0f,0.0f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_InterVal = 0;
	m_DamageInterVal = 0;

	m_BulletType = BULLET_FORROW;

	m_BoundPower = { 0.0f,0.0f };

	m_Confu = false;
	m_ConfuTimer = 0;
	//初期化ぶち込み
	Initialize();
	//移動処理用
	velocity /= 5.0f;
	//大きさ
	m_ChargePower = {};
	m_ChargeType = POWER_NONE;
	m_Scale = { 1.2f,0.8f,1.2f };
	HungerGauge::GetInstance()->SetAdditional(0.0f);

	m_HitPlayer = true;

	_DarkState = DARK_SET;
	_LastState = LAST_SET;
	_EndState = END_SET;
	_LastEndState = LAST_END_SET;
	display = true;
	AnimationControl(AnimeName::IDLE, true, 1);
	effects.clear();
	abseffect.clear();
}
//状態遷移
/*CharaStateのState並び順に合わせる*/
void (Player::* Player::stateTable[])() = {
	&Player::Idle,//待機
	&Player::Walk,//移動
};
//更新処理
void Player::Update()
{
	//any_castはdouble型なのでそれをstatic_castでfloatに
	//doubleがatof()関数の戻り値なので変更できない<any_cast<float>で処理通らなかった>
	//つまるところstd::any_cast<double>(〇〇)は固定(static_castで変換)
	/*①*/m_AddSpeed = static_cast<float>(std::any_cast<double>(sp));
	m_HitPlayer = true;
	Input* input = Input::GetInstance();
	/*FBXのカウンタdoubleにしたほうが調整ききやすそう*/

	OldPos = m_Position;
	/*--------キャラの基本動作-------*/
	/*-----------------------------*/
	if (!isStop) {
		//スティックが押されてる間は移動する
		if (input->TiltPushStick(Input::L_UP, 0.0f) ||
			input->TiltPushStick(Input::L_DOWN, 0.0f) ||
			input->TiltPushStick(Input::L_RIGHT, 0.0f) ||
			input->TiltPushStick(Input::L_LEFT, 0.0f))
		{
			//fbxmodels->PlayAnimation(2);
			_charaState = CharaState::STATE_RUN;
		}
		//何もアクションがなかったらアイドル状態
		else
		{
			_charaState = CharaState::STATE_IDLE;
		}
	}
	if (isStop) {
		_charaState = CharaState::STATE_IDLE;
	}
	//弾の管理
	Bullet_Management();

	//状態移行(charastateに合わせる)
	(this->*stateTable[_charaState])();

	index = 15;
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->FollowUpdate(skirtmat);
	skirtobj->SetColor(m_Color);


	fbxmodels->GetBoneIndexMat(index2, sutomat);
	sutoobj->FollowUpdate(sutomat);
	sutoobj->SetColor(m_Color);
	m_viewBull = true;
	//Stateに入れなくていいやつ
	//攻撃のインターバル
	InterVal();
	//飢餓ゲージ更新
	HungerGauge::GetInstance()->Update();

	Helper::GetInstance()->CheckMax(m_DamageInterVal, 0, -1);

	//混乱状態
	if (m_Confu) {
		if (Helper::GetInstance()->CheckMax(m_ConfuTimer, 0, -1)) {
			m_Confu = false;
		}
		if (m_ConfuTimer % 80 == 0) {
			Audio::GetInstance()->PlayWave("Resources/Sound/SE/piyopiyo.wav", VolumManager::GetInstance()->GetSEVolum() * 0.7f);
		}
	}
	//反発
	ReBound();

	m_LoopFlag = true;
	m_AnimationSpeed = 2;
	//適当にダメージ食らってるときは赤色
	if (m_DamageInterVal == 0) {
		m_Color = { 1.0f,1.0f,1.0f,1.0f };
	}
	else {
		m_Color = { 1.0f,0.0f,0.0f,1.0f };
	}

	//リミット制限
	Helper::GetInstance()->Clamp(m_Position.x, -55.0f, 65.0f);
	Helper::GetInstance()->Clamp(m_Position.z, -60.0f, 60.0f);
	Helper::GetInstance()->Clamp(m_HP, 0.0f, 5.0f);
	

	//どっち使えばいいか分からなかったから保留
	SetParam();
	//エフェクト
	for (InterEffect* effect : effects) {
		if (effect != nullptr) {
			effect->Update();
		}
	}

	//エフェクトの削除
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i] == nullptr) {
			continue;
		}

		if (!effects[i]->GetAlive()) {
			effects.erase(cbegin(effects) += i);
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
}
//描画
void Player::Draw(DirectXCommon* dxCommon)
{
	//キャラクター
	fbxmodels->Draw(dxCommon->GetCmdList());
	playerattach->Draw(dxCommon);
	//弾の描画
	BulletDraw(ghostbullets, dxCommon);
	BulletDraw(attackbullets, dxCommon);
	IKEObject3d::PreDraw();
	skirtobj->Draw();
	if (m_viewBull&&display) {
		sutoobj->Draw();
	}
	IKEObject3d::PostDraw();

	//吸収エフェクト
	for (AbsorptionEffect* neweffect : abseffect) {
		if (neweffect != nullptr) {
			neweffect->Draw(dxCommon);
		}
	}
}
//弾の描画
void Player::BulletDraw(std::vector<InterBullet*> bullets, DirectXCommon* dxCommon) {
	for (InterBullet* bullet : bullets) {
		if (bullet != nullptr) {
			bullet->Draw(dxCommon);
		}
	}
}
//ImGui
void Player::ImGuiDraw() {
	ImGui::Begin("Player");
	ImGui::Text("Type:%d", m_DamageInterVal);
	ImGui::End();
	playerattach->ImGuiDraw();
}
//FBXのアニメーション管理(アニメーションの名前,ループするか,カウンタ速度)
void Player::AnimationControl(AnimeName name, const bool& loop, int speed)
{
	//アニメーションを引数に合わせる
	if (_animeName != name)
	{
		fbxmodels->PlayAnimation((int)(name));
	}

	//各種パラメータ反映
	_animeName = name;
	m_LoopFlag = loop;
	m_AnimationSpeed = speed;
}
//歩き(コントローラー)
void Player::Walk()
{
	XMFLOAT3 rot = m_Rotation;

	float AddSpeed = 2.f;
	Input* input = Input::GetInstance();

	float StickX = input->GetLeftControllerX();
	float StickY = input->GetLeftControllerY();
	const float STICK_MAX = 32768.0f;

	//上入力
	if (input->TiltPushStick(Input::L_UP, 0.0f))
		XMFLOAT3 vecvel = MoveVECTOR(XMVECTOR{ 0, 0, velocity, 0 }, angle);

	//下入力
	if (input->TiltPushStick(Input::L_DOWN, 0.0f))
		XMFLOAT3 vecvel = MoveVECTOR(XMVECTOR{ 0, 0, -velocity, 0 }, angle);

	//右入力
	if (input->TiltPushStick(Input::L_RIGHT, 0.0f))
		XMFLOAT3 vecvel = MoveVECTOR(XMVECTOR{ velocity, 0, 0, 0 }, angle);

	//左入力
	if (input->TiltPushStick(Input::L_LEFT, 0.0f))
		XMFLOAT3 vecvel = MoveVECTOR(XMVECTOR{ -velocity, 0, 0, 0 }, angle);


	const float rnd_vel = 0.1f;

	XMFLOAT3 vel{};

	vel.x = static_cast<float>(rand()) / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
	vel.y = static_cast<float>(rand()) / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
	vel.z = static_cast<float>(rand()) / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
	rot.y = angle + atan2f(StickX, StickY) * (PI_180 / PI);

	//プレイヤーの回転角を取る
	m_Rotation = { rot.x, rot.y, rot.z };

	if (m_Rotation.y <= 0.0f) {
		m_Rotation.y = m_Rotation.y + 360.0f;
	}

	XMVECTOR move = { 0.0f, 0.0f, 0.1f, 0.0f };
	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
	move = XMVector3TransformNormal(move, matRot);
	//向いた方向に進む
	if (m_RigidityTime == 0) {
		//混乱していると逆状態になる
		if (!m_Confu) {
			m_Position.x += move.m128_f32[0] * m_AddSpeed;
			m_Position.z += move.m128_f32[2] * m_AddSpeed;
		}
		else {
			m_Position.x -= move.m128_f32[0] * m_AddSpeed;
			m_Position.z -= move.m128_f32[2] * m_AddSpeed;
		}
	}
	if(_animeName!=AnimeName::ATTACK)
	AnimationControl(AnimeName::WALK, true, 1);
}
//VECTOR
XMFLOAT3 Player::MoveVECTOR(XMVECTOR v, float angle)
{
	XMMATRIX rot2 = {};
	rot2 = XMMatrixRotationY(XMConvertToRadians(angle));
	v = XMVector3TransformNormal(v, rot2);
	XMFLOAT3 pos = { v.m128_f32[0], v.m128_f32[1], v.m128_f32[2] };
	return pos;
}
//弾の更新
void Player::Bullet_Management() {
	const int l_TargetCount = 1;
	const int l_Limit = 20;//ショットのチャージ時間
	const float l_AddFrame = 0.1f;

	Helper::GetInstance()->CheckMax(m_ChangeLimit, 0, -1);
	/*-----------------------------*/
	//RB||LBが押されたら弾を切り替える
	if (((Input::GetInstance()->TriggerButton(Input::RB)) || (Input::GetInstance()->TriggerButton(Input::LB))) && (m_canShot) && (m_ChargePower == 0.0f) && (m_ChangeLimit == 0))
	{
		m_ChangeLimit = 5;
		isShotNow = true;
		float nowhunger = HungerGauge::GetInstance()->GetNowHunger();
		if (nowhunger != 0) {
			m_Skip = false;
		}
		if (Input::GetInstance()->TriggerButton(Input::RB)) {
			if (m_BulletType != BULLET_ATTACK) {
				if (!m_CanSearch && m_BulletType == BULLET_FORROW) {
					m_BulletType += 2;
				}
				else {

					m_BulletType++;
				}
				
				if (nowhunger <= 0 && m_BulletType == BULLET_ATTACK) {

					m_BulletType = BULLET_FORROW;
					m_Skip = true;
				}
			}
			else {
				m_BulletType = BULLET_FORROW;
			}
		}
		else if (Input::GetInstance()->TriggerButton(Input::LB)) {
			if (m_BulletType != BULLET_FORROW) {
				if (!m_CanSearch && m_BulletType == BULLET_ATTACK) {
					m_BulletType -= 2;
				}
				else {
					m_BulletType--;
				}
			}
			else {
				if (nowhunger > 0.0f) {
					m_BulletType = BULLET_ATTACK;
				}
				if (nowhunger <= 0) {
					if (m_CanSearch) {

						m_BulletType = BULLET_SEARCH;
					}
					else {

						m_BulletType = BULLET_FORROW;
					}
					m_Skip = true;
				}
			}
		}
	}

	//弾を打った瞬間チャージ量分飢餓ゲージを減らす
	if (m_SubHunger) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		}
		else {
			m_Frame = {};
			m_LimitHunger = {};
			m_SubHunger = false;
		}
		HungerGauge::GetInstance()->SetNowHunger(Ease(In, Cubic, m_Frame, HungerGauge::GetInstance()->GetNowHunger(), m_LimitHunger));
	}
	//攻撃
	//Bが押されたら弾のチャージ
	if (m_BulletType == BULLET_ATTACK) {
		if (Input::GetInstance()->PushButton(Input::B) && (m_InterVal == 0) && (HungerGauge::GetInstance()->GetCatchCount() >= l_TargetCount)
			&& (m_canShot)) {
			isShotNow = true;
			m_ChargePower += 0.2f;
			viewbullet->SetAlive(true);

			//チャージの量によって威力変える
			if (m_ChargePower < m_PowerLimit[POWER_NONE]) {
				m_ChargeType = POWER_NONE;
			}
			else if (m_ChargePower >= m_PowerLimit[POWER_NONE] && m_ChargePower < m_PowerLimit[POWER_MIDDLE]) {
				m_ChargeType = POWER_MIDDLE;
				if (!m_Birthabs[POWER_MIDDLE - 1]) {
					BirthAbs({ 1.0f,1.0f,0.0f,1.0f });
					m_Birthabs[POWER_MIDDLE - 1] = true;
					Audio::GetInstance()->PlayWave("Resources/Sound/SE/charge.wav", VolumManager::GetInstance()->GetSEVolum());
				}
			}
			else if (m_ChargePower >= m_PowerLimit[POWER_MIDDLE] && m_ChargePower < m_PowerLimit[POWER_STRONG]) {
				m_ChargeType = POWER_STRONG;
				if (!m_Birthabs[POWER_STRONG - 1]) {
					BirthAbs({ 1.0f,0.3f,0.0f,1.0f });
					m_Birthabs[POWER_STRONG - 1] = true;
					Audio::GetInstance()->PlayWave("Resources/Sound/SE/charge.wav", VolumManager::GetInstance()->GetSEVolum());
				}
			}
			else {
				m_ChargePower = m_PowerLimit[POWER_STRONG];
				m_ChargeType = POWER_UNLIMITED;
				if (!m_Birthabs[POWER_UNLIMITED - 1]) {
					BirthAbs({ 1.0f,0.0f,0.0f,1.0f });
					m_Birthabs[POWER_UNLIMITED - 1] = true;
					Audio::GetInstance()->PlayWave("Resources/Sound/SE/charge.wav", VolumManager::GetInstance()->GetSEVolum());
				}
			}
		}

		//チャージ中に飢餓ゲージが切れた場合弾が自動で放たれる
		if (m_ChargePower != 0.0f && (HungerGauge::GetInstance()->GetCatchCount() >= l_TargetCount)) {
			if ((HungerGauge::GetInstance()->GetNowHunger() == 0.0f) || (m_ChargePower > HungerGauge::GetInstance()->GetNowHunger())) {
				m_ChargePower = HungerGauge::GetInstance()->GetNowHunger();
				HungerGauge::GetInstance()->SetIsStop(true);
			}
		}

		if (!Input::GetInstance()->PushButton(Input::B) && m_ChargePower != 0.0f) {
			HungerGauge::GetInstance()->SetIsStop(false);
			if (m_ChargeType < POWER_STRONG) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Voice_Shot.wav", VolumManager::GetInstance()->GetSEVolum());
				BirthShot("Attack", false);
				playerattach->SetAlive(true);
			}
			else {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Shot_Charge.wav", VolumManager::GetInstance()->GetSEVolum());
				BirthShot("Attack", true);
				playerattach->SetAlive(true);
			}
			for (auto i = 0; i < ABS_NUM; i++) {
				m_Birthabs[i] = false;
			}
			//減る飢餓ゲージ量を決める
			if (m_ChargeType != POWER_NONE) {
				if (m_ChargeType == POWER_MIDDLE) {
					m_LimitHunger = HungerGauge::GetInstance()->GetNowHunger() - m_PowerLimit[POWER_NONE];
				}
				else if (m_ChargeType == POWER_STRONG) {
					m_LimitHunger = HungerGauge::GetInstance()->GetNowHunger() - m_PowerLimit[POWER_MIDDLE];
				}
				else if (m_ChargeType == POWER_UNLIMITED) {
					m_LimitHunger = HungerGauge::GetInstance()->GetNowHunger() - m_PowerLimit[POWER_STRONG];
				}
				m_Frame = {};
				m_SubHunger = true;
			}
			ResetBullet();
		}

		if (TriggerAttack) {
			if (fbxmodels->GetCurrent() >= fbxmodels->GetEndTime() - 1) {

				fbxmodels->StopAnimation();
				_animeName = AnimeName::IDLE;
				TriggerAttack = false;
			}
		}
	}
	else {			//言魂
		if (Input::GetInstance()->TriggerButton(Input::B) && (m_InterVal == 0) && (m_canShot)) {
			if (m_BulletType == BULLET_FORROW) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Voice_Follow.wav", VolumManager::GetInstance()->GetSEVolum());
			}
			else if (m_BulletType == BULLET_SEARCH) {
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Voice_Seach.wav", VolumManager::GetInstance()->GetSEVolum());
			}
		
			isShotNow = true;
			ResetBullet();
			playerattach->SetAlive(true);
			BirthShot("Ghost", false);
		}

		if (TriggerAttack) {
			if (fbxmodels->GetCurrent() >= fbxmodels->GetEndTime() - 1) {
				fbxmodels->StopAnimation();
				_animeName = AnimeName::IDLE;
				TriggerAttack = false;
			}
		}
	}


	//弾の削除(言霊)
	for (int i = 0; i < ghostbullets.size(); i++) {
		if (ghostbullets[i] == nullptr) {
			continue;
		}

		if (!ghostbullets[i]->GetAlive()) {
			ghostbullets.erase(cbegin(ghostbullets) + i);
		}
	}

	//弾の削除(言霊)
	for (int i = 0; i < attackbullets.size(); i++) {
		if (attackbullets[i] == nullptr) {
			continue;
		}

		if (!attackbullets[i]->GetAlive()) {
			attackbullets.erase(cbegin(attackbullets) + i);
		}
	}

	if (attackbullets.size() <= 0 && ghostbullets.size() <= 0) {
		isShotNow = false;
	}

	//弾の数指定
	if (HungerGauge::GetInstance()->GetCatchCount() < 3) {
		m_BulletNum = 1;
	}
	else if (HungerGauge::GetInstance()->GetCatchCount() >= 3 && HungerGauge::GetInstance()->GetCatchCount() < 6) {
		m_BulletNum = 2;
	}
	else {
		m_BulletNum = 3;
	}

	//弾の更新
	BulletUpdate(ghostbullets);
	BulletUpdate(attackbullets);
	//弾を撃つ方向を算出するために回転を求める
	XMVECTOR move = { 0.0f, 0.0f, 0.1f, 0.0f };
	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
	move = XMVector3TransformNormal(move, matRot);
	XMFLOAT2 l_Angle;
	l_Angle.x = move.m128_f32[0];
	l_Angle.y = move.m128_f32[2];

	//可視化の弾関係
	viewbullet->Update();
	viewbullet->SetAngle(l_Angle);
	viewbullet->SetPosition({ m_Position.x,0.0f,m_Position.z });

	m_ChargePower = min(m_ChargePower, HungerGauge::GetInstance()->GetNowHunger());
	m_ChargePower = max(m_ChargePower, 0.0f);
	SutoponUpdate();
}
void Player::BulletUpdate(std::vector<InterBullet*> bullets) {
	//弾の更新
	for (InterBullet* bullet : bullets) {
		if (bullet != nullptr) {
			bullet->Update();
		}
	}
}
//弾の生成
void Player::BirthShot(const std::string& bulletName, bool Super) {
	float l_ShotMag = 0.0f;
	const int l_BulletNum = m_BulletNum;
	XMVECTOR move2 = { 0.0f, 0.0f, 0.1f, 0.0f };
	XMMATRIX matRot2 = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
	move2 = XMVector3TransformNormal(move2, matRot2);
	XMFLOAT2 l_Angle2;
	l_Angle2.x = move2.m128_f32[0];
	l_Angle2.y = move2.m128_f32[2];
	//攻撃の弾
	if (bulletName == "Attack") {
		if (m_ChargeType == POWER_NONE) {			//チャージしていないやつ
			for (int i = 0; i < m_BulletNum; i++) {
				XMVECTOR move = { 0.0f, 0.0f, 0.1f, 0.0f };

				XMMATRIX matRot;
				//弾の状況によって数と角度を決めている
				if (l_BulletNum == 1) {
					matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
					l_ShotMag = 1.0f;
				}
				else if (l_BulletNum == 2) {
					l_ShotMag = 0.6f;
					if (i == 0) {
						matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y + 5.0f));
					}
					else {
						matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y - 5.0f));
					}
				}
				else {
					l_ShotMag = 0.65f;
					if (i == 0) {
						matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y - 10.0f));
					}
					else if (i == 1) {
						matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y + 10.0f));
					}
					else {
						matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
					}
				}
				move = XMVector3TransformNormal(move, matRot);
				XMFLOAT2 l_Angle;
				l_Angle.x = move.m128_f32[0];
				l_Angle.y = move.m128_f32[2];

				InterBullet* newbullet;
				newbullet = new AttackBullet();
				newbullet->SetPowerState(m_ChargeType);
				newbullet->Initialize();
				newbullet->SetPosition(viewbullet->GetPosition());
				newbullet->SetNumMag(l_ShotMag);
				newbullet->SetAngle(l_Angle);
				attackbullets.push_back(newbullet);
			}
		}
		else {			//チャージしたやつ
			XMVECTOR move2 = { 0.0f, 0.0f, 0.1f, 0.0f };
			XMMATRIX matRot2;
			matRot2 = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
			l_ShotMag = 1.0f;

			move2 = XMVector3TransformNormal(move2, matRot2);
			XMFLOAT2 l_Angle2;
			l_Angle2.x = move2.m128_f32[0];
			l_Angle2.y = move2.m128_f32[2];
			InterBullet* newbullet;
			newbullet = new AttackBullet();
			newbullet->SetPowerState(m_ChargeType);
			newbullet->Initialize();
			newbullet->SetPosition(viewbullet->GetPosition());
			newbullet->SetNumMag(l_ShotMag);
			newbullet->SetAngle(l_Angle2);
			attackbullets.push_back(newbullet);
		}
	}
	//言霊
	else if (bulletName == "Ghost") {
		//弾の生成
		GhostBullet* newbullet;
		newbullet = new GhostBullet();
		newbullet->Initialize();
		newbullet->SetPosition(viewbullet->GetPosition());
		newbullet->SetRotation({ m_Rotation.x,m_Rotation.y + 90.0f,m_Rotation.z });
		newbullet->SetBulletType(m_BulletType);
		newbullet->SetAngle(l_Angle2);
		ghostbullets.push_back(newbullet);
	}
	else {
		assert(0);
	}
}
//待機モーション
void Player::Idle()
{
	//条件少しおかしいので後で修正
	if (isShotNow)return;
	if (_animeName == AnimeName::IDLE)return;
	if (!TriggerAttack) {
		AnimationControl(AnimeName::IDLE, true, 1);
	}
}
//インターバル
void Player::InterVal() {
	Helper::GetInstance()->CheckMax(m_InterVal, 0, -1);
	Helper::GetInstance()->CheckMax(m_RigidityTime, 0, -1);
}
//プレイヤーとの当たり判定
bool Player::PlayerCollide(const XMFLOAT3& pos) {
	float l_Radius = 3.1f;//当たり範囲
	if (Collision::CircleCollision(m_Position.x, m_Position.z, l_Radius, pos.x, pos.z, l_Radius)) {
		return true;
	}
	else {
		return false;
	}

	return false;
}
//弾のリセット
void Player::ResetBullet() {
	m_InterVal = m_TargetInterVal;
	m_RigidityTime = m_TargetRigidityTime;
	viewbullet->SetAlive(false);
	viewbullet->SetCharge(false);
	m_ChargePower = {};
	m_ChargeType = POWER_NONE;
	AnimationControl(AnimeName::ATTACK, false, 1);
	TriggerAttack = true;
}
void Player::isOldPos()
{
	m_Position = OldPos;
}
//プレイヤーのダメージ判定
void Player::RecvDamage(float Damage) {
	if (m_HitPlayer) {
		Audio::GetInstance()->PlayWave("Resources/Sound/SE/Voice_Damage.wav", VolumManager::GetInstance()->GetSEVolum());
		m_HP -= Damage;
		m_DamageInterVal = 50;
		m_Damage = true;
		m_Confu = false;
		m_ConfuTimer = 0;
		BirthParticle();
	}
}
//弾の削除
void Player::BulletDelete() {
	ghostbullets.clear();
	attackbullets.clear();
	effects.clear();
	abseffect.clear();
}
//プレイヤーが敵にあたった瞬間の判定
void Player::PlayerHit(const XMFLOAT3& pos) {
	XMFLOAT2 l_Distance;
	l_Distance.x = m_Position.x - pos.x + 0.1f;
	l_Distance.y = m_Position.z - pos.z;
	m_BoundPower.x = (sin(atan2f(l_Distance.x, l_Distance.y)) * 2.0f);
	m_BoundPower.y = (cos(atan2f(l_Distance.x, l_Distance.y)) * 2.0f);
}
//弾かれる処理
void Player::ReBound() {
	if (m_Damage) {
		m_BoundPower = {
	Ease(In,Cubic,0.5f,m_BoundPower.x,0.0f),
	Ease(In,Cubic,0.5f,m_BoundPower.y,0.0f),
		};

		m_Position.x += m_BoundPower.x;
		m_Position.z += m_BoundPower.y;

		if (m_BoundPower.x == 0.0f) {
			m_Damage = false;
		}
	}
}
//銃の処理
void Player::SutoponUpdate() {
	XMVECTOR move = { 0.0f, 0.0f, 0.1f, 0.0f };
	XMMATRIX matRot;
	matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
	move = XMVector3TransformNormal(move, matRot);
	XMFLOAT2 l_Angle;
	l_Angle.x = move.m128_f32[0];
	l_Angle.y = move.m128_f32[2];
	playerattach->SetAngle(l_Angle);
	playerattach->Update();
	playerattach->SetPosition({ m_Position.x,playerattach->GetPosition().y,m_Position.z });
	playerattach->SetRotation({ m_Rotation.x,m_Rotation.y + 90.0f,m_Rotation.z });
}
//ダメージパーティクル
void Player::BirthParticle() {
	InterEffect* neweffect;
	neweffect = new BreakEffect();
	neweffect->Initialize();
	neweffect->SetPosition(m_Position);
	neweffect->SetDiviSpeed(1.0f);
	neweffect->SetLife(50);
	effects.push_back(neweffect);
}
//ボス登場シーンの更新
void Player::AppearUpdate() {
	m_viewBull = false;
	index = 15;
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->SetColor({ 0.7f,0.7f,0.7f,1.0f });
	skirtobj->FollowUpdate(skirtmat);
	SetParam();
}
//ボス撃破シーンの更新
void Player::DeathUpdate() {
	fbxmodels->StopAnimation();
	m_viewBull = false;
	m_HitPlayer = false;
	BulletDelete();
	index = 15;
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->SetColor({ 0.7f,0.7f,0.7f,1.0f });
	skirtobj->FollowUpdate(skirtmat);
	SetParam();
}
void Player::DeathUpdateAfter(int Timer) {
	if (Timer == 1) {
		AnimationControl(AnimeName::IDLE, true, 1);
		m_Position = { 30.0f,-2.0f,-45.0f };
		m_Rotation = { 0.0f,270.0f,0.0f };
		m_Color = { 1.0f,1.0f,1.0f,1.0f };
	}

	index = 15;
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->FollowUpdate(skirtmat);
	skirtobj->SetColor(m_Color);
	playerattach->DeathUpdate();
	SetParam();
}
//割合
float Player::GetPercentage() {
	float temp = m_ChargePower / 50.0f;
	Helper::GetInstance()->Clamp(temp, 0.0f, 1.0f);
	return temp;
}
//ダークコトコの登場シーン
void Player::DarkAppearUpdate(int Timer) {
	m_viewBull = false;
	if (_DarkState == DARK_SET) {
		if (Timer == 1) {
			AnimationControl(AnimeName::WALK, true, 1);
			m_Position = { 0.0f,-2.0f,-20.0f };
			_DarkState = DARK_WALK;
		}
	}
	else if (_DarkState == DARK_WALK) {
		m_Position.z += 0.2f;

		if (Helper::GetInstance()->CheckMin(m_Position.z, -6.0f, 0.025f)) {
			_DarkState = DARK_STOP;
			AnimationControl(AnimeName::IDLE, true, 1);
		}
	}
	index = 15;
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->SetColor({ 0.7f,0.7f,0.7f,1.0f });
	skirtobj->FollowUpdate(skirtmat);
	playerattach->DarkAppear(Timer);
	//基礎パラメータ設定

	//どっち使えばいいか分からなかったから保留
	SetParam();
}
//覚醒シーンの初期化
void Player::AwakeInit() {
	m_viewBull = false;
	m_Position = {0.0f,0.0f,300.0f};
	m_Rotation = { 0.0f,0.0f,0.0f };
	m_Scale = { 1.2f,0.8f,1.2f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_InterVal = 0;
	m_DamageInterVal = 0;
	m_BulletType = BULLET_FORROW;
	m_BoundPower = { 0.0f,0.0f };
	m_Confu = false;
	m_ConfuTimer = 0;
	//大きさ
	m_ChargePower = {};
	m_ChargeType = POWER_NONE;
	HungerGauge::GetInstance()->SetCatchCount(0);
	HungerGauge::GetInstance()->SetNowHunger(0.0f);
	HungerGauge::GetInstance()->SetAdditional(0.0f);
	BulletDelete();
	AnimationControl(AnimeName::IDLE, true, 1);
}
//ラスボス登場
void Player::LastAppearUpdate(int Timer) {
	m_viewBull = false;
	if (_LastState == LAST_SET) {
		if (Timer == 1) {
			AnimationControl(AnimeName::WALK, true, 1);
			m_Position = { 3.0f,-2.0f,-40.0f };
			_LastState = LAST_WALK;
		}
	}
	else if (_LastState == LAST_WALK) {
		m_Position.z += 0.2f;
		
		if (Helper::GetInstance()->CheckMin(m_Position.z, 3.0f, 0.025f)) {
			_LastState = LAST_STOP;
			AnimationControl(AnimeName::IDLE, true, 1);
		}
	}
	else if(_LastState == LAST_SECOND_WALK) {
		if (Helper::GetInstance()->CheckMin(m_Position.z, 15.0f, 0.025f)) {
			_LastState = LAST_STOP;
			AnimationControl(AnimeName::IDLE, true, 1);
		}
	}
	else {
	}
	index = 15;
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->SetColor({ 0.7f,0.7f,0.7f,1.0f });
	skirtobj->FollowUpdate(skirtmat);
	playerattach->AppearUpdate(Timer);
	//基礎パラメータ設定

	//どっち使えばいいか分からなかったから保留
	SetParam();
}
//ラスボス撃破
void Player::LastDeadUpdate(int Timer) {
	m_viewBull = false;
	BulletDelete();
	if (_LastEndState == LAST_END_SET) {
		if (Timer == 1) {
			m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			m_Position = { 5.0f,0.0f,-10.0f };
			m_Rotation = { 0.0f,0.0f,0.0f };
			AnimationControl(AnimeName::IDLE, true, 1);
		}
		else if (Timer == 920) {
			_LastEndState = LAST_END_WALK;
			AnimationControl(AnimeName::WALK, true, 1);
		}
	}
	else if (_LastEndState == LAST_END_WALK) {
	
		if (Helper::GetInstance()->CheckMin(m_Position.z, 5.0f, 0.4f)) {
			_LastEndState = LAST_END_DIR;
			AnimationControl(AnimeName::IDLE, true, 1);
		}
	}
	else {
		Helper::GetInstance()->CheckMax(m_Rotation.y, -45.0f, -2.0f);
		//if () {
	}
	index = 15;
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->SetColor({ 0.7f,0.7f,0.7f,1.0f });
	skirtobj->FollowUpdate(skirtmat);
	playerattach->LastDeadUpdate(Timer);
	SetParam();
}
void Player::EndRollUpdate(int Timer) {
	m_viewBull = false;
	const float l_AddPosX = 0.2f;
	const float l_AddRotY = 2.0f;
	if (_EndState == END_SET) {
		if (Timer == 1) {
			m_Scale = { 0.8f,0.4f,0.8f };
			m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			m_Position = { 20.0f,0.0f,-25.0f };
			m_Rotation = { 0.0f,270.0f,0.0f };
			AnimationControl(AnimeName::WALK, true, 1);
			_EndState = END_WALK;
			//
		}
	}
	else if (_EndState == END_WALK) {
		if (Helper::GetInstance()->CheckMax(m_Position.x, 0.0f, -l_AddPosX)) {
			_EndState = END_DIR_CAMERA;
			AnimationControl(AnimeName::IDLE, true, 1);
		}
	}
	else if (_EndState == END_DIR_CAMERA) {
		Helper::GetInstance()->CheckMax(m_Rotation.y, 180.0f, -l_AddRotY);
		if (Timer == 310) {
			_EndState = END_DIR_RIGHT;
		}
	}
	else if (_EndState == END_DIR_RIGHT) {
		m_Rotation.y = Ease(In, Cubic, 0.5f, m_Rotation.y, 90.0f);
		if (Timer == 400 || Timer == 630) {
			_EndState = END_DIR_LEFT;
		}
		else if (Timer == 900) {
			_EndState = END_DIR_BACK;
		}
	}
	else if(_EndState == END_DIR_LEFT) {
		m_Rotation.y = Ease(In, Cubic, 0.5f, m_Rotation.y, 270.0f);
		if (Timer == 500 || Timer == 730) {
			_EndState = END_DIR_RIGHT;
		}
	}
	else if (_EndState == END_DIR_BACK) {
		m_Rotation.y = Ease(In, Cubic, 0.5f, m_Rotation.y, 0.0f);
		if (Timer == 1000) {
			_EndState = END_DIR_FRONT;
		}
	}
	else {
		m_Rotation.y = Ease(In, Cubic, 0.5f, m_Rotation.y, 180.0f);
	}

	if (Timer == 1670) {
		fbxmodels->StopAnimation();
	}
	index = 15;
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->FollowUpdate(skirtmat);
	playerattach->EndRollUpdate(Timer);
	SetParam();
}
//パラメーターセット
void Player::SetParam() {
	fbxmodels->SetPosition(m_Position);
	fbxmodels->SetRotation(m_Rotation);
	fbxmodels->SetScale(m_Scale);
	fbxmodels->SetColor(m_Color);
	fbxmodels->Update(m_LoopFlag, m_AnimationSpeed, m_StopFlag);
}
//パーティクル
void Player::BirthAbs(const XMFLOAT4& color) {
	for (int i = 0; i < 20;i++) {
		//ノーツの発生
		AbsorptionEffect* neweffect;
		neweffect = new AbsorptionEffect();
		neweffect->Initialize();
		neweffect->SetBasePos(m_Position);
		neweffect->SetColor(color);
		neweffect->SetAddFrame(0.05f);
		abseffect.push_back(neweffect);
	}
}
//タイトルの更新
void Player::TitleUpdate() {
	AnimationControl(AnimeName::IDLE, true, 1);
	m_Position = { 0.0f,-2.0f,-12.0f };
	m_Rotation = { 0.0f,180.0f,0.0f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_Scale = { 0.6f,0.4f,0.6f };

	index = 15;
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->FollowUpdate(skirtmat);
	skirtobj->SetColor(m_Color);

	playerattach->TitleUpdate();
	SetParam();
}

void Player::TyutorialUpdate()
{
	fbxmodels->GetBoneIndexMat(index, skirtmat);
	skirtobj->FollowUpdate(skirtmat);
	skirtobj->SetColor(m_Color);
	SetParam();
	display = false;
}
