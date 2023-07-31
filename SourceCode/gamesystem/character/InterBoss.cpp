#include "InterBoss.h"
#include"Collision.h"
#include "Helper.h"
#include <random>
#include "Player.h"
#include "HitStop.h"
//更新
void InterBoss::Update() {
	//陦悟虚
	Action();

	Helper::GetInstance()->CheckMax(m_HP, 0.0f, 0.0f);
	if (m_HP == 0.0f) {
		DeathAction();
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

//登場の更新
void InterBoss::AppearUpdate() {
	AppearAction();//ボス固有の更新
}
//撃破の更新
void InterBoss::DeadUpdate() {
	if (ThrowUpdateF)
		DeadAction_Throw();
	else
		DeadAction();//ボス固有の更新
}
//描画
void InterBoss::Draw(DirectXCommon* dxCommon) {
}
//ImGui謠冗判
void InterBoss::ImGuiDraw() {
	if (!this) { return; }
	ImGui_Origin();
}

float InterBoss::HpPercent() {
	float temp = m_HP / m_MaxHp;
	Helper::GetInstance()->Clamp(temp, 0.0f, 1.0f);
	return temp;
}

void InterBoss::AwakeUpdate() {
	InitAwake();
	if (m_AwakeInit) {
		//m_Rotation.y += 5.0f;
	}
	//OBJのステータスのセット
	Obj_SetParam();
}

//弾との当たり判定
void InterBoss::CollideBul(vector<InterBullet*> bullet,Type type)
{
	int l_RandCount = 0;

	if (ColChangeEaseT>0.f)return;

	for (InterBullet* _bullet : bullet) {
		if (_bullet != nullptr && _bullet->GetAlive() && !_bullet->GetBossCatch()) {
			bool JudgColide;
			if (type == Type::CIRCLE)JudgColide=Collision::CircleCollision(_bullet->GetPosition().x, _bullet->GetPosition().z, _bullet->GetRadius(), m_Position.x, m_Position.z, m_Radius);
			if (type == Type::SPHERE)JudgColide=Collision::SphereCollision(_bullet->GetPosition(), m_Radius, m_Position, m_Radius);
			if (JudgColide)
			{
				SearchF = true;
				//乱数指定
				mt19937 mt{ std::random_device{}() };
				uniform_int_distribution<int> l_RandomRange(1, 100);
				l_RandCount = int(l_RandomRange(mt));
				//乱数が一定の値より大きかったらハートが出る
				if (l_RandCount > m_BirthTarget) {
					m_BirthHeart = true;
				}

				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Attack_Normal.wav", VolumManager::GetInstance()->GetSEVolum());
				ActionTimer++;
				Recv = true;
				_bullet->SetAlive(false);
				//弾の大きさによって与えるダメージが違う
				if (_bullet->GetPowerState() == POWER_NONE) {
					m_HP -= _bullet->GetPower() * m_Magnification;
				}
				else {
					m_HP -=  _bullet->GetPower();
				}
				if (m_HP <1.f) {
					if (SceneName == "FIRSTSTAGE")
					{
						m_Position = { 0,40,20 };
					}
				}
				BirthEffect();
				if (m_HP != 0.0f && _bullet->GetPowerState() == POWER_UNLIMITED) {
					HitStop::GetInstance()->SetHitStop(true);
				}
			}
		}
	}
}
//プレイヤーとボスの当たり判定
void InterBoss::ColPlayer()
{
	//ラッシュ中判定あり
	if (Collision::CircleCollision(m_Position.x, m_Position.z, m_Radius, Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().z, 1.f) &&
		(Player::GetInstance()->GetDamageInterVal() == 0))
	{
		Player::GetInstance()->RecvDamage(1.0f);
		Player::GetInstance()->PlayerHit(m_Position);
	}
}
//エフェクトの発生
void InterBoss::BirthEffect() {
	InterEffect* neweffect;
	neweffect = new BreakEffect();
	neweffect->Initialize();
	neweffect->SetPosition(m_Position);
	//if (SceneName == "FIRSTSTAGE")
	//	neweffect->SetPosition(EffectFirstPos);
	if (m_HP <= 0.0f && m_Slow) {
		neweffect->SetLife(1000);
		neweffect->SetDiviSpeed(8.0f);
	} else {
		neweffect->SetDiviSpeed(1.0f);
		neweffect->SetLife(50);
	}

	effects.push_back(neweffect);
}


void InterBoss::isRespawn(std::vector<InterEnemy*> boss)
{
	if(ResF)
	{
		for(auto i=0;i<3;i++)
		{
			NextActionInteval = 0;
			boss[i]->SetHP(1);
			boss[i]->SetScale({ 0,0,0});
			boss[i]->SetColor({ 1,1,1,1 });
			boss[i]->SetShotF(false);
			boss[i]->SetcanRotandRush(false);
			boss[i]->SetAlive(true);
			
		}
		ResF = false;
	}
}

void InterBoss::SpawnSetEnemy(vector<unique_ptr<InterEnemy>> enemys) {

}


void InterBoss::SummonEnemyUpda(std::vector<InterEnemy*> boss)
{
	if (m_HP < 0.1)return;
	std::vector<XMVECTOR> move(boss.size());
	std::vector<XMMATRIX>matRot(boss.size());

	for (auto i = 0; i < boss.size(); i++) {
		move[i] = { 0.f,0.f, 0.1f, 0.0f };
	}
	if (!SummonF) {
		for (auto i = 0; i < boss.size(); i++) {
			if (boss[i] == nullptr)continue;
			move[i] = { 0.f,0.f, 0.1f, 0.0f };
			matRot[i] = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y + (0.f + static_cast<float>(i) * 45.f)));
			move[i] = XMVector3TransformNormal(move[i], matRot[i]);

			boss[i]->SetPosition({ m_Position.x + move[i].m128_f32[0] * 90.f,0.f,	m_Position.z + move[i].m128_f32[2] * 90.f });
		}
		
	}

	if (SummonF) {
		IdleRota = false;
		for (auto i = 0; i < boss.size(); i++)
		{
			if (boss[i] == nullptr)continue;
			boss[i]->Update();
		}
		for (auto i = 0; i < boss.size(); i++) {
			if (boss[i] == nullptr)continue;
			boss[i]->SetShotF(true);
		}
		
	}
	EndSummon(boss);
}

void InterBoss::EndSummon(std::vector<InterEnemy*> boss)
{
	bool tempList[3];
	for (auto i = 0; i < _countof(tempList); i++) {
		if (boss[i] == nullptr)continue;
		tempList[i] = boss[i]->GetShotF();
	}
	if (SummobnStop) {
		if (Helper::GetInstance()->All_Of(tempList, _countof(tempList))) {
			
			NextActionInteval++;
			if (NextActionInteval > SummonCool) {
				ActionTimer++;
				EndSummonRepos = true;
				SummobnStop = false;
			}
		}
	}
	else
	{
		NextActionInteval = 0;
	}
	bool tempList2[3];
	for (auto i = 0; i < _countof(tempList2); i++) {
		if (boss[i] == nullptr)continue;
		tempList2[i] = !boss[i]->GetisAlive() && boss[i]->GEtAlpha() <= 0.f;
	}
	if(Helper::GetInstance()->All_Of(tempList2,_countof(tempList2)))
	{
		SummonF = false;
	}
}

void InterBoss::SummonEnemyDraw(std::vector<InterEnemy*> boss, DirectXCommon* dxcomn)
{

	if (m_HP < 0.1)return;
	for (auto i = 0; i < boss.size(); i++)
	{
		if (boss[i] == nullptr)continue;
		boss[i]->Draw(dxcomn);
	}
}

void InterBoss::DeathAction()
{
	if (isAlive)return;

	DeathSceneF = true;
}

void InterBoss::EndRollUpdate() {
	EndRollAction();
}

void InterBoss::AnimationControl(AnimeNames name, const bool& loop, int speed)
{
	//アニメーションを引数に合わせる
	if (_animeName != name)
	{
		fbxmodel->PlayAnimation(static_cast<int>(name));
	}

	//各種パラメータ反映
	_animeName = name;
	m_LoopFlag = loop;
	m_AnimationSpeed = speed;
}

void InterBoss::AfterUpdate() {
	AfterAction();
}