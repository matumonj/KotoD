#include "SmashShotAttack.h"

#include <random>

#include "Collision.h"
#include"Easing.h"
#include "Player.h"
#include"Helper.h"
#include "ImageManager.h"

void SmashShotAttack::Init()
{
	for (auto i = 0; i < BulSize; i++) {
		ShotObj[i].reset(new IKEObject3d());
		ShotObj[i]->Initialize();
		ShotObj[i]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Bullet));

		ShotArea[i].reset(IKETexture::Create(ImageManager::DARKBOM, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
		ShotArea[i]->TextureCreate();

		BulAlpha[i] = 1.f;
	}

}
void (SmashShotAttack::* SmashShotAttack::stateTable[])() = {
	&SmashShotAttack::Phase_Idle,//待機
	&SmashShotAttack::Phase_Shot,//移動
	&SmashShotAttack::Phase_Impact,
	&SmashShotAttack::Phase_End,//移動
};

void SmashShotAttack::Upda()
{
	m_Position = boss->GetPosition();

	if (_phase == Phase::NON && !ActionEnd) {
		_phase = Phase::SHOT;
	}
	//状態移行(charastateに合わせる)
	(this->*stateTable[_phase])();

	for (auto i = 0; i < BulSize; i++)
	{
		ShotObj[i]->SetColor({ 1.f,1.f,1.f ,BulAlpha[i] });
		ShotObj[i]->SetScale({ 1.f,1.f,1.f });
		ShotObj[i]->SetRotation(BulRot[i]);
		ShotObj[i]->SetPosition(BulPos[i]);
		ShotObj[i]->Update();

		BulRotZ[i] += 2.f;
		ShotArea[i]->SetPosition(ImpactTexPos[i]);
		ShotArea[i]->SetRotation({ 90,0,BulRotZ[i] });
		ShotArea[i]->SetScale({ AreaScl[i],AreaScl[i],1.f });
		ShotArea[i]->SetColor({ 1,1,1,AreaAlpha[i] });
		ShotArea[i]->Update();
	}
	boss->SetPosition(m_Position);

	//攻撃終了時の初期化周り
	if (ActionEnd)
	{
		for (auto i = 0; i < BulSize; i++)
			BulAlpha[i] = 1.f;
		_phase = NON;

	}
}

void SmashShotAttack::Draw(DirectXCommon* dxCommon)
{
	IKEObject3d::PreDraw();
	for (auto i = 0; i < BulSize; i++)
		ShotObj[i]->Draw();
	IKEObject3d::PostDraw();

	IKETexture::PreDraw2(dxCommon, AlphaBlendType);

	for (auto i = 0; i < BulSize; i++)
		ShotArea[i]->Draw();
	IKETexture::PostDraw();

}

void SmashShotAttack::SpriteDraw()
{

}


#pragma region 行動の中身
void SmashShotAttack::Phase_Idle()
{
	AttackTimer++;
	FollowPlayer();

	//次フェーズ
	bool next = AttackTimer > 120;

	m_Rotation = boss->GetRotation();
	if (next) {
		for (auto i = 0; i < BulSize; i++)
			BulPos[i] = boss->GetPosition();

		_phase = Phase::SHOT;
	}
}

void SmashShotAttack::Phase_Shot()
{

	//弾の向きをプレイヤーに
	RottoPlayer();
	//向いた方向に進む
	constexpr float SummonSpeed = 4.f;

	constexpr float MaxY = 20.f;

	Helper::GetInstance()->FrameCheck(BulEaseCount[0], 0.015f);
	BulPos[0].y += 0.5f;
	BulPos[0].x =boss->GetPosition().x;
	BulPos[0].z = boss->GetPosition().z;

	for (auto i = 1; i < BulSize; i++)
	{
		//弾を上乱射
		if (BulPos[i - 1].y > 5.f) {
			Helper::GetInstance()->FrameCheck(BulEaseCount[i], 0.015f);

			BulPos[i].y += 0.5f;
			BulPos[i].x = Ease(In, Quad, BulEaseCount[i], boss->GetPosition().x, boss->GetPosition().x + PosXVal[i]);
			BulPos[i].z = Ease(In, Quad, BulEaseCount[i], boss->GetPosition().z, boss->GetPosition().z + PosZVal[i]);
		} else
		{
			mt19937 mt{ std::random_device{}() };
			uniform_int_distribution<int> l_RandomMove(-10, 10);
			PosXVal[i] = (float)(l_RandomMove(mt));
			PosZVal[i] = (float)(l_RandomMove(mt));

		}
	}
	//フェーズ移行
	if (BulPos[BulSize - 1].y > MaxY) {

		for (auto i = 0; i < BulSize; i++) {

			mt19937 mt{ std::random_device{}() };
			uniform_int_distribution<int> l_RandPosX(-30, 30);
			uniform_int_distribution<int> l_RandPosZ(-30, 30);
			uniform_int_distribution<int> l_RandomMove(1, 5);

			//BulSpeed[i] = (float)(l_RandomMove(mt)) / 10.f;

			ImpactTexPos[i].x = Player::GetInstance()->GetPosition().x + (float)(l_RandPosX(mt));
			ImpactTexPos[i].z = Player::GetInstance()->GetPosition().z + (float)(l_RandPosZ(mt));
		}
		ImpactTexPos[0].x = Player::GetInstance()->GetPosition().x;
		ImpactTexPos[0].z = Player::GetInstance()->GetPosition().z;

		//ImpactTexPos[0] = { Player::GetInstance()->GetPosition() };
		_phase = IMPACT;
	}

	for (auto i = 0; i < BulSize; i++) {
		if (BulPos[i].y <= -5.f)
		{
			BulPos[i] = boss->GetPosition();
		}
	}

	{
		XMFLOAT4 s_color[BulSize];
		XMFLOAT4 e_color[BulSize];
		float s_scale[BulSize];
		float e_scale[BulSize];
		int m_Life[BulSize];

		for (auto i = 0; i < BulSize; i++)
		{
			if (BulPos[i].y < 0.f)continue;
			s_color[i] = { 0.8f,0.4f,1.0f,1.0f };
			e_color[i] = { 0.40f,0.0f,0.50f,1.0f };
			s_scale[i] = 2.0f;
			e_scale[i] = 0.0f;
			m_Life[i] = 50;

			ParticleEmitter::GetInstance()->FireEffect(m_Life[i], BulPos[i], s_scale[i], e_scale[i], s_color[i], e_color[i]);

		}
	}
}

void SmashShotAttack::Phase_End()
{
	for (auto i = 0; i < BulSize; i++)
	{
		BulAlpha[i] = 1.f;
		BulPos[i] = boss->GetPosition();
		AreaScl[i] = 0.f;
		BulEaseCount[i] = 0.f;
		AreaAlpha[i] = 0.f;
	}
	boss->SetRotation({ boss->GetRotation().x,
	OldRot.y + 90.f,
	boss->GetRotation().z });
	ActionEnd = true;
}


void SmashShotAttack::RottoPlayer()
{
	//FollowPlayer();



}

void SmashShotAttack::DeathUpdate(int Timer) {

}

void SmashShotAttack::Phase_Impact()
{
	//[0]はプレイヤー追従

	AreaScl[0] += 0.05f;
	if (BulPos[0].y >= 0.f)
		AreaAlpha[0] += 0.02f;


	if (AreaScl[BulSize - 1] >= 1.f) {
		BulPos[0].y--;
	} else {
		for (auto i = 0; i < BulSize; i++)
			BulPos[i].y = 50.f;
	}
	for (auto i = 1; i < BulSize; i++)
	{
		if (BulPos[i].y >= 0.f)
			AreaAlpha[i] += 0.02f;
		if (AreaScl[i - 1] > 0.1f)AreaScl[i] += 0.05f;

		if (BulPos[i - 1].y <= 40.f)BulPos[i].y--;
	}

	//たま降ってくる
	for (auto i = 0; i < BulSize; i++) {
		BulPos[i].x = ImpactTexPos[i].x;
		BulPos[i].z = ImpactTexPos[i].z;

		Helper::GetInstance()->Clamp(BulAlpha[i], 0.f, 1.f);
		Helper::GetInstance()->Clamp(AreaScl[i], 0.f, 1.5f);
	}

	{
		XMFLOAT4 s_color[BulSize];
		XMFLOAT4 e_color[BulSize];
		float s_scale[BulSize];
		float e_scale[BulSize];
		int m_Life[BulSize];

		for (auto i = 0; i < BulSize; i++)
		{
			if (BulPos[i].y < 0.f)continue;
			s_color[i] = { 0.8f,0.3f,1.0f,1.0f };
			e_color[i] = { 0.3f,0.0f,0.4f,1.0f };
			s_scale[i] = 2.0f;
			e_scale[i] = 0.0f;
			m_Life[i] = 50;

			ParticleEmitter::GetInstance()->FireEffect(m_Life[i], BulPos[i], s_scale[i], e_scale[i], s_color[i], e_color[i]);

		}
	}
	BomParticle();
}


void SmashShotAttack::BomParticle()
{
	float l_AddSize[BulSize];
	float s_scale[BulSize];
	float e_scale[BulSize];
	float red[BulSize];
	//色
	const float RandRed = 0.2f;
	//色
	XMFLOAT4 s_color[BulSize];
	XMFLOAT4 e_color[BulSize];
	float RandScale[BulSize];
	int l_Life[BulSize];



	for (auto i = 0; i < BulSize; i++) {
		if (BulPos[i].y > 0.f)continue;
		if (BulPos[i].y < -5.f)continue;
		red[i] = 0.2f + (float)rand() / RAND_MAX * RandRed;
		RandScale[i] = 3.0f;
		l_AddSize[i] = 2.5f;
		s_scale[i] = 0.3f * l_AddSize[i];
		e_scale[i] = (4.0f + (float)rand() / RAND_MAX * RandScale[i] - RandScale[i] / 2.0f) * l_AddSize[i];

		s_color[i] = { 0.8f, 0.f, 0.9f, 1.0f }; //濃い赤
		e_color[i] = { 0.5f, 0, 0.5f, 1.0f }; //無色

		//乱数指定
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_Randlife(10, 40);
		l_Life[i] = int(l_Randlife(mt));
		if (Player::GetInstance()->GetDamageInterVal() == 0 && Collision::GetLength(Player::GetInstance()->GetPosition(), BulPos[i]) < 5.f)
		{
			Player::GetInstance()->PlayerHit(boss->GetPosition());
			Player::GetInstance()->RecvDamage(Dam);
		}
		ParticleEmitter::GetInstance()->Explosion(l_Life[i], BulPos[i], l_AddSize[i], s_scale[i], e_scale[i], s_color[i], e_color[i],0);

	}

	constexpr float AlphaSub = 0.06f;
	for (auto i = 0; i < BulSize; i++) {
		if (BulPos[i].y < 0.f)
		{
			AreaAlpha[i] -= AlphaSub;
		}
	}

	if (AreaAlpha[BulSize - 1] <= 0.f)
	{
		_phase = END;
	}
}
