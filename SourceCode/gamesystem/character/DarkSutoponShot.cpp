#include "DarkSutoponShot.h"

#include "Collision.h"
#include "Player.h"

void DarkSutoponShot::Init()
{
	
}


void DarkSutoponShot::Upda()
{

	if(ActionStart)
	ShotCount++;
	else
	{
		ShotCount = 1;
	}

	if ( ShotCount<=250&&ShotCount % 30 == 0) {
		ShotBul* newbullet;
		newbullet = new ShotBul();
		newbullet->SetSutopos({ SutoPos.x,0.f,SutoPos.z });
		newbullet->SetSutorot(SutoRot);
		newbullet->init();
		bullist.push_back(newbullet);
	}
	if (ShotCount > 250)
		ActionStart = false;


	for (ShotBul* bullet : bullist) {
		if (bullet != nullptr) {
			bullet->Upda();
		}
	}

	//弾の削除
	for (int i = 0; i < bullist.size(); i++) {
		if (bullist[i] == nullptr) {
			continue;
		}

		if (bullist[i]->ShotEnd()) {
			bullist.erase(cbegin(bullist) + i);
		}
	}
}

void DarkSutoponShot::Draw(DirectXCommon* dxCommon)
{
	for (ShotBul* bullet : bullist) {
		if (bullet != nullptr) {
			bullet->Draw();
		}
	}
}

void DarkSutoponShot::SpriteDraw()
{
	
}

void DarkSutoponShot::DeathUpdate(int m_Timer) {

}
void ShotBul::init()
{

	for (auto i = 0; i < BulSize; i++)
	{
		ShotObj[i].reset(new IKEObject3d());
		ShotObj[i]->Initialize();
		ShotObj[i]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Bullet));

		move[i] = { 0.f,0.f,0.1f,0.f };
		matRot[i] = XMMatrixRotationY(XMConvertToRadians(SutoRot.y - 0.f + (static_cast<float>(i * 30.f - 30.f))));
		move[i] = XMVector3TransformNormal(move[i], matRot[i]);

		BulAlive[i] = true;
		BulAlpha[i] = 1.f;
		BulPos[i] = Sutopos;
	}
}

void ShotBul::Upda()
{
	XMFLOAT4 s_color[BulSize];
	XMFLOAT4 e_color[BulSize];
	float s_scale[BulSize];
	float e_scale[BulSize];
	int m_Life[BulSize];

	for (auto i = 0; i < BulSize; i++)
	{
		if (!BulAlive[i])continue;

		s_color[i] = { 0.8f,0.4f,1.0f,1.0f };
		e_color[i] = { 0.40f,0.0f,0.50f,1.0f };
		s_scale[i] = 2.0f;
		e_scale[i] = 0.0f;
		m_Life[i] = 50;

		ParticleEmitter::GetInstance()->FireEffect(m_Life[i], BulPos[i], s_scale[i], e_scale[i], s_color[i], e_color[i]);

		if (Player::GetInstance()->GetDamageInterVal() == 0 && Collision::CircleCollision(BulPos[i].x,BulPos[i].z,0.7f,
			Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().z,1.f)) {
			Player::GetInstance()->PlayerHit(BulPos[i]);
			Player::GetInstance()->RecvDamage(0.3f);
			BulAlive[i] = false;
		}

		//進行スピード
		BulPos[i].y = Sutopos.y;
		BulPos[i].x += move[i].m128_f32[0] * 4;
		BulPos[i].z += move[i].m128_f32[2] * 4;
		//弾を薄く
		BulAlpha[i] -= 0.005f;
		if (BulAlpha[i] <= 0.f)BulAlive[i] = false;
	}
}

void ShotBul::Draw()
{
	IKEObject3d::PreDraw();
	for (auto i = 0; i < 3; i++) {
		if (!BulAlive[i])continue;
		ShotObj[i]->Draw();
	}
	IKEObject3d::PostDraw();
		
}
