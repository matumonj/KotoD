#include "FiveBoss.h"
#include "ImageManager.h"
#include "Helper.h"
#include "Player.h"
#include "VariableCommon.h"
#include "CsvLoader.h"
#include "Easing.h"
#include <random>

#include "Input.h"

FiveBoss::FiveBoss()
{

	//ダーク琴子モデル
		//モデル初期化と読み込み
	fbxmodel.reset(new IKEFBXObject3d());
	fbxmodel->Initialize();
	fbxmodel->SetModel(ModelManager::GetInstance()->GetFBXModel(ModelManager::PLAYER2));
	fbxmodel->LoadAnimation();
	fbxmodel->PlayAnimation(0);

	confueffect.reset(new ConfuEffect());
	confueffect->Initialize();

	noteeffect.reset(new NoteEffect());
	noteeffect->Initialize();
	//m_HP = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "hp")));
}

bool FiveBoss::Initialize()
{
	m_Position = { 0.0f,3.0f,30.0f };
	m_Rotation = { 0.0f,90.0f,0.0f };
	m_Radius = 5.2f;
	m_Scale = { 1.9f,1.5f,1.9f };
	m_Color = { 0.10f,0.0f,0.10f,0.80f };
	//m_Rotation.y = -90.f;
	m_Magnification = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "Magnification")));
	m_HP = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "hp")));
	m_BirthTarget = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "HeartTarget")));

	//ノックバック頻度
	KnockInter = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "KnockInter")));

	//KnockBack
	KnockDam = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "KnockDam")));


	//MeteoShot
	MeteoDam = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "MeteoDam")));

	//WayShot
	ShotDam = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "ShotDam")));

	//Ultimate
	UltDam = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "UltDam")));

	//darkshot
	DarkShotDam = 0.5f;// static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "DarkSutoponShotDam")));

	noAction = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "noAction")));

	shot = new ShotAttack();
	normal = new NormalAttack();
	smash = new SmashShotAttack();
	slash = new ShadowSlashAttack();
	single = new SingleShot();
	guard = new GuardAction();
	knock = new KnockAttack();
	darkshot = new DarkSutoponShot();
	skirtobj.reset(new IKEObject3d());
	skirtobj->Initialize();
	skirtobj->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::SKIRT));

	smash->Init();
	shot->Init();
	slash->Init();
	single->Init();
	guard->Init();
	normal->Init();
	knock->Init();
	darkshot->Init();

	normal->SetBoss(this);

	knock->SetDam(KnockDam);
	smash->SetDam(MeteoDam);
	slash->SetDam(UltDam);
	single->SetDam(ShotDam);
	darkshot->SetDam(DarkShotDam);
	m_MaxHp = m_HP;
	GhostSize = 0;
	_aPhase = ATTACK_SHOT;
	/*ActionTimer = 1;


	_charaState = STATE_INTER;
	m_AreaState = AREA_SET;*/
	//CSVロード
	bonesize = fbxmodel->GetBoneSize();
	//{
	bonepos.resize(19);
	bonemat.resize(19);
	s_color.resize(19);
	e_color.resize(19);
	s_scale.resize(19);
	e_scale.resize(19);
	m_Life.resize(19);
	effects.clear();
	//m_HP = 10;
	CSVLoad();
	return true;
}

void FiveBoss::SkipInitialize()
{

	m_Position = { 0.0f,-5.0f,30.0f };
	m_Rotation = { 0.0f,90.0f,0.0f };
	m_Color = { 0.10f,0.0f,0.10f,0.80f };
}

void FiveBoss::Pause()
{
}
void (FiveBoss::* FiveBoss::attackTable[])() = {
	&FiveBoss::Pause,
	&FiveBoss::Shot,
	&FiveBoss::Normal,
	&FiveBoss::Smash,
	&FiveBoss::Single
};

void FiveBoss::ActionSet(ActionPhase phase, InterAttack* attack)
{
	if (_aPhase == phase)
	{
		RandAction = 0;
		if (attack->GetActionEnd())
		{
			ActionTimer++;
			shot->SetCanRand(0);

			shot->SetActionEnd(false);
			_aPhase = ATTACK_SHOT;
		}
	}
}

void FiveBoss::Action()
{
	smash->SetBoss(this);
	shot->SetBoss(this);

	shot->SetSutoPos(knock->Sutoobj()->GetPosition());
	shot->SetSutoRot(knock->Sutoobj()->GetRotation());

	slash->SetBoss(this);
	single->SetBoss(this);
	guard->SetBoss(this);
	knock->SetBoss(this);
	m_Color.w = 0.80f;
	////状態移行(charastateに合わせる)
	if (GhostSize < 6) {
		if (m_HP > 0.0f) {
			(this->*attackTable[_aPhase])();
		}
	}
	//	skirtobj->SetRotation({ 0,0,90 });
	skirtobj->SetScale({ 3,3,3 });

	index = 13;
	fbxmodel->GetBoneIndexMat(index, skirtmat);
	skirtobj->SetRotation({ 0,0,90 });
	skirtobj->FollowUpdate(skirtmat);
	skirtobj->SetColor(m_Color);

	//knockの攻撃判定
	KnockTimer++;
	if (!darkshot->GetActionStart()) {
		if (KnockTimer % KnockInter == 0)
			knock->setKnockF(true);
	}
	//盾貼るタイミング
	if (GuardCount == 0 && m_HP < m_MaxHp / 2) {
		guard->SetGuardStart(true);
		GuardCount++;
		//guard->SetGuardStart(true);
	}
	if (guard->GetGuardStart())
		m_Magnification = 0.f;
	else
		m_Magnification = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/first/Firstboss.csv", "Magnification")));

	darkshot->SetSutoPos(knock->Sutoobj()->GetPosition());
	darkshot->SetSutoRot(knock->Sutoobj()->GetRotation());
	darkshot->Upda();


	if (m_HP > 0.0f) {
		knock->Upda();
		guard->Upda();
	}

	/// <summary>
	/// 攻撃ー３WAY
	/// </summary>

	////ActionSet(ATTACK_SHOT, shot);
	ActionSet(ATTACK_IMPACT, smash);
	//ActionSet(ATTACK_SLASH, slash);
	ActionSet(ATTACK_SINGLESHOT, single);

	if (_aPhase == ATTACK_SHOT) { ActionTimer++; }

	{
		for (auto i = 0; i < ghosts.size(); i++) {
			if (ghosts[i]->GetState() != Ghost::STATE_DARKOTI)continue;

			ghosts[i]->SetFivePos(m_Position);
		}

		for (auto i = 0; i < ghosts.size(); ++i) {
			for (auto j = 0; j < ghosts.size(); ++j) {
				XMFLOAT3 ghostpos = ghosts[i]->GetPosition();
				XMFLOAT3 ghostpos2 = ghosts[j]->GetPosition();
				if ((i == j)) { continue; }
				if ((!ghosts[i]->GetAlive()) || (!ghosts[j]->GetAlive())) { continue; }
				if ((!ghosts[i]->GetCollide()) || (!ghosts[j]->GetCollide())) { continue; }
				if (Collision::SphereCollision(ghostpos, 1.5f, ghostpos2, 1.5f)) {
					ghosts[i]->GhostCollision(ghostpos2);
					ghosts[j]->GhostCollision(ghostpos);
				}
			}
		}
	}

	for (auto i = 0; i < ghosts.size(); i++)
	{
		if (ghosts[i]->GetStateSpawn())
		{
			GhostSize = 0;
			ghosts[i]->SetCollide(false);
			ghosts[i]->SetCleanGhost(false);
			ghosts[i]->SetStateSpawn(false);
		}
	}


	mt19937 mt{ std::random_device{}() };
	if (GhostSize == 0)
	{
		if (_aPhase == ATTACK_SINGLESHOT)
		{
			single->SetActionEnd(true);
			shot->SetCanRand(0);
			shot->SetActionEnd(false);
			_aPhase = ATTACK_SHOT;
		}
	}
	//single->Upda();
	if (shot->GetPhase() == ShotAttack::Phase::END) {
		//通常攻撃
		if (shot->GetCanRand() > noAction && GhostSize > 0 && GhostSize < 4)
		{
			shot->SetActionEnd(true);
			shot->SetIdleDam(false);
			single->SetActionEnd(false);
			AnimationControl(InterBoss::AnimeNames::SHOT, false, 1);
			_aPhase = ATTACK_SINGLESHOT;
		}
		if (shot->GetCanRand() > noAction && GhostSize == 4)
		{
			shot->SetActionEnd(true);
			shot->SetIdleDam(false);
			smash->SetActionEnd(false);
			_aPhase = ATTACK_IMPACT;
		}
		if (GhostSize >= 5)
		{
			shot->SetActionEnd(true);
			shot->SetIdleDam(false);
			single->SetActionEnd(false);
			JudgSlash = true;
			_aPhase = ATTACK_SINGLESHOT;
		}
	}

	if (JudgSlash)
	{
		slash->SetActionEnd(false);
		slash->Upda();
		if (slash->GetActionEnd())
			JudgSlash = false;
	}


	if (!JudgDShot && GhostSize > 4)
	{
		uniform_int_distribution<int> l_Rand(0, 10);
		ThreeGhostActionRand = l_Rand(mt);

		if (ThreeGhostActionRand < 20) {
			JudgDShot = true;
			darkshot->SetActionStart(true);
		}
	}
	if (JudgDShot)
	{
		CoolDShot++;
		if (CoolDShot > 300)
			JudgDShot = false;
	} else
	{
		CoolDShot = 0;
	}

	Helper::GetInstance()->Clamp(GhostSize, 0, 5);

	/*^^^^当たり判定^^^^*/
	//弾とボスの当たり判定
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
	if (!guard->GetGuardStart())
		CollideBul(_playerBulA, Type::CIRCLE);
	//プレイヤーの当たり判定
	ColPlayer();
	//OBJのステータスのセット
	//Obj_SetParam();
	//リミット制限
	Helper::GetInstance()->Clamp(m_Position.x, -55.0f, 65.0f);
	Helper::GetInstance()->Clamp(m_Position.z, -60.0f, 60.0f);


	//HPが半分切ったら強化
	if (m_HP < m_MaxHp / 2) {
		isStrong = true;
	}
	//基礎パラメータ設定

	//どっち使えばいいか分からなかったから保留
	fbxmodel->SetPosition(m_Position);
	fbxmodel->SetRotation(m_Rotation);
	fbxmodel->SetScale(m_Scale);
	fbxmodel->SetColor(m_Color);
	fbxmodel->Update(m_LoopFlag, m_AnimationSpeed, m_StopFlag);

	m_ParticleTimer++;
	if (m_ParticleTimer == 30) {
		for (auto i = 0; i < 19; i++)
		{
			fbxmodel->GetBoneIndexMat(i, bonemat[i]);
			MatTranstoPos(bonemat[i], bonepos[i]);

			s_color[i] = { 0.30f,0.0f,0.30f,0.50f };
			e_color[i] = { 0.f,0.f,0.f,0.0f };
			s_scale[i] = 3.0f;
			e_scale[i] = 0.0f;
			m_Life[i] = 40;
			ParticleEmitter::GetInstance()->DarkOtiEffect(m_Life[i], bonepos[i], s_scale[i], e_scale[i], s_color[i], e_color[i]);

		}
		m_ParticleTimer = {};
	}

}

void FiveBoss::AppearAction()
{
	const float l_AddFrame = 0.05f;
	m_AppearTimer++;
	if (_AppState == APP_SET) {
		m_Position = { 600.0f,-3.0f,30.0f };
		m_Rotation = { 0.0f,180.0f,0.0f };
		m_Color = { 0.10f,0.0f,0.10f,0.80f };

		if (m_AppearTimer == 270) {
			_AppState = APP_BIRTH;
		}
	} else if (_AppState == APP_BIRTH) {
		AppParticle();

		if (m_AppearTimer >= 350) {
			m_Position.x = 0.0f;
			if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
				m_Frame = {};
				_AppState = APP_END;
			}
		}

		m_Color.w = Ease(In, Cubic, m_Frame, m_Color.w, 1.0f);
	} else {

	}

	knock->AppearUpdate(m_AppearTimer);
	//どっち使えばいいか分からなかったから保留
	fbxmodel->SetPosition(m_Position);
	fbxmodel->SetRotation(m_Rotation);
	fbxmodel->SetScale(m_Scale);
	fbxmodel->SetColor(m_Color);
	fbxmodel->Update(m_LoopFlag, m_AnimationSpeed, m_StopFlag);
}

void FiveBoss::DeadAction()
{
	m_DeathTimer++;
	if (_DeathState == DEATH_SET) {
		if (m_DeathTimer == 1) {
			fbxmodel->PlayAnimation(0);
			m_Position = { -4.0f,-5.0f,10.0f };
			m_Rotation = { 0.0f,180.0f,0.0f };
			_DeathState = DEATH_KNOCK;
		}
	} else if (_DeathState == DEATH_KNOCK) {
		m_Position.y = Ease(Out, Quad, m_DeathTimer / static_cast<float>(150), -5.0f, 0.0f);
		m_Rotation.x = Ease(Out, Quad, m_DeathTimer / static_cast<float>(150), 0.0f, -90.0f);

		if (m_Rotation.x <= -80.0f) {
			_DeathState = DEATH_STOP;
		}
	} else {
		DeathParticle();
	}

	knock->DeathUpdate(m_DeathTimer);
	//どっち使えばいいか分からなかったから保留
	fbxmodel->SetPosition(m_Position);
	fbxmodel->SetRotation(m_Rotation);
	fbxmodel->SetScale(m_Scale);
	fbxmodel->SetColor(m_Color);
	fbxmodel->Update(m_LoopFlag, m_AnimationSpeed, m_StopFlag);
}

void FiveBoss::DeadAction_Throw()
{
	fbxmodel->StopAnimation();
	fbxmodel->SetPosition(m_Position);
	fbxmodel->SetRotation(m_Rotation);
	fbxmodel->SetScale(m_Scale);
	fbxmodel->SetColor(m_Color);
	fbxmodel->Update(m_LoopFlag, m_AnimationSpeed, m_StopFlag);
}

void FiveBoss::CSVLoad()
{
}

void FiveBoss::DeathParticle()
{
	float l_AddSize = 2.5f;
	const float RandScale = 3.0f;
	float s_scale = 0.3f * l_AddSize;
	float e_scale = (4.0f + (float)rand() / RAND_MAX * RandScale - RandScale / 2.0f) * l_AddSize;

	//色
	const float RandRed = 0.2f;
	const float red = 0.2f + (float)rand() / RAND_MAX * RandRed;
	const XMFLOAT4 s_color = { 0.9f, red, 0.1f, 1.0f }; //濃い赤
	const XMFLOAT4 e_color = { 0, 0, 0, 1.0f }; //無色
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_Randlife(10, 40);
	int l_Life = int(l_Randlife(mt));
	ParticleEmitter::GetInstance()->ExproEffectBoss(l_Life, m_Position, l_AddSize, s_scale, e_scale, s_color, e_color);
}

void FiveBoss::AppParticle() {
	XMFLOAT4 s_color = { 1.0f,0.0f,1.0f,1.0f };
	XMFLOAT4 e_color = { 1.0f,1.0f,1.0f,1.0f };
	float s_scale = 5.0f;
	float e_scale = 0.0f;
	XMFLOAT3 l_birthPos = {};
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandX(-6, 6);
	l_birthPos.x = float(l_RandX(mt));
	l_birthPos.z = m_Position.z - 5.0f;
	l_birthPos.y = -1.0f;

	for (int i = 0; i < 5; i++) {
		ParticleEmitter::GetInstance()->DarkEffect(200, l_birthPos, s_scale, e_scale, s_color, e_color);
	}
}
void FiveBoss::ImGui_Origin()
{
	ImGui::Begin("bone");
	ImGui::SliderFloat("rot", &rotys, 0, 360);
	ImGui::SliderInt("index", &index, 0, 24); ImGui::End();
}

void FiveBoss::EffecttexDraw(DirectXCommon* dxCommon)
{
	if (m_HP <= 0.0f)return;

	confueffect->Draw(dxCommon);
	noteeffect->Draw(dxCommon);
}

void FiveBoss::Draw(DirectXCommon* dxCommon)
{
	//Obj_Draw();
	if (m_HP > 0.0f) {
		smash->Draw(dxCommon);
		if (_aPhase == ATTACK_SINGLESHOT)
			single->Draw(dxCommon);

		shot->Draw(dxCommon);

		slash->Draw(dxCommon);
		darkshot->Draw(dxCommon);
	}
	knock->Draw(dxCommon);
	fbxmodel->Draw(dxCommon->GetCmdList());
	guard->Draw(dxCommon);
	IKEObject3d::PreDraw();
	skirtobj->Draw();
	IKEObject3d::PostDraw();
}

void FiveBoss::MatTranstoPos(XMMATRIX trans, XMFLOAT3& m_Pos)
{
	m_Pos.x = trans.r[3].m128_f32[0]; // GetPosition().x;
	m_Pos.y = trans.r[3].m128_f32[1];
	m_Pos.z = trans.r[3].m128_f32[2];
}
void FiveBoss::InitAwake() {

}
void FiveBoss::EndRollAction()
{
	const float l_AddFrame = 0.01f;
	m_EndTimer++;
	if (_EndState2 == END_SET2) {
		m_Position = { 10.0f,5.0f,-25.0f };
		m_Rotation = { 0.0f,180.0f,270.0f };
		m_Scale = { 0.8f,0.4f,0.8f };
		fbxmodel->PlayAnimation(0);
		if (m_EndTimer == 980) {
			_EndState2 = END_RIGHT;
			m_AfterPos = { 7.0f,5.0f,-25.0f };
			m_View = true;
			_ViewType = VIEW_MOVE;
		}
	} else if (_EndState2 == END_RIGHT) {
		if (m_View) {
			if (_ViewType == VIEW_MOVE) {
				if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
					m_Frame = {};
					m_AfterPos = { 10.0f,5.0f,-25.0f };
					_ViewType = VIEW_RETURN;
				}
			} else {
				if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
					m_Frame = {};
					_ViewType = VIEW_MOVE;
					m_Position = { -10.0f,5.0f,-25.0f };
					m_AfterPos = { -7.0f,5.0f,-25.0f };
					_EndState2 = END_LEFT;
				}
			}
			SetEasePos();
		}
	} else if (_EndState2 == END_LEFT) {
		m_Rotation = { 0.0f,180.0f,90.0f };
		if (m_View) {
			if (_ViewType == VIEW_MOVE) {
				if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
					m_Frame = {};
					m_AfterPos = { -10.0f,5.0f,-25.0f };
					_ViewType = VIEW_RETURN;
				}
			} else {
				if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
					m_Frame = {};
					_ViewType = VIEW_MOVE;
					m_Position = { -3.0f,15.0f,-25.0f };
					m_AfterPos = { -3.0f,10.0f,-25.0f };
					_EndState2 = END_TOP;
				}
			}
			SetEasePos();
		}
	} else if (_EndState2 == END_TOP) {
		m_Rotation = { 0.0f,180.0f,180.0f };
		if (m_View) {
			if (_ViewType == VIEW_MOVE) {
				if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
					m_Frame = {};
					m_AfterPos = { -3.0f,15.0f,-25.0f };
					_ViewType = VIEW_RETURN;
				}
			} else {
				if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
				}
			}
			SetEasePos();
		}

		if (m_EndTimer == 1670) {
			_EndState2 = END_MOVE2;
			fbxmodel->StopAnimation();
		}
	} else {
		m_Rotation = { 0.0f,180.0f,0.0f };
		m_Position = { -3.0f,0.0f,-25.0f };
	}
	fbxmodel->SetPosition(m_Position);
	fbxmodel->SetRotation(m_Rotation);
	fbxmodel->SetScale(m_Scale);
	fbxmodel->SetColor(m_Color);
	fbxmodel->Update(m_LoopFlag, m_AnimationSpeed, m_StopFlag);
}

void FiveBoss::SetEasePos() {
	m_Position = {
Ease(In,Cubic,m_Frame,m_Position.x,m_AfterPos.x),
Ease(In,Cubic,m_Frame,m_Position.y,m_AfterPos.y),
	Ease(In,Cubic,m_Frame,m_Position.z,m_AfterPos.z)
	};
}
void FiveBoss::BirthParticle() {
	const int l_ParticleTimer = 30;
	XMFLOAT4 s_color = { 1.0f,0.0f,1.0f,1.0f };
	XMFLOAT4 e_color = { 0.0f,0.0f,0.0f,1.0f };
	float s_scale = 50.0f;
	float e_scale = 0.0f;

	//m_ParticleTimer++;

	//if (m_ParticleTimer == l_ParticleTimer) {
	//	ParticleEmitter::GetInstance()->KotokoEffect(50, m_Position, s_scale, e_scale, s_color, e_color);
	//	m_ParticleTimer = {};
	//}
}
//会話
void FiveBoss::AfterAction() {

}