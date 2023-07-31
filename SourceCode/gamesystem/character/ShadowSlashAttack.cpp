#include "ShadowSlashAttack.h"

#include <random>

#include "Collision.h"
#include "Helper.h"
#include "ImageManager.h"
#include "Player.h"

void ShadowSlashAttack::Init()
{
	for (auto i = 0; i < SwordSize; i++) {
		Swords_H[i].Obj.reset(new IKEObject3d());
		Swords_H[i].Obj->Initialize();
		Swords_H[i].Obj->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::NAIL));

		Swords_W[i].Obj.reset(new IKEObject3d());
		Swords_W[i].Obj->Initialize();
		Swords_W[i].Obj->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::NAIL));

	}
	for (auto i = 0; i < KotokoShadow; i++)
	{
		Kotoko[i].reset(new IKEObject3d());
		Kotoko[i]->Initialize();
		Kotoko[i]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Sutopon));

	}
	ShotArea[0].reset(IKETexture::Create(ImageManager::SLASHAREA , { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	ShotArea[0]->TextureCreate();
	ShotArea[1].reset(IKETexture::Create(ImageManager::SLASHAREA, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	ShotArea[1]->TextureCreate();

}

void ShadowSlashAttack::Upda()
{
	//状態移行(charastateに合わせる)
	(this->*stateTable[_phase])();

	for(auto i=0;i<SwordSize;i++)
	{
		Swords_H[i].Obj->SetColor({ 1,1,1,Swords_H[i].Alpha });
		Swords_H[i].Obj->SetScale(Swords_H[i].Scl);
		Swords_H[i].Obj->SetRotation(Swords_H[i].Rot);
		Swords_H[i].Obj->SetPosition(Swords_H[i].Pos);
		Swords_H[i].Obj->Update();

		Swords_W[i].Obj->SetColor({ 1,1,1,Swords_W[i].Alpha });
		Swords_W[i].Obj->SetScale(Swords_W[i].Scl);
		Swords_W[i].Obj->SetRotation(Swords_W[i].Rot);
		Swords_W[i].Obj->SetPosition(Swords_W[i].Pos);
		Swords_W[i].Obj->Update();

	}



	for(auto i=0;i<KotokoShadow;i++)
	{
		//座標セット
		if(i<2)Kotoko[i]->SetPosition(KotokoPos[0]);
		else Kotoko[i]->SetPosition(KotokoPos[1]);

		Kotoko[i]->SetColor({ 1,0,1,KotokoAlpha[i]});
		Kotoko[i]->SetScale({ 2,2,2 });
		Kotoko[i]->Update();
	}

	ShotArea[0]->SetPosition(KotokoPos[0]);
	ShotArea[1]->SetPosition(KotokoPos[1]);

	constexpr float AnchorY = 0.3f;
	
	for(auto i=0;i<2;i++)
	{
		Cinter[i] += 0.013f;
		if (Cinter[i] > 1.0f)Cinter[i] = AnchorY;

		ShotArea[i]->SetCinter(Cinter[i]);
		ShotArea[i]->SetScale({ 2.f,30.f,1.f });
		ShotArea[i]->SetColor({ 1,1,1,AreaAlpha[i]});
		ShotArea[i]->SetClipF(true);
		ShotArea[i]->Update();
	}
	//横
	ShotArea[0]->SetRotation({ 90,0,180 });
	Kotoko[0]->SetRotation({ 0,180,0 });
	
	//縦
	ShotArea[1]->SetRotation({ 90,0,90 });


	if (ActionEnd)
		_phase = NON;
}

void ShadowSlashAttack::Draw(DirectXCommon* dxCommon)
{
	IKEObject3d::PreDraw();
	for(auto i=0;i<SwordSize;i++)
	{
		if (Swords_H[i].Alpha > 0.f)
		Swords_H[i].Obj->Draw();
		if (Swords_W[i].Alpha > 0.f)
		Swords_W[i].Obj->Draw();
	}
	for (auto i = 0; i < KotokoShadow; i++) {
		if (KotokoAlpha[i] <= 0.f)continue;
		Kotoko[i]->Draw();
	}
	IKEObject3d::PostDraw();


	IKETexture::PreDraw2(dxCommon, 0);
	for (auto i = 0; i < 2; i++) {
		if (AreaAlpha[i] <= 0.f)continue;
		ShotArea[i]->Draw();
	}
	IKETexture::PostDraw();
}

void ShadowSlashAttack::SpriteDraw()
{
	
}

void ShadowSlashAttack::DeathUpdate(int Timer) {

}

void ShadowSlashAttack::Phase_Idle()
{
	KotokoPos[0].x = Player::GetInstance()->GetPosition().x;
	KotokoPos[0].z = -60.f;

	KotokoPos[1].x = -60.f;
	KotokoPos[1].z = boss->GetPosition().z;

	ImpactTexPos[0] = KotokoPos[0];
	ImpactTexPos[1] = KotokoPos[1];

	for (auto i = 0; i < 4; i++) {
		KotokoAlpha[i] = 1.f;
	}
	AreaAlpha[0] = 0.8f;
	AreaAlpha[1] = 0.8f;

	_phase = Phase::AREAVIEW;
}

void ShadowSlashAttack::Phase_ViewArea()
{
	//縦範囲
	for (auto i = 0; i < SwordSize; i++) {
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_RandPos(-5, 5);

		Swords_H[i].Pos.x = KotokoPos[0].x + (float)(l_RandPos(mt));
		Swords_H[i].Pos.z = KotokoPos[0].z + static_cast<float>(i) * 6.f;
		Swords_H[i].Pos.y = -10.f;
		uniform_int_distribution<int> l_RandScl(2, 4);

		uniform_int_distribution<int> l_RandRot(-60, 60);
		Swords_H[i].Scl = { (float)l_RandScl(mt) * 1.5f,0.f,(float)l_RandScl(mt) * 1.5f };
		Swords_H[i].YSclRandMax = (float)l_RandScl(mt);
		Swords_H[i].Rot = { (float)l_RandRot(mt) / 2,0,(float)l_RandRot(mt) / 2 };
		Swords_H[i].Alpha = 1.f;
	}


	//横範囲
	for (auto i = 0; i < SwordSize; i++) {
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_RandPos(-5, 5);

		Swords_W[i].Pos.x = KotokoPos[1].x + static_cast<float>(i) * 6.f;
		Swords_W[i].Pos.z = KotokoPos[1].z + (float)(l_RandPos(mt));
		Swords_W[i].Pos.y = -10.f;
		uniform_int_distribution<int> l_RandScl(2, 4);

		uniform_int_distribution<int> l_RandRot(-60, 60);
		Swords_W[i].Scl = { (float)l_RandScl(mt) * 1.5f,0.f,(float)l_RandScl(mt) * 1.5f };
		Swords_W[i].YSclRandMax = (float)l_RandScl(mt);
		Swords_W[i].Rot = { (float)l_RandRot(mt) / 2,0,(float)l_RandRot(mt) / 2 };
		Swords_W[i].Alpha = 1.f;
	}

	AreaAlpha[0] = 0.8f;
	AreaAlpha[1] = 0.8f;
	IdleCount++;
	if (IdleCount > 90) {
		_phase = IMPACTSLASH;
	}
}

void ShadowSlashAttack::Phase_Impact()
{
	constexpr float AddScl = 0.15f;

	Swords_H[0].Scl.y += AddScl;
	Swords_W[0].Scl.y += AddScl;
	
	for(auto i=1;i<SwordSize;i++)
	{
		if(Swords_H[i-1].Scl.y>0.5f)
			Swords_H[i].Scl.y += AddScl;

		if (Swords_W[i - 1].Scl.y > 0.5f)
			Swords_W[i].Scl.y += AddScl;
	}

	for(auto i=0;i<SwordSize;i++)
	{
		Swords_H[i].Alpha = 1.f;
		Swords_W[i].Alpha = 1.f;

		Helper::GetInstance()->Clamp(Swords_W[i].Scl.y, 0.f, Swords_W[i].YSclRandMax);
		Helper::GetInstance()->Clamp(Swords_H[i].Scl.y, 0.f, Swords_H[i].YSclRandMax);
	}

	for(auto i=0;i<4;i++)
	{
		KotokoAlpha[i] -= 0.1f;
	}
	if(Swords_H[SwordSize-1].Scl.y>= Swords_H[SwordSize-1].YSclRandMax&&
		Swords_W[SwordSize - 1].Scl.y >= Swords_W[SwordSize - 1].YSclRandMax )
	{
		_phase = NAILDEST;
	}
	//else{
		Line2D lines[2];
		Point points;
		points = { Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().z };

		lines[0].start = { KotokoPos[0].x,KotokoPos[0].z };
		lines[0].end = { KotokoPos[0].x,60.f };
		lines[1].start = { KotokoPos[1].x,KotokoPos[1].z };
		lines[1].end = { 60,KotokoPos[1].z };
		if (Swords_H[0].Scl.y > 2.f)
		{
			for (size_t i = 0; i < 2; i++) {
				if (Player::GetInstance()->GetDamageInterVal() != 0)break;
				//////////////
				//判定部
				if (Collision::IsCollidingLineAndCircle(lines[i], points, 10.0f))
				{
					Player::GetInstance()->PlayerHit(KotokoPos[i]);
					Player::GetInstance()->RecvDamage(Dam);
				}
			}
		}
	//}
	AreaAlpha[0] = 0.f;
	AreaAlpha[1] = 0.f;
}

void ShadowSlashAttack::Phase_Dest()
{
	Swords_W[0].Alpha -= 0.02f;
	Swords_H[0].Alpha -= 0.02f;

	for(auto i=1;i<SwordSize;i++)
	{
		if (Swords_W[i - 1].Alpha <= 0.6f)
			Swords_W[i].Alpha -= 0.04f;

		if (Swords_H[i - 1].Alpha <= 0.6f)
			Swords_H[i].Alpha -= 0.04f;
	}
	if (Swords_H[SwordSize - 1].Alpha <= 0.f &&
		Swords_W[SwordSize - 1].Alpha <= 0.f) {
		for (auto i = 0; i < boss->GetGhost().size(); i++) {
			if (boss->GetGhost()[i]->GetState() != Ghost::STATE_DARKOTI)continue;
			boss->GetGhost()[i]->SetCleanGhost(true);
		}
		_phase = END;
	}
}

void ShadowSlashAttack::Phase_End()
{
	for(auto i=0;i<SwordSize;i++)
	{
		Swords_W[i].Alpha = 0.8f;
		Swords_W[i].Scl = {};
		Swords_W[i].Rot = {};

		Swords_H[i].Alpha = 0.8f;
		Swords_H[i].Scl = {};
		Swords_H[i].Rot = {};

	}
	IdleCount = 0;
	
	ActionEnd = true;
}


void (ShadowSlashAttack::* ShadowSlashAttack::stateTable[])() = {
	&ShadowSlashAttack::Phase_Idle,//待機
	&ShadowSlashAttack::Phase_ViewArea,//移動
	&ShadowSlashAttack::Phase_Impact,
	& ShadowSlashAttack::Phase_Dest,
	&ShadowSlashAttack::Phase_End,//移動
};



