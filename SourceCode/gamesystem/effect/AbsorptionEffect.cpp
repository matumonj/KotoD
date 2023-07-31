#include "AbsorptionEffect.h"
#include "ImageManager.h"
#include "Helper.h"
#include "Easing.h"
#include <random>
#define WindowW 1920
#define WindowH 1020
//�ǂݍ���
AbsorptionEffect::AbsorptionEffect() {
	tex.reset(IKETexture::Create(ImageManager::CATCH, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	tex->TextureCreate();
	tex->SetRotation({ 0,0,0 });
	tex->SetScale(m_Scale);
	tex->SetIsBillboard(true);
}
//������
void AbsorptionEffect::Initialize() {

	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandScale(10, 30);
	uniform_int_distribution<int> l_RandSpeed(100, 360);

	m_CircleSpeed = float(l_RandSpeed(mt));
	m_CircleScale = float(l_RandScale(mt));
	m_Position = Helper::GetInstance()->CircleMove({ m_BasePos.x,m_BasePos.y + 1.0f,m_BasePos.z }, m_CircleScale, m_CircleSpeed);

	m_Alive = true;
}
//�X�V
void AbsorptionEffect::Update() {
	//�p�[�e�B�N������
	MoveEffect();
}
//�`��
void AbsorptionEffect::Draw(DirectXCommon* dxCommon) {
	IKETexture::PreDraw2(dxCommon, AddBlendType);
	if (m_Alive) {
		tex->Draw();
	}
	IKETexture::PostDraw();
}
//ImGui
void AbsorptionEffect::ImGuiDraw() {
	
}

//�X�^�����G�t�F�N�g
void AbsorptionEffect::MoveEffect() {
	//�X�^�����̂҂�҂�
	if (m_Alive) {
		m_CircleSpeed += 10.0f;

		if (m_Frame < m_FrameMax) {
			m_Frame += m_AddFrame;
		}
		else {
			m_Frame = {};
			m_Alive = false;
		}

		m_CircleScale = Ease(In, Cubic, m_Frame, m_CircleScale, 0.0f);

		m_Scale = { Ease(In,Cubic,m_Frame,m_Scale.x,0.0f),
			Ease(In,Cubic,m_Frame,m_Scale.y,0.0f),
			Ease(In,Cubic,m_Frame,m_Scale.z,0.0f),
		};
	}
	m_Position = Helper::GetInstance()->CircleMove({ m_BasePos.x,m_BasePos.y + 1.0f,m_BasePos.z }, m_CircleScale, m_CircleSpeed);
	tex->SetPosition(m_Position);
	tex->SetScale(m_Scale);
	tex->SetColor(m_Color);
	tex->Update();
}