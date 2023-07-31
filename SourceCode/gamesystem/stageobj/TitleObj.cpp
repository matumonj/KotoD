#include "TItleObj.h"
#include "ModelManager.h"
#include "Helper.h"
#include "Easing.h"

TitleObj* TitleObj::GetInstance()
{
	static TitleObj instance;

	return &instance;
}

//モデル読み込み
void TitleObj::LoadResource() {
	ghostmodel = ModelManager::GetInstance()->GetModel(ModelManager::Ghost);
	foodmodel = ModelManager::GetInstance()->GetModel(ModelManager::Food);
	treemodel = ModelManager::GetInstance()->GetModel(ModelManager::TREE);
	groundmodel = ModelManager::GetInstance()->GetModel(ModelManager::Ground);
	homemodel = ModelManager::GetInstance()->GetModel(ModelManager::HOME);

	for (auto i = 0; i < ghostobj.size(); i++) {
		ghostobj[i].reset(new IKEObject3d());
		ghostobj[i]->Initialize();
		ghostobj[i]->SetModel(ghostmodel);
		ghostobj[i]->SetScale({ 0.15f,0.15f,0.15f });
	}
	ghostobj[0]->SetPosition({ -5.0f,-1.0f,-13.0f });
	ghostobj[1]->SetPosition({ -1.0f,-1.0f,-13.0f });

	for (auto i = 0; i < treeobj.size(); i++) {
		treeobj[i].reset(new IKEObject3d());
		treeobj[i]->Initialize();
		treeobj[i]->SetModel(treemodel);
		treeobj[i]->SetPosition({ (i * 20.0f) + (-30.0f),-1.0f,10.0f });
	}


	groundobj.reset(new IKEObject3d());
	groundobj->Initialize();
	groundobj->SetModel(groundmodel);
	groundobj->SetPosition({ 0.0f,15.0f,0.0f });
	groundobj->SetScale({ 0.3f,0.3f,0.3f });
	groundobj->SetColor({ 0.5f,0.5f,0.5f,1.0f });
	groundobj->SetTiling(3.0f);

	foodobj.reset(new IKEObject3d());
	foodobj->Initialize();
	foodobj->SetModel(foodmodel);
	foodobj->SetPosition({ -3.0f,-1.0f,-13.0f });
	foodobj->SetScale({ 0.12f,0.12f,0.12f });

	homeobj.reset(new IKEObject3d());
	homeobj->Initialize();
	homeobj->SetModel(homemodel);
	homeobj->SetPosition({ -7.0f,-1.0f,-5.0f });
	homeobj->SetRotation({ 0.0f,70.0f,0.0f });
	homeobj->SetScale({ 0.7f,0.7f,0.7f });

	BackSkyDome.reset(new IKEObject3d());
	BackSkyDome->Initialize();
	BackSkyDome->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Skydome));
	BackSkyDome->SetScale({ 7.f,7.f,7.f });
	BackSkyDome->SetAddOffset(3.0f);
}

//初期化
void TitleObj::Initialize() {
	for (auto i = 0; i < ghostobj.size(); i++) {
		m_GhostPos[i] = { (1.0f * i) + (-3.0f),-1.0f,(i * 1.0f) + (- 13.0f)};
		m_GhostRot[i] = { 0.0f,270.0f,-10.0f };
	}
}

//更新
void TitleObj::Update() {
	GhostUpdate();
	for (auto i = 0; i < ghostobj.size(); i++) {
		ghostobj[i]->SetPosition(m_GhostPos[i]);
		ghostobj[i]->SetRotation(m_GhostRot[i]);
		ghostobj[i]->Update();
	}
	for (auto i = 0; i < treeobj.size(); i++) {
		treeobj[i]->Update();
	}

	groundobj->Update();
	homeobj->Update();
	foodobj->SetPosition({ m_GhostPos[1].x + 0.2f,m_GhostPos[1].y + 0.3f,m_GhostPos[1].z - 0.7f});
	foodobj->Update();
	m_DomeRot.y += 0.05f;
	BackSkyDome->SetRotation(m_DomeRot);
	BackSkyDome->Update();
}

//描画
void TitleObj::Draw(DirectXCommon* dxCommon) {
	IKEObject3d::PreDraw();
	for (auto i = 0; i < ghostobj.size(); i++) {
		ghostobj[i]->Draw();
	}
	for (auto i = 0; i < treeobj.size(); i++) {
		treeobj[i]->Draw();
	}
	foodobj->Draw();
	homeobj->Draw();
	groundobj->Draw();
	//BackSkyDome->Draw();
	IKEObject3d::PostDraw();
}
//ゴーストの更新
void TitleObj::GhostUpdate() {
	m_SinAngle.x += 2.0f;
	m_SinAngle.y += 3.0f;

	//sin波によって上下に動く
	m_SinAngle2.x = m_SinAngle.x * (3.14f / 180.0f);
	m_SinAngle2.y = m_SinAngle.y * (3.14f / 180.0f);
	m_GhostPos[0].x = (sin(-m_SinAngle2.x) * 0.5f + (-6.0f));
	m_GhostPos[0].y = (sin(-m_SinAngle2.y) * 0.5f + (-1.0f));
	m_GhostPos[0].z = -10.0f;
	m_GhostPos[1].x = (sin(m_SinAngle2.x) * 0.5f + (-3.0f));
	m_GhostPos[1].y = (sin(m_SinAngle2.y) * 0.5f + (-1.0f));
	m_GhostPos[1].z = -12.0f;

	//偶に回転する
	m_RotTimer++;

	m_GhostRot[1].y = Helper::GetInstance()->DirRotation(m_GhostPos[1], { 0.0f,3.0f,-20.0f }, 270.0f);
	m_GhostRot[0].y = Helper::GetInstance()->DirRotation(m_GhostPos[0], { 0.0f,3.0f,-20.0f }, 270.0f);
	if (m_RotTimer == 50) {
		m_Rot = true;
		m_AfterRot = m_GhostRot[0].y + 360.0f;
	}

	//
	if (m_Rot) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
			m_Frame = {};
			m_RotTimer = {};
			m_GhostRot[0].y = Helper::GetInstance()->DirRotation(m_GhostPos[0], { 0.0f,3.0f,-20.0f }, 270.0f);
			m_Rot = false;
		}

		m_GhostRot[0].y = Ease(In, Cubic, m_Frame, m_GhostRot[0].y, m_AfterRot);
	}
}