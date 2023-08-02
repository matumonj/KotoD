#include "LoadStageObj.h"
#include "CsvLoader.h"
#include "HungerGauge.h"
#include "Collision.h"
#include "Helper.h"
#include "Player.h"
#include "VariableCommon.h"
#include <Easing.h>
EnemyManager* LoadStageObj::m_EnemyManager = nullptr;
//ゴーストのロード
void LoadStageObj::GhostLoad() {
	auto Size = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/ghost/ghost.csv", "Quantity")));

	ghosts.resize(Size);

	/**/
	Pos.resize(Size);
	Rot.resize(Size);
	Scl.resize(Size);

	m_HealPower = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/Seven/Sevenboss.csv", "HealPower")));
}
//食べ物ロード
void LoadStageObj::FoodLoad(const std::string& sceneName) {
	size_t Size;
	if (sceneName == "FIRSTSTAGE") {
		Size = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/food/food.csv", "Stage1")));
	} else if (sceneName == "THIRDSTAGE") {
		Size = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/food/food.csv", "Stage2")));
	} else if (sceneName == "TUTORIAL") {
		Size = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/food/food.csv", "Stage1")));
	} else {
		assert(0);
	}

	foods.resize(Size);

	/**/
	Pos.resize(Size);
	Rot.resize(Size);
	Scl.resize(Size);
}
//すべてロード
void LoadStageObj::AllLoad(const std::string& sceneName) {
	m_SceneName = sceneName;
	//ゴースト関係
	GhostLoad();
	//初期化
	for (auto i = 0; i < ghosts.size(); i++) {
		ghosts[i] = new Ghost();
		ghosts[i]->Initialize();
	}
	//食べ物関係
	FoodLoad(sceneName);

	//初期化
	for (auto i = 0; i < foods.size(); i++) {
		foods[i] = new Food();
		foods[i]->Initialize();
	}
}
//ライトセット(食べ物に使う)
void LoadStageObj::LightSet(LightGroup* light) {
	lightgroup = light;
	for (auto i = 0; i < foods.size(); i++) {
		lightgroup->SetCircleShadowActive(i + 2, true);
	}
}
//初期化
void LoadStageObj::Initialize() {
	//Load();
}
void LoadStageObj::TutorialUpdate() {

	CommonUpdate();
}
//更新(ステージ1)
void LoadStageObj::FirstUpdate() {
	//更新
	CommonUpdate();
}

//更新
void LoadStageObj::SecondUpdate() {
	//更新
	CommonUpdate();
}
//3
void LoadStageObj::ThirdUpdate() {
	//更新
	CommonUpdate();
	//こっから特有の処理
	//食料の削除(このステージのみ)
	for (int i = 0; i < foods.size(); i++) {
		if (foods[i] == nullptr) {
			continue;
		}

		if (!foods[i]->GetAlive()) {
			foods.erase(cbegin(foods) + i);
		}
	}

	//食料生成
	if (m_EnemyManager->GetEnemyCheck() && (foods.size() < 5)) {
		Food* newFood;
		newFood = new Food();
		newFood->Initialize();
		newFood->SetPosition({ m_EnemyManager->GetEnemyPosition().x,0.0f,m_EnemyManager->GetEnemyPosition().z });
		//newFood->SetLimit(true);
		foods.push_back(newFood);
		m_EnemyManager->FinishCheck();

		for (int i = 0; i < foods.size(); i++) {
			foods[i]->SetLightSet(true);
		}
	}
}
//4
void LoadStageObj::FourthUpdate() {
	//更新
	CommonUpdate();
	FourthBossAction();

}
//5
void LoadStageObj::FiveUpdate() {
	//更新
	CommonUpdate();
}
//6
void LoadStageObj::SixUpdate() {
	//更新
	CommonUpdate();
	SubHunger();
}
//7
void LoadStageObj::SevenUpdate() {
	//更新
	CommonUpdate();
	//吸収
	Absorption();
	//回復
	CollideBoss();
	//操っている
	Manipulate();
}
//描画
void LoadStageObj::Draw(DirectXCommon* dxCommon) {
	//ゴースト
	for (auto i = 0; i < ghosts.size(); i++) {
		ghosts[i]->Draw(dxCommon);
	}
	//
	//食べ物
	for (auto i = 0; i < foods.size(); i++) {
		foods[i]->Draw(dxCommon);
	}

	//ハート
	for (auto i = 0; i < hearts.size(); i++) {
		hearts[i]->Draw(dxCommon);
	}
	//
}
//ImGui
void LoadStageObj::ImGuiDraw() {
}
//当たり判定(ゴースト)
void LoadStageObj::Collide() {
	for (auto i = 0; i < ghosts.size(); ++i) {
		for (auto j = 0; j < ghosts.size(); ++j) {
			XMFLOAT3 ghostpos = ghosts[i]->GetPosition();
			XMFLOAT3 ghostpos2 = ghosts[j]->GetPosition();
			if ((i == j)) { continue; }
			if ((!ghosts[i]->GetAlive()) || (!ghosts[j]->GetAlive())) { continue; }
			if ((!ghosts[i]->GetFollow()) || (!ghosts[j]->GetFollow())) { continue; }
			if (Collision::SphereCollision(ghostpos, 1.5f, ghostpos2, 1.5f)) {
				ghosts[i]->GhostCollision(ghostpos2);
				ghosts[j]->GhostCollision(ghostpos);
			}
		}
	}
}
//食料の検索
void LoadStageObj::SearchFood() {
	for (auto i = 0; i < ghosts.size(); i++) {
		XMFLOAT3 l_ghostpos = ghosts[i]->GetPosition();
		for (auto j = 0; j < foods.size(); j++) {
			if (!ghosts[i]->GetAlive()) { continue; }
			if (!ghosts[i]->GetCatch()) { continue; }
			if (ghosts[i]->GetFollow()) { continue; }
			XMFLOAT3 l_foodpos = foods[j]->GetPosition();
			float l_dir = Helper::GetInstance()->ChechLength(l_ghostpos, l_foodpos);
			if ((!ghosts[i]->GetSearch()) && (foods[j]->GetAlive()) && (!foods[j]->GetLockOn()) && (!foods[j]->GetIsCarried())) {
				if (l_dir < ghosts[i]->GetLimit()) {
					ghosts[i]->StartSearch(l_foodpos);
					foods[j]->SetLockOn(true);
				} else {
					ghosts[i]->SetLimit(ghosts[i]->GetLimit() + 3.0f);
				}
			}
		}
	}
}
//食料とゴーストの当たり判定
void LoadStageObj::CollideFood() {
	float l_Radius = 1.5f;
	for (auto i = 0; i < ghosts.size(); i++) {
		XMFLOAT3 l_ghostpos = ghosts[i]->GetPosition();
		for (auto j = 0; j < foods.size(); j++) {
			XMFLOAT3 l_foodpos = foods[j]->GetPosition();
			if (ghosts[i]->GetFollow()) { continue; }
			float l_dir = Helper::GetInstance()->ChechLength(l_ghostpos, l_foodpos);
			if ((ghosts[i]->GetSearch()) && (l_dir < l_Radius)) {
				ghosts[i]->EndSearch();
				foods[j]->CarryStart(ghosts[i]);
			}
		}
	}
}
//ゴーストが消える
void LoadStageObj::VanishGhost() {
	int l_TargetCatchCount = HungerGauge::GetInstance()->GetCatchCount() - 1;
	float l_Value = HungerGauge::m_Hungervalue;
	//除算をする
	m_Division = HungerGauge::GetInstance()->GetNowHunger() / 5.0f;
	for (auto i = 0; i < ghosts.size(); ++i) {
		if (ghosts[i]->GetVanish()) { continue; }
		if (!ghosts[i]->GetAlive()) { continue; }
		if (!ghosts[i]->GetCatch()) { continue; }
		if (!ghosts[i]->GetFollow()) { continue; }
		if (ghosts[i]->GetManipulate()) { continue; }
		//特定の値を下回ったら
		if (m_Division <= l_TargetCatchCount) {
			m_Vanish = true;
		}

		//for分抜ける
		if (m_Vanish) {
			ghosts[i]->SetVanish(true);
			HungerGauge::GetInstance()->SetCatchCount(HungerGauge::GetInstance()->GetCatchCount() - 1);
			HungerGauge::GetInstance()->SetHungerMax(HungerGauge::GetInstance()->GetHungerMax() - l_Value);
			m_Vanish = false;
			break;
		}
	}
}
//共通の更新
void LoadStageObj::CommonUpdate() {
	//ゴースト
	for (auto i = 0; i < ghosts.size(); i++) {
		ghosts[i]->Update();
	}

	//
	//食べ物
	for (auto i = 0; i < foods.size(); i++) {
		foods[i]->Update();
	}

	//ハート
	for (auto i = 0; i < hearts.size(); i++) {
		hearts[i]->Update();
	}

	//食料の削除(このステージのみ)
	for (int i = 0; i < hearts.size(); i++) {
		if (hearts[i] == nullptr) {
			continue;
		}

		if (!hearts[i]->GetAlive()) {
			hearts.erase(cbegin(hearts) + i);
		}
	}

	//食料のサイズによって探索型を出せるかどうか決まる
	if (foods.size() != 0) {
		Player::GetInstance()->SetCanSearch(true);
	}
	else {
		Player::GetInstance()->SetCanSearch(false);
	}
	//
	//当たり判定
	Collide();
	//食べ物の当たり判定
	CollideFood();
	//食べ物の検索
	SearchFood();
	//ゴーストが消える
	VanishGhost();
	//ハートの生成
	BirthHeart();
	//ライト生成
	LightReturn();
}
//ハートの生成
void LoadStageObj::BirthHeart() {
	if (!m_EnemyManager->GetBoss()) { return; }
	if (m_EnemyManager->GetBirthHeart()) {
		Heart* newHeart;
		newHeart = new Heart();
		newHeart->Initialize();
		newHeart->SetPosition({ m_EnemyManager->GetEnemyPosition().x,0.0f,m_EnemyManager->GetEnemyPosition().z });
		hearts.push_back(newHeart);
		m_EnemyManager->FinishHeart();
		/*for (int i = 0; i < hearts.size(); i++) {
			if (m_SceneName == "FIRSTSTAGE") {
				lightgroup->SetCircleShadowActive(((int)hearts.size() + 1) + 12, true);
			}
			else if (m_SceneName == "SECONDSTAGE") {
				lightgroup->SetCircleShadowActive(i + 2, true);
			}
		}*/
	}
}

void LoadStageObj::LightReturn() {
}

void LoadStageObj::FourthBossAction() {
	LockVerseGhost();
	LockAllGhost();
	NonVerseGhost();
	CheckReferGhost();
	ChangeGhost2Enemy();
	ChangeGhost2Hyper();
	SubHunger();
}

void LoadStageObj::LockVerseGhost() {
	InterBoss* boss = m_EnemyManager->GetBoss();
	if (!boss->GetSearch()) { return; }
	if (boss->GetStrong()) {
		kStopGhorstMax = 5;
	} else {
		kStopGhorstMax = 3;
	}
	int  nowStopGhorst = 0;
	while (nowStopGhorst < kStopGhorstMax) {
		if (boss->GetLimit() > 150.0f) {
			boss->SetIsMiss(true);
			boss->SetLimit(20.0f);
			ReferGhorstReseted();
			break;
		}
		for (auto i = 0; i < ghosts.size(); i++) {
			if (!ghosts[i]->GetAlive()) { continue; }
			if (ghosts[i]->GetIsRefer()) { continue; }
			//キャラステート変える際に気をつけてください
			if (ghosts[i]->GetStateInst() >= 2) { continue; }
			XMFLOAT3 difPos = ghosts[i]->GetPosition();
			float dif = Helper::GetInstance()->ChechLength(difPos, boss->GetPosition());
			if (boss->GetLimit() > dif) {
				stopGhosts[nowStopGhorst] = ghosts[i];
				ghosts[i]->SetIsRefer(true);
				nowStopGhorst++;
				if (nowStopGhorst >= kStopGhorstMax) {
					break;
				}
			}
		}
		boss->SetLimit(boss->GetLimit() + 5.0f);
	}
	boss->SetLimit(20.0f);
	boss->SetSearch(false);
}

void LoadStageObj::LockAllGhost() {
	InterBoss* boss = m_EnemyManager->GetBoss();
	if (!boss->GetHyperSearch()) { return; }
	int  nowStopGhorst = 0;
	for (auto i = 0; i < ghosts.size(); i++) {
		if (ghosts[i]->GetIsRefer()) { continue; }
		//キャラステート変える際に気をつけてください
		if (ghosts[i]->GetStateInst() >= 2) { continue; }
		stopGhosts[nowStopGhorst] = ghosts[i];
		ghosts[i]->SetIsRefer(true);
		nowStopGhorst++;
	}
	boss->SetHyperSearch(false);
}

void LoadStageObj::ReferGhorstReseted() {
	for (Ghost*& ghost : stopGhosts) {
		if (!ghost) { continue; }
		if (ghost->GetStateInst() >= 4) { continue; }
		ghost->SetIsRefer(false);
	}
}

void LoadStageObj::NonVerseGhost() {
	InterBoss* boss = m_EnemyManager->GetBoss();
	if (boss->GetInstruction() != InterBoss::FourthBossInst::StopGhost) { return; }
	for (int i = 0; i < kStopGhorstMax; i++) {
		if (!stopGhosts[i]) { continue; }
		stopGhosts[i]->SetColor({ 1,0,1,1 });
		stopGhosts[i]->SetIsPostionCheck(true);
	}
	boss->SetInstruction(InterBoss::FourthBossInst::FinishMove);
}

bool LoadStageObj::CheckReferGhost() {
	InterBoss* boss = m_EnemyManager->GetBoss();
	int checkNum = 0;
	for (Ghost*& ghost : stopGhosts) {
		if (!ghost) { continue; }
		if (!ghost->GetIsRefer()) { continue; }
		checkNum++;
	}
	if (checkNum == 0) {
		boss->SetIsReferCheck(true);
		return false;
	} else {
		boss->SetIsReferCheck(false);
		return true;
	}
}

bool LoadStageObj::CheckReferDarkGhost() {
	InterBoss* boss = m_EnemyManager->GetBoss();
	int checkNum = 0;
	for (Ghost*& ghost : stopGhosts) {
		if (!ghost) { continue; }
		if (!ghost->GetIsRefer()) { continue; }
		checkNum++;
	}
	if (checkNum == 0) {
		boss->SetIsReferCheck(true);
		return false;
	} else {
		boss->SetIsReferCheck(false);
		return true;
	}
}

void LoadStageObj::ChangeGhost2Enemy() {
	InterBoss* boss = m_EnemyManager->GetBoss();
	if (boss->GetInstruction() != InterBoss::FourthBossInst::ChangeGhost) { return; }
	int m_GhostPos = 0;
	for (int i = 0; i < kStopGhorstMax; i++) {
		if (!stopGhosts[i]) { continue; }
		if (!stopGhosts[i]->GetAlive()) { continue; }
		if (!stopGhosts[i]->GetIsRefer()) { continue; }
		stopGhosts[i]->SetVanish(true);
		stopGhosts[i]->SetIsVerse(false, 80);
		boss->SetJackPos(i, stopGhosts[i]->GetPosition());
		m_GhostPos++;
	}
	ReferGhorstReseted();
	boss->SetInstruction(InterBoss::FourthBossInst::SpawnEnemy);
}

void LoadStageObj::ChangeGhost2Hyper() {
	InterBoss* boss = m_EnemyManager->GetBoss();
	if (boss->GetInstruction() != InterBoss::FourthBossInst::AllSummon) { return; }
	for (Ghost*& ghost : stopGhosts) {
		if (!ghost) { continue; }
		ghost->SetIsAllPostionCheck(true);
	}
	boss->SetInstruction(InterBoss::FourthBossInst::FinishMove);
}

//飢餓ゲージをゴースト三体分減らす
void LoadStageObj::SubHunger() {
	const float l_AddFrame = 0.1f;
	if (m_EnemyManager->GetEnemyCheck()) {
		m_LimitHunger = HungerGauge::GetInstance()->GetNowHunger() - 15.0f;
		m_SubHunger = true;
		m_EnemyManager->FinishCheck();
	}

	if (m_SubHunger) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		} else {
			m_Frame = {};
			m_LimitHunger = {};
			m_SubHunger = false;
		}
		HungerGauge::GetInstance()->SetNowHunger(Ease(In, Cubic, m_Frame, HungerGauge::GetInstance()->GetNowHunger(), m_LimitHunger));
	}
}
int LoadStageObj::GetGhostNumber() {
	int num = 0;
	for (auto i = 0; i < ghosts.size(); i++) {
		if (!ghosts[i]) { continue; }
		if (!ghosts[i]->GetAlive()) { continue; }
		if (ghosts[i]->GetIsRefer()) { continue; }
		if (ghosts[i]->GetStateInst() >= 2) { continue; }
		num++;
	}
	return num;
}
//ゴーストの吸収
void LoadStageObj::Absorption() {
	for (auto i = 0; i < ghosts.size(); ++i) {
		if (ghosts[i]->GetVanish()) { continue; }
		if (!ghosts[i]->GetAlive()) { continue; }
		if (ghosts[i]->GetCatch()) { continue; }
		if (ghosts[i]->GetFollow()) { continue; }

		if (m_EnemyManager->GetEnemyAbsorption()) {
			ghosts[i]->SetTargetPos(m_EnemyManager->GetEnemyPosition());
			ghosts[i]->SetAbsorption(true);
		} else {
			ghosts[i]->SetRotation({ ghosts[i]->GetRotation().x,-90.0f,ghosts[i]->GetRotation().z });
			ghosts[i]->SetAbsorption(false);
		}
	}
}
//ボスとの当たり判定
void LoadStageObj::CollideBoss() {
	float l_Radius = 6.0f;
	for (auto i = 0; i < ghosts.size(); ++i) {
		if (ghosts[i]->GetVanish()) { continue; }
		if (!ghosts[i]->GetAlive()) { continue; }
		if (ghosts[i]->GetCatch()) { continue; }
		if (ghosts[i]->GetFollow()) { continue; }
		if (!ghosts[i]->GetAbsorption()) { continue; }
		XMFLOAT3 l_ghostpos = ghosts[i]->GetPosition();
		float l_dir = Helper::GetInstance()->ChechLength(l_ghostpos, m_EnemyManager->GetEnemyPosition());
		if (l_dir < l_Radius) {
			ghosts[i]->SetVanish(true);
			m_EnemyManager->HealHP(m_HealPower);
		}
	}
}
//捕まえているゴーストを操る
void LoadStageObj::Manipulate() {
	const float l_AddFrame = 0.5f;
	for (auto i = 0; i < ghosts.size(); ++i) {
		if (ghosts[i]->GetVanish()) { continue; }
		if (!ghosts[i]->GetAlive()) { continue; }
		if (!ghosts[i]->GetCatch()) { continue; }
		if (!ghosts[i]->GetFollow()) { continue; }
		XMFLOAT3 l_ghostpos = ghosts[i]->GetPosition();
		if (m_EnemyManager->GetManipulate()) {
			m_Wide = true;
			m_SubHunger = true;
		}
		float l_dir = Helper::GetInstance()->ChechLength(l_ghostpos, m_EnemyManager->GetEnemyPosition());
		if (m_Wide) {
			m_WideArea += 100.0f;

			if (m_WideArea > l_dir) {
				ghosts[i]->SetManipulate(true);
			}

			if (m_WideArea > 100.0f) {
				m_WideArea = {};
				m_Wide = false;
			}
		}
	}

	if (m_SubHunger) {
		if (m_Frame < m_FrameMax) {
			m_Frame += l_AddFrame;
		} else {
			m_Frame = {};
			m_SubHunger = false;
		}
		HungerGauge::GetInstance()->SetCatchCount(0);
		HungerGauge::GetInstance()->SetNowHunger(Ease(In, Cubic, m_Frame, HungerGauge::GetInstance()->GetNowHunger(), 0.0f));
		HungerGauge::GetInstance()->SetHungerMax(Ease(In, Cubic, m_Frame, HungerGauge::GetInstance()->GetHungerMax(), 0.0f));
	}
}
void LoadStageObj::AwakeInit() {
	hearts.clear();
	for (auto i = 0; i < ghosts.size(); ++i) {
		ghosts[i]->SetVanish(true);
		ghosts[i]->DeleteBullet();
	}
}
//クリア
void LoadStageObj::AllClear() {
	ghosts.clear();
	foods.clear();
	hearts.clear();
}