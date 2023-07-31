#include "AngerStamp.h"
#include "Easing.h"
#include "ImageManager.h"
#include <random>
#include "Player.h"
#include "CsvLoader.h"
#include "Collision.h"
#include "Helper.h"
AngerStamp::AngerStamp() {
	
}

//初期化
bool AngerStamp::Initialize(const XMFLOAT3& pos) {
	texture.reset(IKETexture::Create(ImageManager::ANGER, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	texture->TextureCreate();
	m_Birth = true;
	m_Position = { pos.x,500.0f,pos.z };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_DamagePower = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Third/Thirdboss.csv", "StampDamage")));
	_StampState = STAMP_SET;
	m_AfterScale = m_Scale.x;

	texture->Update();
	texture->SetPosition(m_Position);
	texture->SetRotation(m_Rotation);
	texture->SetScale(m_Scale);
	texture->SetColor(m_Color);
	return true;
}

//行動
void AngerStamp::Action() {
	AngerMove();
}

//ImGui
void AngerStamp::ImGui_Origin() {
}

void AngerStamp::Origin_Draw(DirectXCommon* dxCommon) {
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);

	if (m_Alive) {
		texture->Draw();
	}
	IKETexture::PostDraw();
}

void AngerStamp::BirthParticle() {
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
	uniform_int_distribution<int> l_Randlife(20, 50);
	int l_Life = int(l_Randlife(mt));

	for (int i = 0; i < 3; i++) {
		ParticleEmitter::GetInstance()->Explosion(l_Life, m_Position, l_AddSize, s_scale, e_scale, s_color, e_color, m_ExploType);
	}
}
//当たり判定
bool AngerStamp::Collide() {
	XMFLOAT3 l_PlayerPos = Player::GetInstance()->GetPosition();
	//スタンプの大きさであたり範囲変更
	if (m_AfterScale == 1.3f) {
		m_Radius = 2.5f;
		m_ExploType = EXPLO_SMALL;
	}
	else if (m_AfterScale == 4.5f) {
		m_Radius = 8.5f;
		m_ExploType = EXPLO_MIDIUM;
	}
	else {
		m_Radius = 11.5f;
		m_ExploType = EXPLO_LARGE;
	}
	if (Collision::CircleCollision(m_Position.x, m_Position.z, m_Radius, l_PlayerPos.x, l_PlayerPos.z, m_Radius) && (Player::GetInstance()->GetDamageInterVal() == 0)) {
		Player::GetInstance()->RecvDamage(m_DamagePower);
		Player::GetInstance()->PlayerHit(m_Position);
		return true;
	}
	else {
		return false;
	}

	return false;
}

void AngerStamp::AngerMove() {
	const int l_AliveTimer = 1;
	const int l_LimitTimer = 1;
	const int l_LimitDead = 30;
	float l_AddFrame = {};
	if (m_Birth) {
		if (_StampState == STAMP_SET) {
			m_BirthTimer++;

			if (m_BirthTimer == l_AliveTimer) {
				m_Position.y = 0.0f;
				m_Alive = true;
				_StampState = STAMP_WIDE;
				m_BirthTimer = {};
			}
		}
		else if (_StampState == STAMP_WIDE) {
			l_AddFrame = 0.01f;
			if (m_Frame >= 0.6f) {
				m_Explo = true;
			}
			if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
				m_BirthTimer++;

				if (m_BirthTimer == l_LimitTimer) {
					m_Frame = {};
					m_BirthTimer = {};
				}
				
			}

			m_Scale = { Ease(In,Cubic,m_Frame,m_Scale.x,m_AfterScale),
				Ease(In,Cubic,m_Frame,m_Scale.y,m_AfterScale),
				Ease(In,Cubic,m_Frame,m_Scale.z,m_AfterScale), };
		}
		//爆発する
		if (m_Explo) {
			l_AddFrame = 0.05f;
			if (Helper::GetInstance()->FrameCheck(m_AlphaFrame, l_AddFrame)) {
				m_DeadTimer++;
				m_Alive = false;
				if (m_DeadTimer == l_LimitDead) {
					m_Birth = false;
				}
			}
			//赤に近づけつつ消す
			m_Color = {
				m_Color.x,
				m_Color.y = Ease(In, Cubic, m_AlphaFrame, 1.0f, 0.0f),
				m_Color.z = Ease(In, Cubic, m_AlphaFrame, 1.0f, 0.0f),
				m_Color.w = Ease(In, Cubic, m_AlphaFrame, 1.0f, 0.0f),
			};

			//一定のラインになったら爆発エフェクト発生
			if (m_AlphaFrame >= 0.15f) {
				if (m_Color.w >= 0.2f) {
					BirthParticle();
				}
				//爆発音
				if (m_Color.w <= 0.6f && !m_Audio) {
					Audio::GetInstance()->PlayWave("Resources/Sound/SE/Explo.wav", VolumManager::GetInstance()->GetSEVolum());
					m_Audio = true;
				}
				Collide();
			}
		}
	}
}