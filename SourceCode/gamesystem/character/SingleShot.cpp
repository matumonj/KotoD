#include "SingleShot.h"
#include <random>
#include"Easing.h"
#include "Collision.h"
#include "Helper.h"
#include "Player.h"

void SingleShot::Init()
{
	for (auto i = 0; i < BulSize; i++) {
		ShotObj[i].reset(new IKEObject3d());
		ShotObj[i]->Initialize();
		ShotObj[i]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Bullet));

		BulAlpha[i] = 1.f;
	}

}
void (SingleShot::* SingleShot::stateTable[])() = {
	&SingleShot::Phase_Idle,//待機
	&SingleShot::Phase_Shot,//移動
	&SingleShot::Phase_End,//移動
};

void SingleShot::Upda()
{
	m_Position = boss->GetPosition();


	//状態移行(charastateに合わせる)
	(this->*stateTable[_phase])();

	for (auto i = 0; i < BulSize; i++)
	{
		if (!BulAlive[i])
			//BulAlpha[i] = 0.f;

			ShotObj[i]->SetColor({ 1.f,1.f,1.f ,BulAlpha[i] });
		ShotObj[i]->SetScale({ 3.f,3.f,3.f });
		ShotObj[i]->SetRotation(BulRot[i]);
		ShotObj[i]->SetPosition(BulPos[i]);
		ShotObj[i]->Update();
	}
	boss->SetPosition(m_Position);

	//攻撃終了時の初期化周り
	if (ActionEnd)
	{
		RotEaseTime = 0.f;
		for (auto i = 0; i < BulSize; i++) {
			BulAlpha[i] = 1.f;
			BulPos[i].x = boss->GetPosition().x;
			BulPos[i].y = 3.f;
			BulPos[i].z = boss->GetPosition().z;
			ShotAlive[i] = false;

		}_phase = NON;
	}
}

void SingleShot::Draw(DirectXCommon* dxCommon)
{
	if (_phase != SHOT)return;
	IKEObject3d::PreDraw();
	for (auto i = 0; i < BulSize; i++) {
		if (BulAlpha[i] <= 0.f)continue;
		if (!ShotAlive[i])continue;
		ShotObj[i]->Draw();
	}
	IKEObject3d::PostDraw();


}

void SingleShot::SpriteDraw()
{
	if (_phase != SHOT)return;
	IKESprite::PreDraw();
	for (auto i = 0; i < 4; i++) {
		if (ShotAlive[i])
			ShotArea[i]->Draw();
	}
	IKESprite::PostDraw();
}

void SingleShot::DeathUpdate(int Timer) {

}

#pragma region 行動の中身
void SingleShot::Phase_Idle()
{

	FollowPlayerAct();

	//次フェーズ
	if (RotEaseTime <= 0.f)
		OldRot = boss->GetRotation();

	Helper::GetInstance()->FrameCheck(RotEaseTime, 0.04f);

	XMVECTOR PositionB = { boss->GetPosition().x,
			boss->GetPosition().y,
			boss->GetPosition().z,
	};

	XMVECTOR PositionA = { Player::GetInstance()->GetPosition().x,
		Player::GetInstance()->GetPosition().y,
		Player::GetInstance()->GetPosition().z };
	//プレイヤーと敵のベクトルの長さ(差)を求める
	XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;

	RottoGhost = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);

	for (auto i = 0; i < BulSize; i++) {
		BulAlpha[i] = 1.f;
		BulPos[i].x = boss->GetPosition().x;
		BulPos[i].y = 3.f;
		BulPos[i].z = boss->GetPosition().z;
	}

	boss->SetRotation({ boss->GetRotation().x,
	Ease(In,Quad,RotEaseTime,OldRot.y,RottoGhost * 60 + 180),
	boss->GetRotation().z });

	ShotSize = boss->GetGhostSize();

	if (RotEaseTime >= 1.f) {

		boss->AnimationControl(InterBoss::AnimeNames::SHOT, false, 1);

		_phase = Phase::SHOT;
	}

}
void SingleShot::Phase_Shot()
{

	RotEaseTime = 0.f;

	if (ShotSize == 1)
	{
		ShotAlive[1] = true;
		ShotAlive[0] = false;
		ShotAlive[2] = false;
	}
	if (ShotSize == 2)
	{
		BulAlive[1] = false;
		ShotAlive[0] = true;
		ShotAlive[2] = true;
	}
	if (ShotSize == 3||ShotSize>=5)
	{
		ShotAlive[0] = true;
		ShotAlive[1] = true;
		ShotAlive[2] = true;
	}
	//弾の向きをプレイヤーに
	RottoPlayer();
	FollowPlayerAct();
	//向いた方向に進む
	constexpr float SummonSpeed = 4.f;
	XMFLOAT4 s_color[BulSize];
	XMFLOAT4 e_color[BulSize];
	float s_scale[BulSize];
	float e_scale[BulSize];
	int m_Life[BulSize];
	for (auto i = 0; i < BulSize; i++)
	{
		move[i] = { 0.f,0.f,0.1f,0.f };
		matRot[i] = XMMatrixRotationY(XMConvertToRadians(boss->GetRotation().y - 0.f + (static_cast<float>(i * 30.f - 30.f))));
		move[i] = XMVector3TransformNormal(move[i], matRot[i]);

		s_color[i] = { 0.8f,0.4f,1.0f,1.0f };
		e_color[i] = { 0.40f,0.0f,0.50f,1.0f };
		s_scale[i] = 2.0f;
		e_scale[i] = 0.0f;
		m_Life[i] = 50;

		ParticleEmitter::GetInstance()->FireEffect(m_Life[i], BulPos[i], s_scale[i], e_scale[i], s_color[i], e_color[i]);

	}
	

	for (auto i = 0; i < BulSize; i++)
	{
		if (!ShotAlive[i])continue;
		if (BulAlpha[i] >= 1.f){
			BulPos[i].x = boss->GetPosition().x;
			BulPos[i].y = 3.f;
			BulPos[i].z = boss->GetPosition().z;
		}
		if (Player::GetInstance()->GetDamageInterVal() == 0 && Collision::CircleCollision(BulPos[i].x,BulPos[i].z,3.f, 
			Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().z,3.f)) {
			Player::GetInstance()->PlayerHit(m_Position);
			Player::GetInstance()->RecvDamage(Dam);
			ShotAlive[i] = false;
		}

		//進行スピード
		BulPos[i].x += move[i].m128_f32[0] * SummonSpeed;
		BulPos[i].z += move[i].m128_f32[2] * SummonSpeed;
		//弾を薄く
		BulAlpha[i] -= 0.01f;
	}

	bool OwnShot = ShotSize == 1 && BulAlpha[1] <= 0.f;
	bool TwoShot = ShotSize == 2 && BulAlpha[0] <= 0.f;
	bool ThirdShot = (ShotSize == 3||ShotSize>=5) && BulAlpha[2] <= 0.f;

	if (OwnShot || TwoShot || ThirdShot) {
		_phase = END;
	}
	RotEaseTime = 0.f;
}

void SingleShot::Phase_End()
{
	RotEaseTime = 0;
	for (auto i = 0; i < BulSize; i++) {
		ShotAlive[i] = false;
		BulAlpha[i] = 1.f;
	}
	PhaseCount = 0;
	AttackTimer = 0;
	
	ActionEnd = true;
	_phase = NON;
	//if (PhaseCount < 4) {
	//}

	//else 
}


void SingleShot::RottoPlayer()
{
	//FollowPlayer();

}


void SingleShot::FollowPlayerAct()
{

	XMVECTOR move = { 0.f,0.f, 0.1f, 0.0f };

	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(boss->GetRotation().y));

	move = XMVector3TransformNormal(move, matRot);

	float SummonSpeed = 2.f;
	m_Position = {
					m_Position.x + move.m128_f32[0] * SummonSpeed,
				m_Position.y,
				m_Position.z + move.m128_f32[2] * SummonSpeed
	};

	boss->SetPosition(m_Position);
}
