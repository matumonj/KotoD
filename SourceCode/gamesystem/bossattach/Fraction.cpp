#include "Fraction.h"
#include"Player.h"
#include"Collision.h"
#include"InterBullet.h"
#include"Helper.h"
#include"Easing.h"
#include"ImageManager.h"

Fraction::~Fraction()
{

}

void Fraction::Init(const XMFLOAT3& BossPos)
{
	boss_pos_ = BossPos;
	m_Object.reset(new IKEObject3d);
	m_Object->Initialize();
	m_Object->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::GLASS1));
	m_Scale = { 3.f,3.f,3.f };
	m_Object->SetPosition(m_Position);

	tex.reset(IKETexture::Create(ImageManager::SHADOW, { 0,0,0 }, { 1.f,1.f,1.f }, { 1,1,1,1 }));
	tex->TextureCreate();
	tex->SetPosition(m_Position);
	tex->SetTiling(1.0f);
}

void Fraction::Obj_Set()
{
	tex->SetRotation({ 90.f,0.f,0.f });
	tex->SetPosition({ m_Position.x,0,m_Position.z });
	tex->Update();

	m_Object->SetPosition(m_Position);
	m_Object->SetScale(m_Scale);
	m_Object->SetRotation(m_Rotation);
	m_Object->SetColor(m_Color);
	m_Object->Update();
}

void Fraction::Pop()
{
	if (pop_ != PopState::Before) { return; }
	commandTimer += 1.0f / 60;
	Helper::GetInstance()->Clamp(commandTimer, 0.0f, 1.0f);
	float s_pos = boss_pos_.y;
	float e_pos = 15.f;

	m_Position = {
	boss_pos_.x,
	Ease(Out, Quart, commandTimer, s_pos, e_pos),
	boss_pos_.z,
	};

	if (commandTimer >= 1) {
		pop_ = PopState::After;
		pop_pos_ = m_Position;
		commandTimer = 0.f;
	}
}

void Fraction::Spatter()
{
	if (pop_ != PopState::After) { return; }
	if (drop_F != true) { return; }

	commandTimer += 1.0f / 80;
	Helper::GetInstance()->Clamp(commandTimer, 0.0f, 1.0f);
	XMFLOAT3 s_pos = pop_pos_;
	XMFLOAT3 e_pos = drop_pos_;
	m_Position = {
	Ease(In, Quart, commandTimer, s_pos.x, e_pos.x),
	Ease(In, Quart, commandTimer, s_pos.y, e_pos.y),
	Ease(In, Quart, commandTimer, s_pos.z, e_pos.z),
	};
	if (commandTimer >= 1) {
		if (m_Position.x <= -55.f || m_Position.x >= 65.f
			|| m_Position.z <= -60.f || m_Position.z >= 60.f) {
			Isdelete = true;
		}

		drop_F = false;
	}
}

void Fraction::Drop(const XMFLOAT3& dropposiition)
{
	drop_pos_ = dropposiition;
	drop_F = true;
}


void Fraction::Update(vector<InterBullet*> bullet)
{
	//エフェクト
	for (InterEffect* effect : effects) {
		if (effect != nullptr) {
			effect->Update();
		}
	}

	//マークの削除
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i] == nullptr) {
			continue;
		}

		if (!effects[i]->GetAlive()) {
			effects.erase(cbegin(effects) += i);
		}
	}

	if (m_HP <= 0.f&&effects.size()==0) {
		Isdelete = true;
	}

	ColPlayer(bullet);
	
	Pop();

	Spatter();
	
	Obj_Set();
}

void Fraction::Draw(DirectXCommon* dxCommon)
{
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	tex->Draw();
	IKETexture::PostDraw();

	IKEObject3d::PreDraw();
	m_Object->Draw();
	IKEObject3d::PostDraw();
}

void Fraction::ColPlayer(vector<InterBullet*> bullet)
{
	if (m_HP <= 0) { return; }
	//ラッシュ中判定あり
	if (Collision::SphereCollision(m_Position, m_Radius, Player::GetInstance()->GetPosition(), 1.f) &&
		(Player::GetInstance()->GetDamageInterVal() == 0))
	{
		Player::GetInstance()->RecvDamage(1.0f);
		Player::GetInstance()->PlayerHit(m_Position);
	}
	
	for (InterBullet* _bullet : bullet) {
		bool JudgColide;
		JudgColide = Collision::CircleCollision(_bullet->GetPosition().x, _bullet->GetPosition().z, m_Radius, m_Position.x, m_Position.z, m_Radius);
		if (JudgColide) {
			_bullet->SetAlive(false);
			Break();
			m_HP -= _bullet->GetPower();
		}
	}
	
}

void Fraction::Break()
{
	InterEffect* neweffect;
	neweffect = new BreakEffect();
	neweffect->Initialize();
	neweffect->SetPosition(m_Position);
	neweffect->SetDiviSpeed(1.0f);
	neweffect->SetLife(50);
	effects.push_back(neweffect);
}
