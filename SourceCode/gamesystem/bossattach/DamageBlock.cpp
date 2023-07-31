#include "DamageBlock.h"
#include "ModelManager.h"
#include "ParticleEmitter.h"
#include "Player.h"
#include "Collision.h"
#include "CsvLoader.h"
#include "Easing.h"
#include "Helper.h"
#include "ImageManager.h"
#include <random>
DamageBlock::DamageBlock() {
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::Nucleus);
	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);

	tex.reset(IKETexture::Create(ImageManager::DAMAGEAREA, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	tex->TextureCreate();
	tex->SetPosition({ 0.0f,-500.0f,0.0f });
	tex->SetTiling(2.0f);
}

bool DamageBlock::Initialize() {
	m_Position = { 500.0f,5.0f,0.0f };
	m_Scale = { 0.6f,0.6f,0.6f };
	m_Color = { 1.0f,1.0f,1.0f,0.0f };
	m_Frame = {};
	m_AttackDir = {};
	m_Alive = true;
	m_Damage = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "FireDamage")));
	_BlockState = BLOCK_SET;

	m_TexColor = { 1.0f,1.0f,1.0f,0.0f };
	m_TexRot = { 90.0f,0.0f,0.0f };
	m_TexScale = { 0.2f,3.0f,0.6f };
	return true;
}
//更新
void DamageBlock::Update() {
	Move();
	BirthParticle();
	Collide();
	Obj_SetParam();
}
//描画
void DamageBlock::Draw(DirectXCommon* dxCommon) {
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	tex->Draw();
	IKETexture::PostDraw();
	Obj_Draw();
}
//ImGui
void DamageBlock::ImGuiDraw() {
	ImGui::Begin("Block");
	ImGui::Text("Alpha:%f", m_Alpha);
	ImGui::Text("Frame:%f", m_Frame);
	ImGui::End();
}
//当たり判定
bool DamageBlock::Collide() {
	XMFLOAT3 l_PlayerPos = Player::GetInstance()->GetPosition();

	const float l_Radius = 1.0f;
	if (Collision::CircleCollision(m_Position.x, m_Position.z, l_Radius, l_PlayerPos.x, l_PlayerPos.z, l_Radius) && (Player::GetInstance()->GetDamageInterVal() == 0)) {
		Player::GetInstance()->RecvDamage(m_Damage);
		Player::GetInstance()->PlayerHit(m_Position);
		return true;
	}
	else {
		return false;
	}

	return false;
}
//動き
void DamageBlock::Move() {
	const float l_AddFrame = 0.005f;

	if (_BlockState == BLOCK_SET) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = {};
			m_AfterAlpha = {};
			_BlockState = BLOCK_PLAY;
		}
	}
	else {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = {};
			m_Alive = false;
		}
		m_Position = {
			Ease(In,Cubic,m_Frame,m_Position.x,m_AfterPos.x),
			Ease(In,Cubic,m_Frame,m_Position.y,m_AfterPos.y),
			Ease(In,Cubic,m_Frame,m_Position.z,m_AfterPos.z),
		};
	}

	m_Alpha = Ease(In, Cubic, m_Frame, m_Alpha, m_AfterAlpha);
	m_TexPos = {
	m_TexPos.x = (m_Position.x + m_AfterPos.x) / 2,
	0.0f,
	m_TexPos.z = (m_Position.z + m_AfterPos.z) / 2,
	};

	m_TexScale.y = Helper::GetInstance()->ChechLength(m_Position, m_AfterPos) * 0.1f;
	tex->Update();
	tex->SetPosition(m_TexPos);
	tex->SetRotation(m_TexRot);
	tex->SetScale(m_TexScale);
	tex->SetColor({ m_TexColor.x,m_TexColor.y,m_TexColor.z,m_Alpha });
}
//パーティクル
void DamageBlock::BirthParticle() {
	XMFLOAT4 s_color = { 1.0f,0.0f,0.0f,1.0f };
	XMFLOAT4 e_color = { 1.0f,1.0f,1.0f,1.0f };
	float s_scale = 4.0f;
	float e_scale = 0.0f;
	const int m_Life = 50;
	if (m_Alive) {
		ParticleEmitter::GetInstance()->FireEffect(m_Life, m_Position, s_scale, e_scale, s_color, e_color);
	}
}
void DamageBlock::InitPos(const int Num, const XMFLOAT3& Pos) {
	
	if (m_AttackDir == DIR_UP) {
		m_Position = { Pos.x + (Num * 3.0f),5.0f,60.0f};
		m_AfterPos = { m_Position.x,m_Position.y,-60.0f };
	}
	else if (m_AttackDir == DIR_DOWN) {
		m_Position = { Pos.x + (Num * 3.0f),5.0f,-60.0f };
		m_AfterPos = { m_Position.x,m_Position.y,60.0f };
	}
	else if (m_AttackDir == DIR_RIGHT) {
		m_Position = { 65.0f,5.0f,Pos.z + (Num * 3.0f) };
		m_AfterPos = { -55.0f,m_Position.y,m_Position.z };
	}
	else {
		m_Position = { -55.0f,5.0f,Pos.z + (Num * 3.0f) };
		m_AfterPos = { 65.0f,m_Position.y,m_Position.z };
	}

	
	m_TexRot.y = Helper::GetInstance()->DirRotation(m_Position, m_AfterPos, -PI_180);
}