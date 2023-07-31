#include "ShockWave.h"
#include "ImageManager.h"
#include "Easing.h"
#include "CsvLoader.h"
#include "Player.h"
#include "Collision.h"
void ShockWave::Initialize(const XMFLOAT3& pos) {
	tex = IKETexture::Create(ImageManager::IMPACT, { 0,0,0 }, { 12,12,12 }, { 1,1,1,0.6f });
	tex->TextureCreate();
	m_Position = pos;
	tex->SetRotation({ 90.0f, 0.0f, 0.0f });
	tex->SetScale({ 0.0f,0.0f,0.0f });

	m_Alive = true;

	m_DamagePower = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Third/Thirdboss.csv", "WaveDamage")));
}

void ShockWave::Update() {

	WideWave();
	CollideWave();
	tex->SetPosition(m_Position);
	tex->SetScale(m_Scale);
	tex->SetColor(m_Color);
	tex->Update();
}

void ShockWave::Draw(DirectXCommon* dxCommon) {
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	if (m_Alive) {
		tex->Draw();
	}
	IKETexture::PostDraw();
}
void ShockWave::ImGuiDraw() {
	ImGui::Begin("ShockWave");
	ImGui::Text("%d", m_Alive);
	ImGui::Text("%f", m_DamagRadius);
	ImGui::End();
}
//ボスによって衝撃波が走る
void ShockWave::WideWave() {
	//イージング語の各ステータス
	const float l_AfterScale = m_AfterScale;
	m_AfterRadius = m_Scale.x + 7.0f;
	const float l_AfterColor = 0.0f;
	const float l_AddFrame = 0.015f;
	if (m_Alive) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		}
		else {
			m_Frame = {};
			m_Alive = false;
		}
	}

	m_Scale = { Ease(In,Cubic,m_Frame,m_Scale.x,l_AfterScale),
	Ease(In,Cubic,m_Frame,m_Scale.y,l_AfterScale),
	Ease(In,Cubic,m_Frame,m_Scale.z,l_AfterScale) };

	m_DamagRadius = Ease(In, Cubic, m_Frame, m_DamagRadius, m_AfterRadius);
	m_Color.w = Ease(In, Cubic, m_Frame, m_Color.w, l_AfterColor);
}

bool ShockWave::CollideWave() {
	if (Player::GetInstance()->GetDamageInterVal() != 0) { return false; }
	if (m_Color.w < 0.05f) { return false; }
	if (Collision::CircleCollision(m_Position.x, m_Position.z, m_DamagRadius,
		Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().z,m_DamagRadius)) {
		Player::GetInstance()->RecvDamage(m_DamagePower);
		Player::GetInstance()->PlayerHit(m_Position);

		return true;
	}
	else {
		return false;
	}

	return false;
}
