#include "KnockAttack.h"

#include "Collision.h"
#include "Easing.h"
#include "Helper.h"
#include "ImageManager.h"
#include "Player.h"
#include <random>

void KnockAttack::Init()
{
	darksutopon.reset(new IKEObject3d());
	darksutopon->Initialize();
	darksutopon->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Sutopon));

	KnockImpTex.reset(IKETexture::Create(ImageManager::IMPACT2, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	KnockImpTex->TextureCreate();
	KnockImpTex2.reset(IKETexture::Create(ImageManager::IMPACT, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	KnockImpTex2->TextureCreate();
}

void KnockAttack::Upda()
{
	ImpactAction();

	SutoponCircleAngle++;
	stopos.x = boss->GetPosition().x + sinf(SutoponCircleAngle * (PI / 180.0f)) * 10.0f;
	stopos.z = boss->GetPosition().z + cosf(SutoponCircleAngle * (PI / 180.0f)) * 10.0f;

	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();

	//角度の取得 プレイヤーが敵の索敵位置に入ったら向きをプレイヤーの方に
	XMVECTOR PositionA = {l_player.x,l_player.y,l_player.z };
	XMVECTOR PositionB = {stopos.x,stopos.y,stopos.z };

	//プレイヤーと敵のベクトルの長さ(差)を求める
	XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;

	float RottoPlayer;
	RottoPlayer = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);

	strot.y = RottoPlayer * 60.f + 180.f;
	darksutopon->SetScale({ 1,1,1 });
	darksutopon->SetColor({ 0.9f,0.2f,0.7f,0.7f });
	darksutopon->SetPosition(stopos);
	darksutopon->SetRotation(strot);
	darksutopon->Update();

	KnockImpTex->Update();
	KnockImpTex->SetPosition({ stopos.x,boss->GetPosition().y + 5.f,stopos.z });
	KnockImpTex->SetScale({TexScl});
	KnockImpTex->SetRotation({ 90,0,0 });
	KnockImpTex->SetColor({ 1,1,1,TexAlpha });

	KnockImpTex2->SetPosition({ stopos.x,boss->GetPosition().y + 5.f,stopos.z });
	KnockImpTex2->SetScale(TexScl2);
	KnockImpTex2->SetRotation({ 90,0,0 });
	KnockImpTex2->SetColor({ 1,1,1,TexAlpha2 });
	KnockImpTex2->Update();
}

void KnockAttack::Draw(DirectXCommon* dxCommon)
{
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	if (!m_End) {
		KnockImpTex->Draw();
		KnockImpTex2->Draw();
	}
	IKETexture::PostDraw();

	IKEObject3d::PreDraw();
	darksutopon->Draw();
	IKEObject3d::PostDraw();

}

void KnockAttack::DeathUpdate(int Timer) {
	m_End = true;
	if (_EndState == END_SET) {
		if (Timer == 1) {
			stopos = { 5.0f,2.0f,10.0f };
		}
		else if (Timer == 100) {
			m_AddPower = 0.8f;
			_EndState = END_MOVE;
		}
	}
	else if(_EndState == END_MOVE) {
		m_AddPower -= m_Gravity;
		stopos.x += 0.5f;
		stopos.z += 0.5f;
		stopos.y += m_AddPower;
		if (stopos.y <= 0.5f) {
			_EndState = END_STOP;
		}
	}
	else {
		DeathParticle();
	}
	darksutopon->SetScale({ 1,1,1 });
	darksutopon->SetColor({ 0.9f,0.2f,0.7f,0.7f });
	darksutopon->SetPosition(stopos);
	darksutopon->SetRotation(strot);
	darksutopon->Update();
}

void KnockAttack::AppearUpdate(int Timer) {
	const float l_AddFrame = 0.01f;
	if (_AppState == APP_SET) {
		stopos = { 7.0f,20.0f,25.0f };
		strot.y = 270.0f;

		if (Timer == 510) {
			_AppState = APP_MOVE;
		}
	}
	else if (_AppState == APP_MOVE) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			_AppState = APP_END;
			m_Frame = {};
		}

		stopos.y = Ease(In,Cubic,m_Frame,stopos.y,2.0f);
	}
	else {
		//sin波によって上下に動く
		m_SinAngle += 3.5f;
		m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
		m_Position.y = (sin(m_SinAngle2) * 0.5f + 2.0f);
	}
	darksutopon->SetScale({ 1,1,1 });
	darksutopon->SetColor({ 0.9f,0.2f,0.7f,0.7f });
	darksutopon->SetPosition(stopos);
	darksutopon->SetRotation(strot);
	darksutopon->Update();
}

void KnockAttack::DeathParticle()
{
	float l_AddSize = 2.5f;
	const float RandScale = 3.0f;
	float s_scale = 0.3f * l_AddSize;
	float e_scale = (4.0f + (float)rand() / RAND_MAX * RandScale - RandScale / 2.0f) * l_AddSize;

	//色
	const float RandRed = 0.2f;
	const float red = 0.2f + (float)rand() / RAND_MAX * RandRed;
	const XMFLOAT4 s_color = { 0.9f, red, 0.1f, 1.0f }; //濃い赤
	const XMFLOAT4 e_color = { 0, 0, 0, 1.0f }; //無色
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_Randlife(10, 40);
	int l_Life = int(l_Randlife(mt));
	ParticleEmitter::GetInstance()->ExproEffectBoss(l_Life, stopos, l_AddSize, s_scale, e_scale, s_color, e_color);
}

void KnockAttack::ImpactAction()
{
	constexpr float AddTexScling = 0.08f;
	//ドッスン挙動
	//落下の緩急
	constexpr float Distortion = 3.f;
	//地面の高さ
	constexpr float GroundY = 10.f;
	//ジャンプ高さ
	constexpr float Height = 10.f;

	if(KnockF)
	{

		JFrame += 1.f / 60.f;
		stopos.y = GroundY+ (1.0f - pow(1.0f - sin(PI * JFrame), Distortion)) * Height;
		ReturnEaseT = 0.f;
		//SUB Alpha-Scling
		if (JFrame >= 1.f) {
			if (Player::GetInstance()->GetDamageInterVal() == 0 && Collision::GetLength(stopos, Player::GetInstance()->GetPosition()) < 13.f) {
				Player::GetInstance()->PlayerHit(stopos);
				Player::GetInstance()->RecvDamage(Dam);
			}
			TexAlpha -= 0.02f;
			TexScl.x += AddTexScling; TexScl.y += AddTexScling;
			if (TexScl.x > 1.5f || TexScl.z > 1.5f)
			{
				TexAlpha2 -= 0.02f;
				TexScl2.x += AddTexScling; TexScl2.y += AddTexScling;
			}
		}
		
		//knock
		

	}
	else
	{
		Helper::GetInstance()->FrameCheck(ReturnEaseT, 0.04f);
		Ease(In, Quad, ReturnEaseT, 10.f, boss->GetPosition().y + 10.f);
		//reset
		TexScl = { 0.f,0.f,0.f };
		TexScl2 = { 0.f,0.f,0.f };

		stopos.y = boss->GetPosition().y + 10.f;
		JFrame = 0.f;
		TexAlpha = 0.f;
		TexAlpha2 = 0.f;
	}
	//off Flag
	if (TexAlpha2 <= 0.f)
		KnockF = false;

	Helper::GetInstance()->Clamp(TexAlpha, 0.f, 1.f);
	Helper::GetInstance()->Clamp(TexAlpha2, 0.f, 1.f);
	Helper::GetInstance()->Clamp(JFrame, 0.f, 1.f);
	//Helper::GetInstance()->Clamp(JFrame, 0.f, 1.f);
}

void KnockAttack::SpriteDraw()
{
	
}



