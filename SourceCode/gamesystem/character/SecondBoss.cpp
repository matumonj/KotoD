#include "SecondBoss.h"
#include <any>
#include <random>
#include "Collision.h"
#include "CsvLoader.h"
#include "ImageManager.h"
#include "Helper.h"
#include "Easing.h"
//生成
SecondBoss::SecondBoss() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Tyuta);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);
}

bool SecondBoss::Initialize() {
	m_Position = { 0.0f,0.0f,30.0f };
	m_Scale = { 1.0f,1.4f,1.0f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_Rotation.y = -90.f;
	RTime = 1;
	m_Position.x = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Second/Secondboss.csv", "pos")));
	m_Magnification = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Second/Secondboss.csv", "Magnification")));
	m_HP = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Second/Secondboss.csv", "hp1")));
	m_BirthTarget = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Second/Secondboss.csv", "HeartTarget")));
	m_MaxHp = m_HP;
	MoveCount = 1;
	_phaseN = Phase_Normal::NON;
	_normal.Initialize();
	_cattack.Initialize();
	shake = new Shake();

	ActionTimer = 1;

	damageara.reset(IKETexture::Create(ImageManager::DAMAGEAREA, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	damageara->TextureCreate();
	m_Radius = 8.0f;

	//優先度
	CirclePriority = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Second/Secondboss.csv", "CPriority")));
	SummonPriority = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Second/Secondboss.csv", "SPriority")));
	//攻撃クールタイム
	S_DecisionCount = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Second/Secondboss.csv", "DecisionCount")));
	//落下攻撃　下げ時間
	ImpactDownMove = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Second/Secondboss.csv", "DownVal")));
	_charge.SetDownMoveVal(ImpactDownMove);

	//
	SummonCool = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Second/Secondboss.csv", "SummonCool")));

	effects.clear();
	return true;
}

void SecondBoss::SkipInitialize() {

}
//行動
void SecondBoss::Action() {
	//InterAction = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/second/Secondboss.csv", "inter")));

	//DamageNormal= static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/second/Secondboss.csv", "NormalDamage")));
//	Damage_Circle= static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/second/Secondboss.csv", "impactDamage")));
	if (this == nullptr)return;
	if (m_HP <= 0.0f) return;

	{
		/*^^^^^^上下運動^^^^^^^*/
		float OldsMov = 0;
		if (!_cattack.GetAttackF() && !_normal.GetAttackF()) {
			PosYMovingT++;
			YmovEaseT = 0.f;
			OldsMov = PosYMovingT;
		}
		else
		{
			YmovEaseT += 0.03f;
			PosYMovingT = Easing::EaseOut(YmovEaseT, OldsMov, 0.f);
		}

		Helper::GetInstance()->Clamp(YmovEaseT, 0.f, 1.f);

		if (YmovEaseT <= 1.f)
			m_Position.y = 20.f + sinf(PI * 2.f / 120.f * PosYMovingT) * -5.f;

		/*^^^^^^^^^^^^^^^^^^*/


		/*クルッと回るやつ　タイマー*/
		if (RTime % 100 == 0) { isRot = true; }
		RTime++;

		/*^^^^^^^^^^^^^^^^^^^^^*/

		/*^^^^当たり判定^^^^*/
		ColPlayer();
		/*^^^^^^^^^^^^^^^^^*/


		/*^^^^^攻撃判定^^^^^*/
		//非戦闘時の動き
		if (!EndSummonRepos || !ReturnPosF_Impact)
		{
			OldPos_EndSummon = m_Position;
		}

		if (SummobnStop)m_Rotation.y += 10.f;

		EndSumon_returnPos(EndSummonRepos, RePosEaseT);
		EndSumon_returnPos(ReturnPosF_Impact, RePosEaseT_Impact);

		//範囲攻撃のフェーズが最後まで行ったらプレイヤーの方に戻る
		_cattack.ReturnPosJudg(ReturnPosF_Impact);

		bool noAction = (!_normal.GetAttackF() && !_cattack.GetAttackF() && !SummobnStop && !EndSummonRepos);
		if (!BattleStartF) {
			noBattleCount = 0;
			NoBattleMove();
		}
		else {
			RotEaseTime_noBat = 0.f;
			EaseT_BatStart = 0.f;
			//タイマーカウンタ
			AttackDecision();
			if (!SummobnStop) {
				//通常移動（円運動）
				Move();
			}
			//通常攻撃
			_normal.Update(m_Position, m_Rotation, EncF);
			//ため攻撃
			_cattack.Update(m_Position, m_Rotation);


			Move_Away(); _normal.Remove(m_Position, m_Scale, EncF); DamAction();
		}
		/*^^^^^^^^^^^^^^^^^^^*/
		_normal.SetreposAngle();
		/*^^^^^^^^^^^^^^^^^^^^^*/
	/*^^^^当たり判定^^^^*/
	//弾とボスの当たり判定
		vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
		if (m_Scale.x >= 0.9f || m_Scale.y >= 0.9f || m_Scale.z >= 0.9f)
		//if(SPosMoveEaseT<=0.f|| SPosMoveEaseT>=1.f)
		CollideBul(_playerBulA, Type::CIRCLE);
	}
	//OBJのステータスのセット
	Obj_SetParam();

	if (_normal.GetViewDAreaF()) {
		texAlpha += 0.05f;
		damageara->SetPosition(texpos);
	}
	else {
		texAlpha -= 0.05f;
	}


	Helper::GetInstance()->Clamp(texAlpha, 0.f, 1.f);

	constexpr float AnchorY = 0.3f;

	texpos = { m_Position.x,2.f,m_Position.z };
	cinter += 0.018f;
	if (cinter > 1.2f)cinter = AnchorY;
	damageara->SetCinter(cinter);
	damageara->SetRotation({ 90,m_Rotation.y - 90,0
		});
	damageara->SetScale({ 2.f,12.f,1.f });
	damageara->SetColor({ 1,1,1,texAlpha });
	damageara->SetClipF(true);
	damageara->Update();

	//リミット制限
	Helper::GetInstance()->Clamp(m_Position.x, -55.0f, 65.0f);
	Helper::GetInstance()->Clamp(m_Position.z, -60.0f, 60.0f);
}
//ポーズ
void SecondBoss::Pause() {


}
void SecondBoss::NormalAttak::Update(XMFLOAT3& Pos, XMFLOAT3& Rot, bool& Enf)
{
	switch (_phaseN)
	{
	case Phase_Normal::ROTPLAYER_0:

		Rot.z -= RotSpeed;
		NormalAttak::ShakeAction(Pos, Rot);
		break;
	case Phase_Normal::PHASE_ONE:

		Rot.z -= RotSpeed;
		NormalAttak::Attack(Pos, Rot);
		break;
	case Phase_Normal::ROTPLAYER_1:

		Rot.z -= RotSpeed;
		NormalAttak::ShakeAction(Pos, Rot);
		break;
	case Phase_Normal::PHASE_TWO:

		Rot.z -= RotSpeed;
		NormalAttak::Attack(Pos, Rot);
		break;
	case Phase_Normal::ROTPLAYER_2:

		Rot.z -= RotSpeed;
		NormalAttak::ShakeAction(Pos, Rot);
		break;
	case Phase_Normal::PHASE_THREE:

		Rot.z -= RotSpeed;
		NormalAttak::Attack(Pos, Rot);
		break;
	case Phase_Normal::ROTPLAYER_3:

		Rot.z -= RotSpeed;
		NormalAttak::ShakeAction(Pos, Rot);
		break;
	case Phase_Normal::NON:
	{
		RushRotationF = false;
		BeforeShakePos = Pos;
		RemovePosEaseT = 0.f;
		BackSpeed = 4.f;
		shake->SetShakeTimer(0); shake->SetShakeStart(false);
		shakeend = false;
		RotEaseTime = 0.f;
		Enf = true;
		StayF = false;
		RushMoveEaseT = 0.f;
		StayCount = 0;
	}
	if (NormalAttackF) {
		BackSpeed = 4.f;
		RotEaseTime = 0.f;
		RushRotationF = true;
		RushOldPos = Pos;
		_phaseN = Phase_Normal::ROTPLAYER_0;
	}
	break;

	case Phase_Normal::STIFF:
		NormalAttak::Idle(Pos, Rot, Enf);
		break;
	}
}

void SecondBoss::EffecttexDraw(DirectXCommon* dxCommon)
{
	if (m_HP <= 0.0f)return;

	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	_cattack.Draw();
	IKETexture::PostDraw();
}
//描画
void SecondBoss::Draw(DirectXCommon* dxCommon) {

	Obj_Draw();
	EffecttexDraw(dxCommon);
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	//if (m_Alive) {
	if (m_HP > 0.0f) {
		damageara->Draw();
	}
	///
	IKETexture::PostDraw();

}

void SecondBoss::Rot()
{
	if (_normal.GetAttackF())return;
	//if (!isRot)return;
	if (isRot) {
		RotEaseT += 0.03f;

		m_Rotation.y = Easing::EaseOut(RotEaseT, 180.f, 180.f + 360.f);

		if (RotEaseT >= 1.f)
		{
			isRot = false;
		}
	}
	else {
		//	m_Rotation.y = 180.f;
		RotEaseT = 0.f;
	}

}


void SecondBoss::Move()
{

	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();

	if ((BattleStartF && !EndSummonRepos && !ReturnPosF_Impact) && !_normal.GetAttackF() && !_cattack.GetAttackF() && !SummobnStop) {

		//角度の取得 プレイヤーが敵の索敵位置に入ったら向きをプレイヤーの方に
		XMVECTOR PositionA = { l_player.x,l_player.y,l_player.z };
		XMVECTOR PositionB = { m_Position.x,m_Position.y,m_Position.z };
		//プレイヤーと敵のベクトルの長さ(差)を求める
		XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;
		//回転軸をプレイヤーの方に
			//向きかえる

		float RotY = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);

		m_Rotation.y = RotY * 60.f + 90.f;


		BossAngle++;

		m_Position.x = l_player.x + sinf(BossAngle * (PI / 180.0f)) * 20.0f;
		m_Position.z = l_player.z + cosf(BossAngle * (PI / 180.0f)) * 20.0f;

	}

}


void SecondBoss::Move_Away()
{

}

void SecondBoss::NormalAttak::Idle(XMFLOAT3& Pos, XMFLOAT3 Rot, bool& Enf)
{

	StayCount++;
	if (StayCount >= 50)
	{
		RushRotationF = true;
		NormalAttak::Rot(Pos, Rot);
		if (RotEaseTime >= 1.f)
		{
			Enf = false;
			if (Enf) {
				NormalAttackF = false;
				_phaseN = Phase_Normal::NON;
			}
		}
	}
	else
	{
		StayF = true;
	}
}

void SecondBoss::NormalAttak::Attack(XMFLOAT3& Pos, XMFLOAT3& Rot)
{
	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();

	RushRotationF = false;
	BeforeShakePos = Pos;
	RemovePosEaseT = 0.f;
	BackSpeed = 4.f;
	shake->SetShakeTimer(0); shake->SetShakeStart(false);
	shakeend = false;
	RotEaseTime = 0.f;

	ViewDAreaF = false;
	m_move = { 0.f,0.f, 0.1f, 0.0f };
	m_matRot = XMMatrixRotationY(XMConvertToRadians(Rot.y + 90.f));
	m_move = XMVector3TransformNormal(m_move, m_matRot);
	RushMoveEaseT += 0.05f;
	Pos.x = Easing::EaseOut(RushMoveEaseT, RushOldPos.x, RushOldPos.x + m_move.m128_f32[0] * 600.f);
	Pos.z = Easing::EaseOut(RushMoveEaseT, RushOldPos.z, RushOldPos.z + m_move.m128_f32[2] * 600.f);


	if (RushMoveEaseT >= 1.f)
	{
		RotSpeed = 0.f;
		Rot.z = 0;
		RushRotationF = true;

		if (_phaseN == Phase_Normal::PHASE_ONE)_phaseN = Phase_Normal::ROTPLAYER_1;
		if (_phaseN == Phase_Normal::PHASE_TWO)_phaseN = Phase_Normal::ROTPLAYER_2;
		if (_phaseN == Phase_Normal::PHASE_THREE)_phaseN = Phase_Normal::STIFF;
	}
	RushOldRotY = Rot.y;

	Helper::GetInstance()->Clamp(RushMoveEaseT, 0.f, 1.f);
	Helper::GetInstance()->Clamp(BackSpeed, 0.f, 8.f);
	Helper::GetInstance()->Clamp(RotEaseTime, 0.f, 1.f);
}

void SecondBoss::NormalAttak::ShakeAction(XMFLOAT3& Pos, XMFLOAT3& Rot)
{
	//初期化部
	{
		//RotSpeed = 0.f;
		RushMoveEaseT = 0.f;
	}
	XMVECTOR move = { 0.f,0.f, 0.1f, 0.0f };

	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(Rot.y + 270.f));

	move = XMVector3TransformNormal(move, matRot);

	const int MaxShakeTime = 60;
	if (RotSpeed > 2.f)ViewDAreaF = true;
	if (RotEaseTime >= 1.f) {

		RotSpeed += 0.1f;
		if (RotSpeed >= 6.0f && !shakeend) {
			RemovePosEaseT += 0.05f;
			if (RemovePosEaseT >= 1.f) {
				shakeend = true;
			}
		}



		if (shakeend)
		{
			//Rot.z = 0.f;
			BackSpeed -= 0.25f;
			Pos.x = Pos.x + move.m128_f32[0] * BackSpeed;
			Pos.z = Pos.z + move.m128_f32[2] * BackSpeed;
		}
		else
		{

			XMVECTOR PositionA = { Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().y, Player::GetInstance()->GetPosition().z };
			XMVECTOR PositionB = { Pos.x,Pos.y,Pos.z };
			//プレイヤーと敵のベクトルの長さ(差)を求める
			XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA);
			float RotY = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);
			if (RotSpeed < 7.f)
				Rot.y = RotY * 60.f + 90.f;
		}

		//攻撃状態に遷移
		if (BackSpeed <= 0.f)
		{
			RushOldPos = Pos;
			if (_phaseN == Phase_Normal::ROTPLAYER_0)_phaseN = Phase_Normal::PHASE_ONE;
			if (_phaseN == Phase_Normal::ROTPLAYER_1)_phaseN = Phase_Normal::PHASE_TWO;
			if (_phaseN == Phase_Normal::ROTPLAYER_2)_phaseN = Phase_Normal::PHASE_THREE;
			///if (_phaseN == Phase_Normal::ROTPLAYER_2)_phaseN = Phase_Normal::PHASE_THREE;
		}
	}
	else
	{
		NormalAttak::Rot(Pos, Rot);
	}

	//if (Collision::CircleCollision(Pos.x, Pos.z, 1.f, Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().z, 3.f))
	//{
	//	Player::GetInstance()->isOldPos();
	//}
}

void SecondBoss::NormalAttak::Rot(XMFLOAT3& Pos, XMFLOAT3& Rot)
{
	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();
	//角度の取得 プレイヤーが敵の索敵位置に入ったら向きをプレイヤーの方に
	XMVECTOR PositionA = { l_player.x,l_player.y,l_player.z };
	XMVECTOR PositionB = { Pos.x,Pos.y,Pos.z };
	//プレイヤーと敵のベクトルの長さ(差)を求める
	XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;

	if (!RushRotationF)
	{
		RotEaseTime = 0.f;
		RushOldRotY = Rot.y;
	}
	else {

		float RotY = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);

		//イージングカウンタ＋＋
		RotEaseTime += 0.02f;

		//Rotation反映
//		Rot.y = Easing::EaseOut(RotEaseTime, RushOldRotY, RotY * 60.f + 90.f);


	}
}

void SecondBoss::DebRot()
{
	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();
	if (!RushRotationF)
	{
		RotEaseTime = 0.f;
		RushOldRotY = m_Rotation.y;
	}
	else {
		//角度の取得 プレイヤーが敵の索敵位置に入ったら向きをプレイヤーの方に
		XMVECTOR PositionA = { l_player.x,l_player.y,l_player.z };
		XMVECTOR PositionB = { m_Position.x,m_Position.y,m_Position.z };
		//プレイヤーと敵のベクトルの長さ(差)を求める
		XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;
		//回転軸をプレイヤーの方に
			//向きかえる

		float RotY = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);

		//イージングカウンタ＋＋
		RotEaseTime += 0.02f;

		//Rotation反映
		m_Rotation.y = Easing::EaseOut(RotEaseTime, RushOldRotY, RotY * 60.f + 90.f);


	}
}

void SecondBoss::DamAction()
{
	if (Recv && ColChangeEaseT <= 0.f)
	{
		DamColSetF = true;
	}
	if (DamColSetF)
	{
		ColChangeEaseT += 0.03f;
		if (ColChangeEaseT >= 1.f)
		{
			Recv = false;
		}
	}

	//被ダメージ時
	if (!Recv)
	{
		DamColSetF = false;
		ColChangeEaseT -= 0.03f;
	}

	m_Color.y = Easing::EaseOut(ColChangeEaseT, 0.9f, 0.2f);
	m_Color.z = Easing::EaseOut(ColChangeEaseT, 0.9f, 0.2f);
	m_Color.x = 1.f;
	m_Color.w = 1.f;

	Helper::GetInstance()->Clamp(ColChangeEaseT, 0.f, 1.f);

}

void SecondBoss::NormalAttak::Initialize()
{
	shake = new Shake();
}

void SecondBoss::NormalAttak::Remove(XMFLOAT3& Pos, XMFLOAT3& Scl, bool Enf)
{
	if (!Enf) {
		SPosMoveEaseT += 0.05f;

		if (SPosMoveEaseT >= 1.f)
		{
			Pos = RotStartPos;
			NormalAttackF = false;
			_phaseN = Phase_Normal::NON;
			//EncF = true;
		}
	}
	if (Enf)
	{
		SPosMoveEaseT -= 0.05f;
		OldPos_Remove = Pos;
	}

	Scl.x = Easing::EaseOut(SPosMoveEaseT, 1.0f, 0.f);
	Scl.y = Easing::EaseOut(SPosMoveEaseT-0.05f, 1.0f, 0.f);
	Scl.z = Easing::EaseOut(SPosMoveEaseT-0.05f, 1.0f, 0.f);

	Helper::GetInstance()->Clamp(SPosMoveEaseT, 0.f, 1.f);
	Helper::GetInstance()->Clamp(Scl.x, 0.f, 1.0f);
	Helper::GetInstance()->Clamp(Scl.y, 0.f, 1.0f);
	Helper::GetInstance()->Clamp(Scl.z, 0.f, 1.0f);
}

void SecondBoss::RemovePos()
{
	if (!EncF) {
		SPosMoveEaseT += 0.05f;

		if (SPosMoveEaseT >= 1.f)
		{
			m_Position = RotStartPos;
			_normal.SetNormalAttackF(false);
			_phaseN = Phase_Normal::NON;
			//EncF = true;
		}
	}
	if (EncF)
	{
		SPosMoveEaseT -= 0.05f;
		OldPos_Remove = m_Position;
	}

	m_Scale.x = Easing::EaseOut(SPosMoveEaseT, 1.5f, 0.f);
	m_Scale.y = Easing::EaseOut(SPosMoveEaseT, 1.5f, 0.f);
	m_Scale.z = Easing::EaseOut(SPosMoveEaseT, 1.5f, 0.f);

	Helper::GetInstance()->Clamp(SPosMoveEaseT, 0.f, 1.f);
	Helper::GetInstance()->Clamp(m_Scale.x, 0.f, 1.5f);
	Helper::GetInstance()->Clamp(m_Scale.y, 0.f, 1.5f);
	Helper::GetInstance()->Clamp(m_Scale.z, 0.f, 1.5f);
}

void SecondBoss::NoBattleMove()
{
	XMVECTOR move = { 0.f,0.f, 0.1f, 0.0f };

	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y + 90.f));

	move = XMVector3TransformNormal(move, matRot);

	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();

	constexpr float MoveSpeed = 4.f;

	constexpr float EaseAddVal = 0.04f;


	//角度の取得 プレイヤーが敵の索敵位置に入ったら向きをプレイヤーの方に
	XMVECTOR PositionA = { l_player.x + sinf(1 * (PI / 180.0f)) * 20.0f,l_player.y, l_player.z + cosf(1 * (PI / 180.0f)) * 20.0f };
	XMVECTOR PositionB = { m_Position.x,m_Position.y,m_Position.z };
	//プレイヤーと敵のベクトルの長さ(差)を求める
	XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;

	if (!SearchF) {
		if (MoveCount % 90 == 0) {
			YRotRandVal = m_Rotation.y + static_cast<float>(rand() % 90 + 45);
			RotChange = true;
		}
		else {
			if (!RotChange)
				YRotOld = m_Rotation.y;
		}

		if (RotChange) {
			MoveCount = 1;
			RotEaseTime_noBat += EaseAddVal;

			m_Rotation.y = Easing::EaseOut(RotEaseTime_noBat, YRotOld, YRotRandVal);
			if (RotEaseTime_noBat >= 1.f)
			{
				RotChange = false;
			}


		}

		else {
			MoveCount++;
			RotEaseTime_noBat = 0.f;
			m_Position.x = m_Position.x + move.m128_f32[0] * MoveSpeed;
			m_Position.z = m_Position.z + move.m128_f32[2] * MoveSpeed;
		}

		OldPos = m_Position;

	}
	if (SearchF) {
		EaseT_BatStart += 0.02f;
		float RotY = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);
		m_Color = { 0,1,0,1 };
		if (EaseT_BatStart <= 0.1f) {
			m_Rotation.y = RotY * 60.f + 90.f;
		}

		//円運動の初期位置に
		m_Position.x = Easing::EaseOut(EaseT_BatStart, OldPos.x, l_player.x + sinf(1 * (PI / 180.0f)) * 20.0f);
		m_Position.z = Easing::EaseOut(EaseT_BatStart, OldPos.z, l_player.z + cosf(1 * (PI / 180.0f)) * 20.0f);

		if (EaseT_BatStart >= 1.f)
		{
			RushRotationF = true;
			DebRot();
			if (RotEaseTime >= 1.f)
			{
				RushRotationF = false;
				m_Color = { 1,1,1,1 };
				BossAngle = 1;
				BattleStartF = true;
			}
		}
	}

	Helper::GetInstance()->Clamp(RotEaseTime_noBat, 0.f, 1.f);
	Helper::GetInstance()->Clamp(EaseT_BatStart, 0.f, 1.f);
}

void SecondBoss::ColPlayer_Def()
{
	if (RushMoveEaseT <= 0.f && Collision::CircleCollision(m_Position.x, m_Position.z, 5.f, Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().z, 1.f))
	{
		//Player::GetInstance()->isOldPos();
	}
}
//ImGui
void SecondBoss::ImGui_Origin() {
	//ImGui::Begin("Second");
	//ImGui::Text("HP:%f", m_HP);
	//ImGui::End();
}



void SecondBoss::ChargeAttack::Initialize()
{
	impacttex[0].reset(IKETexture::Create(ImageManager::IMPACT2, { 0,0,0 }, { 10,10,10 }, { 1,1,1,1 }));
	impacttex[1].reset(IKETexture::Create(ImageManager::IMPACT, { 0,0,0 }, { 0,0,0 }, { 1,1,1,1 }));

	//テクスチャ初期化
	for (auto i = 0; i < impacttex.size(); i++) {
		impacttex[i]->TextureCreate();
		texAlpha[i] = 1.f;
		texScl[i] = { 0.f,0.f };
	}
	shake = new Shake();
}

#define MaxShakeTime 90
void SecondBoss::ChargeAttack::Attack(XMFLOAT3& Pos, XMFLOAT3& Rot)
{
	switch (_phase)
	{
	case Phase_Charge::NON:
		for (auto i = 0; i < impacttex.size(); i++)
		{
			texAlpha[i] = 1.f;
			texScl[i] = { 0.f,0.f };
		}

		ChargeTime = 0;
		if (AttackF)_phase = Phase_Charge::CHARGE;
		break;

	case Phase_Charge::CHARGE:
		if (shake->GetShakeTimer() < MaxShakeTime) {

			shake->SetShakeStart(true);
			shake->ShakePos(shakeX, 1, -1, MaxShakeTime, 12.f);
			shake->ShakePos(shakeZ, 1, -1, MaxShakeTime, 12.f);

			////シェイク値足す
			Pos.x += shakeX;
			Pos.z += shakeZ;
		}

		JFrame = 0.f;
		if (shake->GetShakeTimer() >= MaxShakeTime) {
			shake->SetShakeStart(false);
			_phase = Phase_Charge::JUMP;
		}
		break;
	case Phase_Charge::JUMP:
		shake->SetShakeTimer(0);

		JumpAction(Pos, Rot);
		break;
	case Phase_Charge::ATTACK:
		//テクスチャ広がるやつ
		TexScling(Rot);
		break;

	}
}

void SecondBoss::ChargeAttack::ReturnPosJudg(bool& reposf)
{
	if (_phase != Phase_Charge::END)return;
	//終了時にフラグとフェーズ初期化
	AttackF = false;
	ChargeTime = 0;
	RotSpeed = 0.f;
	reposf = true;
	_phase = Phase_Charge::NON;
}

void SecondBoss::ChargeAttack::JumpAction(XMFLOAT3& Pos, XMFLOAT3& Rot)
{
	XMFLOAT3 PPos = Player::GetInstance()->GetPosition();

	float SubPower = 0.001f;
	//落下の緩急
	constexpr float Distortion = 3.f;
	//地面の高さ
	constexpr float GroundY = 10.f;
	//ジャンプ高さ
	constexpr float Height = 10.f;

	//ドッスン挙動
	JFrame += 1.f / 60.f;
	Pos.y = GroundY + (1.0f - pow(1.0f - sin(PI * JFrame), Distortion)) * Height;

	if (JFrame >= 1.f)
	{
		if (Collision::CircleCollision(PPos.x, PPos.z, 2.f, Pos.x, Pos.z, 5.f) && Player::GetInstance()->GetDamageInterVal() == 0)
		{
			Player::GetInstance()->RecvDamage(2.0f);
		}

		_phase = Phase_Charge::ATTACK;
	}

	if (JFrame >= 1.0f / 2.0f)
	{
		Rot.z -= 5.0f;
	}
	Helper::GetInstance()->Clamp(Rot.z, -90.f, 0.f);
	Helper::GetInstance()->Clamp(JFrame, 0.f, 1.f);
	Helper::GetInstance()->Clamp(JFrame, 0.f, 1.f);
}


void SecondBoss::ChargeAttack::TexScling(XMFLOAT3& Rot)
{
	Rot.z += 5.0f;
	Helper::GetInstance()->Clamp(Rot.x, -90.f, 0.f);

	constexpr float AddScling = 0.08f;
	bool flagOff = texAlpha[0] < 0.f && texAlpha[1] < 0.f;

	texScl[0].x += AddScling;
	texScl[0].y += AddScling;

	if (texScl[0].x > 2.f)
	{
		texScl[1].x += AddScling;
		texScl[1].y += AddScling;
	}

	for (auto i = 0; i < impacttex.size(); i++)
	{
		if (texScl[i].x > AddScling)texAlpha[i] -= 0.015f;
	}

	if (flagOff)_phase = Phase_Charge::END;
}

void SecondBoss::ChargeAttack::Update(XMFLOAT3& Pos, XMFLOAT3& Rot)
{
	constexpr float PosYVal = 8.f;

	for (auto i = 0; i < impacttex.size(); i++)
	{
		impacttex[i]->SetPosition({ Pos.x,PosYVal,Pos.z });
		impacttex[i]->SetIsBillboard(false);
	}

	if (!AttackF)
	{
		for (auto i = 0; i < 2; i++)
		{
			texScl[i] = { 0.f,0.f };
			texAlpha[i] = 1.f;
		}
	}

	Attack(Pos, Rot);

	//パラメータ更新
	for (auto i = 0; i < 2; i++)
	{
		impacttex[i]->SetScale({ texScl[i].x,texScl[i].y,5.f });
		impacttex[i]->SetRotation({ 90,0,0 });
		impacttex[i]->SetColor({ 1,1,1,texAlpha[i] });
		impacttex[i]->Update();
	}
}


void SecondBoss::ChargeAttack::Draw()
{
	for (auto i = 0; i < impacttex.size(); i++)
	{
		impacttex[i]->Draw();
	}
}

void SecondBoss::EndSumon_returnPos(bool& f, float& easespeed)
{
	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();
	bool AddSpeed_Early = Collision::CircleCollision(m_Position.x, m_Position.z, 5.f, l_player.x, l_player.z, 1.f);
	bool AddSpeed_Low = !Collision::CircleCollision(m_Position.x, m_Position.z, 5.f, l_player.x, l_player.z, 1.f);

	float AddEaseTime = 0.02f;

	if (AddSpeed_Early)AddEaseTime = 0.04f;
	if (AddSpeed_Low)AddEaseTime = 0.01f;

	XMVECTOR PositionA = { l_player.x,l_player.y,l_player.z };
	XMVECTOR PositionB = { m_Position.x,m_Position.y,m_Position.z };
	//プレイヤーと敵のベクトルの長さ(差)を求める
	XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;

	float RotY = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);

	if (f)
	{
		m_Rotation.y = RotY * 60.f + 90.f;
		easespeed += AddEaseTime;
		m_Position.x = Easing::EaseOut(easespeed, OldPos_EndSummon.x, Player::GetInstance()->GetPosition().x + sinf(BossAngle * (PI / 180.0f)) * 20.0f);
		m_Position.z = Easing::EaseOut(easespeed, OldPos_EndSummon.z, Player::GetInstance()->GetPosition().z + cosf(BossAngle * (PI / 180.0f)) * 20.0f);

		if (easespeed >= 1.f)
		{
			f = false;
		}
	}
	else
	{
		easespeed = 0.f;
	}
}

//ボス登場シーン
void SecondBoss::AppearAction() {

}
//ボス撃破シーン
void SecondBoss::DeadAction_Throw() {
	m_Scale = { 1.0f,1.4f,1.0f };
	m_Color = { 1,1,1,1 };
	//m_Position = { 0,40,20.f };
	if (!ResetRota) {
		m_Rotation.y = 90.f;
		m_Rotation.x = 0.f;
		m_Rotation.z = 0.f;
		ResetRota = true;
	}
	else
	{
		//m_Position.y = 40;
		m_Rotation.y += 0.02f;
		m_Rotation.z += 0.09f;
	}
	RotFrontSpeed = 3.f;
	//Player::GetInstance()->SetPosition({ 0,0,10 });
	Obj_SetParam();
}
//ボス撃破シーン
void SecondBoss::DeadAction() {

	constexpr int ShakeTimer = 250;

	if (shake->GetShakeTimer() >= ShakeTimer - 5) {
		shake->SetShakeStart(false);
		m_Position.y -= 0.5f;
		if (m_Position.y <= 5.f)
			DeathEffectF = true;
		m_Rotation.y += 0.06f;
		DeathSpeed += 0.14f;
		m_Rotation.z += DeathSpeed;
		if (DeathSpeed >= 3.f)
		{
			//m_Rotation.y += 0.4f;
			m_Rotation.z += 1.f;
		}
	}
	else
	{
		shake->SetShakeStart(true);
		shake->ShakePos(ShakePos.x, 5, -5, ShakeTimer, 10);
		shake->ShakePos(ShakePos.z, 5, -5, ShakeTimer, 10);
		m_Position.x += ShakePos.x / 2.f;
		m_Position.z += ShakePos.z / 2.f;
		SinRotCount += 3.f;
		m_Rotation.y = 90 + sin(PI * 2 / 240 * SinRotCount) * 40;
		m_Rotation.z -= RotFrontSpeed;
		RotFrontSpeed -= 0.04f;

		//シェイクを止める
		if (!shake->GetShakeStart()) {
			ShakePos = { 0.0f,0.0f,0.0f };
		}
		DeathSpeed = 0.f;
	}
	if (m_Position.y <= 0)
		DeathEffect();

	Obj_SetParam();

	//Player::GetInstance()->SetPosition({ 0,0,0 });
	Helper::GetInstance()->Clamp(m_Rotation.z, -90.f, 90.f);
	Helper::GetInstance()->Clamp(DeathSpeed, 0.f, 3.f);
	Helper::GetInstance()->Clamp(RotFrontSpeed, 0.f, 2.f);
}

void SecondBoss::AttackDecision()
{
	//攻撃hラグ
	if (ActionTimer % S_DecisionCount == 0)
		Active = true;


	if (Active) {
		//攻撃の種類はランダム
		RandActionCount = rand() % 100;

		//比重は通常攻撃多め
		if (RandActionCount >= 100 - SummonPriority)_attackAction = SUMMON;
		else if (RandActionCount >= 100 - SummonPriority - CirclePriority)_attackAction = CHARGE;
		else _attackAction = NORMAL;

		//核攻撃のフラグオン
		SelAttack();

		//次のアクションまでの猶予
		//S_DecisionCount = rand() % ;

		//最初から
		Active = false;
	}
	else {

		_attackAction = NON;
		//こうげきちゅうはTIMER切る7
		if (!SummobnStop && !EndSummonRepos && !_charge.GetAttackF() && !_normal.GetAttackF())
			ActionTimer++;
	}

	//タイマーリセット(通常攻撃とため攻撃が重ならないように)
	if (ActionTimer >= 800)
	{
		ActionTimer = 0;
	}
}

void SecondBoss::SelAttack()
{
	if (!_normal.GetAttackF() && !SummonF && _attackAction == SUMMON) {
		ResF = true;
		SummobnStop = true;
		SummonF = true;
	}
	//通常攻撃
	if (!SummobnStop && !_cattack.GetAttackF() && !_normal.GetAttackF() && _attackAction == NORMAL)
		_normal.SetNormalAttackF(true);
	//ため攻撃
	if (!SummobnStop &&SummonF&& !_cattack.GetAttackF() && !_normal.GetAttackF() && _attackAction == CHARGE)
		_cattack.SetAttackF(true);
}

void SecondBoss::DeathEffect()
{

	float l_AddSize = 3.5f;
	const float RandScale = 5.0f;
	float s_scale = 13.9f * l_AddSize;
	float e_scale = (8.0f + (float)rand() / RAND_MAX * RandScale - RandScale / 2.0f) * l_AddSize;

	//色
	const float RandRed = 0.02f;
	const float red = 0.2f + (float)rand() / RAND_MAX * RandRed;
	const XMFLOAT4 s_color = { 0.9f, red, 0.1f, 1.0f }; //濃い赤
	const XMFLOAT4 e_color = { 0, 0, 0, 1.0f }; //無色

	//乱数指定
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_Randlife(10, 40);
	int l_Life = int(l_Randlife(mt));

	const XMFLOAT4 s_color2 = { 1.0f,1.0f,1.0f,1.0f };
	const XMFLOAT4 e_color2 = { 0.8f,0.1f,0.1f,1.0f };
	float s_scale2 = 8.0f;
	float e_scale2 = 3.0f;
	float l_velocity2 = 0.5f;
	for (int i = 0; i < 3; ++i) {
		ParticleEmitter::GetInstance()->DeathEffect(50, { m_Position.x,(m_Position.y - 1.0f),m_Position.z }, s_scale2, e_scale2, s_color2, e_color2, l_velocity2);
	}


	ParticleEmitter::GetInstance()->DeathEffectBoss(l_Life, { m_Position.x,m_Position.y - 4.f,m_Position.z }, l_AddSize, s_scale, e_scale, s_color, e_color);

}

void SecondBoss::InitAwake() {

}


void SecondBoss::EndRollAction() {
	const float l_AddPosX = 0.4f;
	const float l_AddFrame = 0.01f;
	m_EndTimer++;
	if (_EndState == END_SET) {
		if (m_EndTimer == 1) {
			m_Position = { -70.0f,6.0f,12.0f };
			m_Rotation = { 0.0f,0.0f,0.0f };
			m_Scale = { 0.4f,0.8f,0.4f };
		}

		if (m_EndTimer == 380) {
			_EndState = END_WALK;
		}
	}
	else if (_EndState == END_WALK) {
		if (Helper::GetInstance()->CheckMin(m_Position.x, -10.0f, l_AddPosX)) {
			_EndState = END_DIR_CAMERA;
		}
	}
	else {
		Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame);
		m_Rotation.y = Ease(In, Cubic, m_Frame, m_Rotation.y, 90.0f);

		if (m_EndTimer == 1670) {
			m_EndStop = true;
		}
	}

	//sin波によって上下に動く
	if (!m_EndStop) {
		m_SinAngle += 6.0f;
		m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
		m_Position.y = (sin(m_SinAngle2) * 1.0f + 6.0f);
	}
	//OBJのステータスのセット
	Obj_SetParam();
}
//会話
void SecondBoss::AfterAction() {

}