#include "DamageArea.h"
#include "ModelManager.h"
#include "ImageManager.h"
#include "Helper.h"
#include <Easing.h>
#include <random>
#include <CsvLoader.h>
#include "Player.h"
#include "Collision.h"
DamageArea::DamageArea(const int Num) {
	obj.resize(Num);
	m_Position.resize(Num);
	m_Rotation.resize(Num);
	m_Scale.resize(Num);
	m_Color.resize(Num);
	m_Alive.resize(Num);

	tex.resize(Num - 1);
	m_TexPosition.resize(Num - 1);
	m_TexRotation.resize(Num - 1);
	m_TexScale.resize(Num - 1);
	m_TexColor.resize(Num - 1);
	m_TexAlive.resize(Num - 1);

	model = ModelManager::GetInstance()->GetModel(ModelManager::NOTE);
	for (size_t i = 0; i < obj.size(); i++) {
		obj[i] = new IKEObject3d();
		obj[i]->Initialize();
		obj[i]->SetModel(model);
		obj[i]->SetPosition({ 0.0f,-500.0f,0.0f });
		obj[i]->SetBillboard(true);
	}

	for (size_t i = 0; i < tex.size(); i++) {
		tex[i] = IKETexture::Create(ImageManager::SLASHAREA, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 });
		tex[i]->TextureCreate();
		tex[i]->SetPosition({ 0.0f,-500.0f,0.0f });
		tex[i]->SetTiling(2.0f);
	}
}

void DamageArea::Initialize() {

	for (size_t i = 0; i < obj.size(); i++) {
		m_Scale[i] = { 0.0f,0.0f,0.0f };
		m_Color[i] = { 1.0f,0.0f,1.0f,1.0f };
		m_Rotation[i] = { 0.0f,270.0f,0.0f };
		m_Alive[i] = true;
	}

	//サイズによってラインの場所が違う
	if (obj.size() == 2) {
		//乱数指定
		for (size_t i = 0; i < obj.size(); i++) {
			mt19937 mt{ std::random_device{}() };
			uniform_int_distribution<int> l_distX(-60, 65);
			uniform_int_distribution<int> l_distZ(-60, 60);
			m_Position[i] = { float(l_distX(mt)),0.0f,float(l_distZ(mt)) };
		}
	}
	else if (obj.size() == 4) {
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_distX(-50, -20);
		uniform_int_distribution<int> l_distZ(20, 50);
		m_Position[0] = { float(l_distX(mt)),0.0f,float(l_distZ(mt)) };
		uniform_int_distribution<int> l_distX2(-50, -20);
		uniform_int_distribution<int> l_distZ2(-50, -20);
		m_Position[1] = { float(l_distX2(mt)),0.0f,float(l_distZ2(mt)) };
		uniform_int_distribution<int> l_distX3(20, 50);
		uniform_int_distribution<int> l_distZ3(-50, -20);
		m_Position[2] = { float(l_distX3(mt)),0.0f,float(l_distZ3(mt)) };
		uniform_int_distribution<int> l_distX4(20, 50);
		uniform_int_distribution<int> l_distZ4(20, 50);
		m_Position[3] = { float(l_distX4(mt)),0.0f,float(l_distZ4(mt)) };
	}

	for (size_t i = 0; i < tex.size(); i++) {
		m_TexColor[i] = { 1.0f,1.0f,1.0f,0.0f };
		m_TexRotation[i] = { 90.0f,0.0f,0.0f };
		m_TexScale[i] = { 0.4f,3.0f,1.0f };
		m_TexAlive[i] = true;
	}

	m_VanishLimit = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "LineLimit")));
	m_Damage = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Six/Sixboss.csv", "LineDamage")));
}

void DamageArea::Update() {
	StateManager();
	LineUpdate();
	PointUpdate();
	Collide();
	for (int i = 0; i < obj.size(); i++) {
		if (obj[i] == nullptr) {
			continue;
		}

		if (!m_Alive[i]) {
			obj.erase(cbegin(obj) + i);
		}
	}

	for (int i = 0; i < tex.size(); i++) {
		if (tex[i] == nullptr) {
			continue;
		}

		if (!m_TexAlive[i]) {
			tex.erase(cbegin(tex) + i);
		}
	}
}

void DamageArea::Draw(DirectXCommon* dxCommon) {
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	for (size_t i = 0; i < tex.size(); i++) {
		if (tex[i] != nullptr) {
			tex[i]->Draw();
		}
	}
	IKETexture::PostDraw();
	IKEObject3d::PreDraw();
	for (size_t i = 0; i < obj.size(); i++) {
		if (obj[i] != nullptr && m_Alive[i]) {
			obj[i]->Draw();
		}
	}
	IKEObject3d::PostDraw();
}

void DamageArea::ImGuiDraw() {
}

void DamageArea::LineUpdate() {
	for (size_t i = 0; i < tex.size(); i++) {
		m_TexPosition[i] = {
		m_TexPosition[i].x = (m_Position[i].x + m_Position[i + 1].x) / 2,
		0.0f,
		m_TexPosition[i].z = (m_Position[i].z + m_Position[i + 1].z) / 2,
		};

		m_TexRotation[i].y = Helper::GetInstance()->DirRotation(m_TexPosition[i], m_Position[i], -PI_180);

		m_TexScale[i].y = Helper::GetInstance()->ChechLength(m_Position[i], m_Position[i + 1]) * 0.1f;
		m_OffSet.x = -0.01f;
		if (tex[i] != nullptr) {
			tex[i]->Update();
			tex[i]->SetAddOffset(m_OffSet);
			tex[i]->SetPosition(m_TexPosition[i]);
			tex[i]->SetRotation(m_TexRotation[i]);
			tex[i]->SetScale(m_TexScale[i]);
			tex[i]->SetColor({ m_TexColor[i].x,m_TexColor[i].y,m_TexColor[i].z,m_Alpha});
		}
	}
}

void DamageArea::PointUpdate() {
	
	for (size_t i = 0; i < obj.size(); i++) {
		
		if (obj[i] != nullptr) {
			obj[i]->Update();
			obj[i]->SetPosition(m_Position[i]);
			obj[i]->SetRotation(m_Rotation[i]);
			obj[i]->SetScale({ m_CommonScale, m_CommonScale, m_CommonScale });
			obj[i]->SetColor(m_Color[i]);
		}
	}
}

void DamageArea::StateManager() {
	const float l_AddFrame = 0.03f;
	if (m_AreaState == POINT_BIRTH) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		}
		else {
			m_Frame = {};
			m_AreaState = LINE_BIRTH;
		}
		m_CommonScale = Ease(In, Cubic, m_Frame, m_CommonScale, m_AfterScale);
	}
	else if (m_AreaState == LINE_BIRTH) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		}
		else {
			m_Frame = {};
			m_AreaState = STAY;
		}
		m_Alpha = Ease(In, Cubic, m_Frame, m_Alpha, m_AfterAlpha);
	}
	else if (m_AreaState == STAY) {
		m_StayTimer++;
		if (m_StayTimer > m_VanishLimit) {
			m_AreaState = VANISH_AREA;
			m_AfterAlpha = 0.0f;
			m_AfterScale = 0.0f;
		}
	}
	else {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		}
		else {
			m_Frame = {};
			m_AreaState = POINT_BIRTH;
			for (size_t i = 0; i < tex.size(); i++) {
				m_TexAlive[i] = false;
			}
			for (size_t i = 0; i < obj.size(); i++) {
				m_Alive[i] = false;
			}
		}
		m_CommonScale = Ease(In, Cubic, m_Frame, m_CommonScale, m_AfterScale);
		m_Alpha = Ease(In, Cubic, m_Frame, m_Alpha, m_AfterAlpha);
	}
}

bool DamageArea::Collide() {
	Line2D lines;
	Point points;
	points = { Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().z };
	for (size_t i = 0; i < tex.size(); i++) {
		//////////////
		lines.start = { m_Position[i].x,m_Position[i].z };
		lines.end = { m_Position[i + 1].x,m_Position[i + 1].z };
		//判定部
		if (Collision::IsCollidingLineAndCircle(lines, points, 5.0f) && (Player::GetInstance()->GetDamageInterVal() == 0) && (m_Alpha == 1.0f))
		{
			Player::GetInstance()->PlayerHit(m_TexPosition[i]);
			Player::GetInstance()->RecvDamage(m_Damage);
			break;
		}
		else {
		}
	}

	return true;
}