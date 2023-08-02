#include "FireBoll.h"
#include "ModelManager.h"
#include "ImageManager.h"
#include "ParticleEmitter.h"
#include "Player.h"
#include "Collision.h"
#include "CsvLoader.h"
#include "Easing.h"
#include "Helper.h"
FireBoll::FireBoll() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Nucleus);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);

	tex.reset(IKETexture::Create(ImageManager::DARKBOM, { 0,0,0 }, { 12,12,12 }, { 1,1,1,0.6f }));
	tex->TextureCreate();
	tex->SetScale({ 2.0f,2.0f,2.0f });
	tex->SetRotation({ 90.0f, 0.0f, 0.0f });
}

bool FireBoll::Initialize() {
	m_Position = { 500.0f,8.0f,0.0f };
	m_Scale = { 20.0f,20.0f,20.0f };
	m_Color = { 1.0f,0.0f,0.0f,1.0f };
	m_TexPos = { 0.0f,0.0f,0.0f };
	m_CircleScale = 20.0f;
	m_CircleSpeed = 0.0f;
	_MoveState = MOVE_SET;
	m_Alive = true;
	m_Damage = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "FireDamage")));
	effects.clear();
	return true;

}
//更新
void FireBoll::Update() {
	Move();
	//BirthParticle();

	Obj_SetParam();
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
	if (m_Color.w == 1.0f) {
		CollideBul(_playerBulA);
		Collide();
	}
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
}
//描画
void FireBoll::Draw(DirectXCommon* dxCommon) {
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	if (m_TexAlive) {
		tex->Draw();
	}
	IKETexture::PostDraw();

	Obj_Draw();
}
//ImGui
void FireBoll::ImGuiDraw() {
}
//当たり判定
bool FireBoll::Collide() {
	XMFLOAT3 l_PlayerPos = Player::GetInstance()->GetPosition();
	if (m_Position.y != 3.0f)return false;
	const float l_Radius = 3.0f;
	if (Collision::CircleCollision(m_Position.x, m_Position.z, l_Radius, l_PlayerPos.x, l_PlayerPos.z, l_Radius) && (Player::GetInstance()->GetDamageInterVal() == 0)) {
		Player::GetInstance()->RecvDamage(m_Damage);
		Player::GetInstance()->PlayerHit(m_Position);
		m_Frame = {};
		_MoveState = MOVE_END;
		return true;
	}
	else {
		return false;
	}

	return false;
}
//動き
void FireBoll::Move() {
	XMFLOAT3 l_PlayerPos = Player::GetInstance()->GetPosition();
	const float l_AddSpeed = 2.0f;
	float l_AddFrame = 0.1f;

	if (_MoveState == MOVE_SET) {		//落下マークが出てくる
		m_TexAlive = true;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			_MoveState = MOVE_PLAY;
			m_Frame = {};
		}
		m_TexColor.w = Ease(In, Cubic, m_Frame, m_TexColor.w, 1.0f);
		m_Position.y = 60.0f;
		m_Position = Helper::GetInstance()->CircleMove({ l_PlayerPos.x,m_Position.y,l_PlayerPos.z }, m_CircleScale, m_CircleSpeed);
		m_TexPos = { m_Position.x,0.0f,m_Position.z };
	}else if (_MoveState == MOVE_PLAY) {		//ブロックが落ちてくる
		m_MoveTimer++;
		if (m_MoveTimer == m_LimitTimer) {
			m_AfterPos.y = 3.0f;
			_MoveState = MOVE_OMEN;
			m_MoveTimer = {};
			m_Frame = {};
			m_TargetPos = { l_PlayerPos.x,m_Position.y,l_PlayerPos.z };
		}
		m_Position = Helper::GetInstance()->CircleMove({ l_PlayerPos.x,m_Position.y,l_PlayerPos.z }, m_CircleScale, m_CircleSpeed);
		m_TexPos = { m_Position.x,1.0f,m_Position.z };
	}
	else if (_MoveState == MOVE_OMEN) {			//予兆
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_MoveTimer++;
			m_TexAlive = false;
			if (m_MoveTimer == 200 && m_Delete) {
				m_Frame = {};
				_MoveState = MOVE_END;
			}
			if (m_MoveTimer == 300) {
				_MoveState = MOVE_ATTACK;
				m_Frame = {};
				m_MoveTimer = {};
				m_Color.x = 1.0f;
			}
		}
		m_Position.y = Ease(In, Cubic, m_Frame, m_Position.y, m_AfterPos.y);
		m_Position = Helper::GetInstance()->CircleMove({ m_TargetPos.x,m_Position.y,m_TargetPos.z }, m_CircleScale, m_CircleSpeed);

		m_SinAngle += 6.0f;
		m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
		m_Color.x = (sin(m_SinAngle2) * 1.0f + 1.0f);
	}
	else if (_MoveState == MOVE_ATTACK) {			//攻撃
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = {};
			_MoveState = MOVE_END;
		}

		m_CircleScale = Ease(In, Cubic, m_Frame, m_CircleScale, 1.0f);
		m_Position = Helper::GetInstance()->CircleMove({ m_TargetPos.x,m_Position.y,m_TargetPos.z }, m_CircleScale, m_CircleSpeed);
	}
	else {
		l_AddFrame = 0.05f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = {};
			m_Alive = false;
		}

		m_Color.w = Ease(In,Cubic,m_Frame,m_Color.w,0.0f);
	}

	tex->SetPosition(m_TexPos);
	tex->SetColor(m_TexColor);
	tex->Update();
}
//パーティクル
void FireBoll::BirthParticle() {
	XMFLOAT4 s_color = { 1.0f,0.4f,0.0f,1.0f };
	XMFLOAT4 e_color = { 1.0f,1.0f,1.0f,1.0f };
	float s_scale = 4.0f;
	float e_scale = 0.0f;
	const int m_Life = 50;
	if (m_Alive) {
		ParticleEmitter::GetInstance()->FireEffect(m_Life, m_Position, s_scale, e_scale, s_color, e_color);
	}
}


//弾との当たり判定
void FireBoll::CollideBul(vector<InterBullet*> bullet)
{
	if (m_Position.y != 3.0f)return;
	const float l_Radius = 3.0f;
	for (InterBullet* _bullet : bullet) {
		if (_bullet != nullptr && _bullet->GetAlive()) {
			if (Collision::CircleCollision(_bullet->GetPosition().x, _bullet->GetPosition().z, _bullet->GetRadius(), m_Position.x, m_Position.z, l_Radius)) {
				BirthEffect();
				_bullet->SetAlive(false);
				m_HP -= 1.0f;
				if (m_HP <= 0.0f) {
					m_Frame = {};
					_MoveState = MOVE_END;
				}
			}
		}
	}
}

//エフェクトの発生
void FireBoll::BirthEffect() {
	InterEffect* neweffect;
	neweffect = new BreakEffect();
	neweffect->Initialize();
	neweffect->SetPosition(m_Position);
	neweffect->SetDiviSpeed(1.0f);
	neweffect->SetLife(50);
	neweffect->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	effects.push_back(neweffect);
}
