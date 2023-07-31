#include "EnemyManager.h"
#include "Helper.h"

EnemyManager::EnemyManager(const std::string& SceneName) {

	m_SceneName = SceneName;
	//シーンによって読み込むボスが違う
	if (m_SceneName == "FIRSTSTAGE") {
		boss.reset(new FirstBoss());
		boss->Initialize();
	} else if (m_SceneName == "SECONDSTAGE") {
		boss = make_unique < SecondBoss>();
		boss->Initialize();
		bulletenemy.resize(thirdEnemyMax);
		for (auto i = 0; i < bulletenemy.size(); i++) {
			bulletenemy[i] = new NormalEnemy();
			bulletenemy[i]->Initialize();
		}
	} else if (m_SceneName == "THIRDSTAGE") {
		boss = make_unique<ThirdBoss>();
		boss->Initialize();

	} else if (m_SceneName == "FOURTHSTAGE") {
		boss = make_unique <FourthBoss>();
		boss->Initialize();
	} else if (m_SceneName == "FIVESTAGE") {
		boss = make_unique <FiveBoss>();
		boss->Initialize();
	}
	else if (m_SceneName == "SIXSTAGE") {
		boss = make_unique<SixBoss>();
		boss->Initialize();
	}
	else if (m_SceneName == "SEVENSTAGE") {
		boss = make_unique<SevenBoss>();
		boss->Initialize();
	}
	else if (m_SceneName == "TUTORIAL") {
		for (auto i = 0; i < tutorialenemy.size(); i++) {
			tutorialenemy[i] = make_unique<TutorialEnemy>();
			tutorialenemy[i]->Initialize();
		}
		//最初の敵のみ固定湧き
		tutorialenemy[0]->SetPosition({ 0,5.0f ,50.0f });
		tutorialenemy[0]->SetUnrival(true);
		//カメラ追従しつつスポーン
		for (auto i = 1; i < tutorialenemy.size(); i++) {
			float posX = -40.0f + (i * 20.f);
			tutorialenemy[i]->SetPosition({ posX ,5.0f ,50.0f });
			tutorialenemy[i]->SetUnrival(true);
		}
	} else {
		assert(0);
	}
}
void EnemyManager::Initialize(DirectXCommon* dxCommon) {
}
void EnemyManager::SkipInitialize() {
	boss->SkipInitialize();
}
//バトル更新
void EnemyManager::BattleUpdate() {
	boss->Update();
	if (m_SceneName == "SECONDSTAGE") {
		boss->isRespawn(bulletenemy);
		boss->SummonEnemyUpda(bulletenemy);
	} 
}
//登場シーン
void EnemyManager::AppearUpdate() {
	boss->AppearUpdate();
}
//撃破
void EnemyManager::DeadUpdate() {
	boss->DeadUpdate();
}
void EnemyManager::TutorialUpdate(int pattern) {
	if (pattern == 0) {
		tutorialenemy[0]->Update();
		tutorialenemy[0]->SetIsWeak(true);
	} else if (pattern == 1) {
		for (auto i = 0; i < tutorialEnemyMax; i++) {
			tutorialenemy[i]->Update();
		}
	} else {
		for (auto i = 0; i < tutorialEnemyMax; i++) {
			tutorialenemy[i]->Obj_SetParam();
		}
	}
}
//描画
void EnemyManager::Draw(DirectXCommon* dxCommon) {
	boss->Draw(dxCommon);
	if (m_SceneName == "SECONDSTAGE") {
		boss->SummonEnemyDraw(bulletenemy, dxCommon);
	}
}
//描画
void EnemyManager::SpriteDraw(DirectXCommon* dxCommon) {

}
void EnemyManager::TutorialDraw(DirectXCommon* dxCommon) {
	for (auto i = 0; i < tutorialEnemyMax; i++) {
		tutorialenemy[i]->Draw(dxCommon);
	}

}
//ImGui
void EnemyManager::ImGuiDraw() {
	boss->ImGuiDraw();
}

//敵の死亡処置
bool EnemyManager::BossDestroy() {
	if (boss->GetHP() <= 0.0f) {
		return true;
	} else {
		return false;
	}

	return false;
}

bool EnemyManager::AllDeadEnemy() {
	for (auto i = 0; i < tutorialEnemyMax; i++) {
		if (tutorialenemy[i]->GetisAlive()) { return false; }
	}
	return true;
}

//敵のチェック終了
void EnemyManager::FinishCheck() {
	boss->SetCheck(false);
}

//ハート生成の終了
void EnemyManager::FinishHeart() {
	boss->SetBirthHeart(false);
}

//敵の向き(2個めのボス)
void EnemyManager::DirSet(int dir) {
	boss->SetDirEmo(dir);
}

//ボスのHP回復
void EnemyManager::HealHP(const float power) {
	boss->SetHP(boss->GetHP() + power);
}
//覚醒
void EnemyManager::Awake() {
	boss->AwakeUpdate();
}
void EnemyManager::DeleteObj() {
	boss->SetDeleteObj(true);
}

void EnemyManager::UpdateStop()
{
	boss->SetStateReset(true);
}
