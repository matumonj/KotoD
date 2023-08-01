#include "FirstBoss.h"
#include "ImageManager.h"
#include"Easing.h"
#include "Helper.h"
#include "Player.h"
#include "VariableCommon.h"
#include "CsvLoader.h"
#include <random>

void (FirstBoss::* FirstBoss::stateTable[])() = {
	&FirstBoss::InterValMove,
	&FirstBoss::Choice,
	&FirstBoss::RockOnAttack,
	&FirstBoss::DropAttack,
	&FirstBoss::Hit,
	&FirstBoss::Big,
};

FirstBoss::FirstBoss()
{
	m_Model = ModelManager::GetInstance()->GetModel(ModelManager::MILKCAP_NORMAL);

	m_Object.reset(new IKEObject3d());
	m_Object->Initialize();
	m_Object->SetModel(m_Model);

	confueffect.reset(new ConfuEffect());
	confueffect->Initialize();

	noteeffect.reset(new NoteEffect());
	noteeffect->Initialize();

	tex.reset(IKETexture::Create(ImageManager::DAMAGEAREA, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	tex->TextureCreate();
	tex->SetPosition({ 0.0f,-500.0f,0.0f });
	tex->SetTiling(2.0f);

	m_HP = 0.f;
}

bool FirstBoss::Initialize()
{
	m_Position = { 0.0f,3.0f,50.0f };
	m_Rotation = { 0.0f,0.0f,0.0f };
	m_Scale = { 18.3f,18.3f,18.3f };
	m_Color = { 1.0f,1.0f,1.0f,0.1f };
	m_Rotation.y = 90.f;
	m_Position.x = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/first/Firstboss.csv", "pos")));
	m_Magnification = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/first/Firstboss.csv", "Magnification")));
	m_HP =  static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/first/Firstboss.csv", "hp1")));
	m_BirthTarget = static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/first/Firstboss.csv", "HeartTarget")));
	DecisionCount=static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/first/Firstboss.csv", "DecisionCount")));
	m_Radius = 5.2f;
	m_MaxHp = m_HP;
	half_hp_ = 10;
	_charstate = CharaState::STATE_INTER;

	m_TexColor = { 1.0f,1.0f,1.0f,0.0f };
	m_TexRot = { 90.0f,0.0f,0.0f };
	m_TexScale = { 0.2f,3.0f,0.6f };

	effects.clear();
	return true;
}

void FirstBoss::SkipInitialize()
{
	m_Position = { 0.0f,3.0f,30.0f };
	m_Rotation = { 0.0f,0.0f,0.0f };
	m_Scale = { 15.3f,15.3f,15.3f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
}

void FirstBoss::Pause()
{
}


void FirstBoss::Action()
{
	if (statereset_) {
		_charstate = CharaState::STATE_INTER;
		statereset_ = false;
	}


	if (m_HP < half_hp_) {
		returnflag = true;
		half_hp_ = -100;
		m_Magnification = 0.3f;
		nowrot_ = m_Rotation.x;
		_charstate = CharaState::STATE_BIG;
	}

	if (bounce_ == Bounce::SOURCE) {
		XMFLOAT3 s_scl = m_Scale;
		bounceTimer += 1.0f / 60;
		Helper::GetInstance()->Clamp(bounceTimer, 0.0f, 1.0f);
		m_Scale = {
		Ease(Out, Quart, bounceTimer, s_scl.x, e_scl.x),
		Ease(Out, Quart, bounceTimer, s_scl.y, e_scl.y),
		Ease(Out, Quart, bounceTimer, s_scl.z, e_scl.z),
		};
	}

	if (bottlestate_ == BottleState::NORMAL) {
		m_Model = ModelManager::GetInstance()->GetModel(ModelManager::MILKCAP_NORMAL);
		m_Object->SetModel(m_Model);
	}
	else {
		m_Model = ModelManager::GetInstance()->GetModel(ModelManager::MILKCAP_BREAK);
		m_Object->SetModel(m_Model);
	}

	//状態移行(charastateに合わせる)
	if (m_HP > 0.0f) {
		(this->*stateTable[_charstate])();
		FaceToOrientation();
	}
	
	/*^^^^当たり判定^^^^*/
	//弾とボスの当たり判定
	vector<InterBullet*> _playerBulA = Player::GetInstance()->GetBulllet_attack();
	CollideBul(_playerBulA, Type::CIRCLE);
	
	//プレイヤーの当たり判定
	ColPlayer();
	//OBJのステータスのセット
	Obj_SetParam();
	for (std::unique_ptr<Fraction>& fraction : fraction_) {
		fraction->Update(_playerBulA);
	}
	fraction_.remove_if([](std::unique_ptr<Fraction>& fraction) {
		return fraction->IsDelete();
		});
	//リミット制限
	Helper::GetInstance()->Clamp(m_Position.x, -55.0f, 65.0f);
	Helper::GetInstance()->Clamp(m_Position.z, -60.0f, 60.0f);
}

void FirstBoss::AppearAction()
{
	if (movie == false) {
		if (m_Position.z >= 41) {
			m_Position.z -= 0.1f;
			m_Color.w += 0.01f;		
		}
		else {
			movie = true;
		}
	}

	Obj_SetParam();
}

void FirstBoss::DeadAction()
{
	m_Scale = { 15.3f,15.3f,15.3f };
	const float l_AddAngle = 5.0f;
	m_DeathTimer++;
	const int l_BaseTarget = 50;
	if (m_DeathTimer == 1) {
		m_Position = { 0.0f,0.0f,20.0f };
		m_Rotation = { 20.0f,0.0f,0.0f };
	}
	else if (m_DeathTimer >= 2 && m_DeathTimer < 120) {
		m_Rotation.y += 10.f;
		DeathParticle();
	}
	else {
		if (m_Rotation.x <= 90) {
			m_Rotation.x++;
		}
	}

	Obj_SetParam();
}

void FirstBoss::DeadAction_Throw()
{
	Obj_SetParam();
}

void FirstBoss::DeathParticle()
{
	const XMFLOAT4 s_color = { 1.0f,1.0f,1.0f,1.0f };
	const XMFLOAT4 e_color = { 0.0f,0.0f,1.0f,1.0f };
	float s_scale = 5.0f;
	float e_scale = 0.0f;
	float l_velocity = 0.5f;
	for (int i = 0; i < 3; ++i) {
		ParticleEmitter::GetInstance()->DeathEffect(50, { m_Position.x,(m_Position.y - 1.0f),m_Position.z }, s_scale, e_scale, s_color, e_color, l_velocity);
	}
	
}

void FirstBoss::ImGui_Origin()
{
}

void FirstBoss::EffecttexDraw(DirectXCommon* dxCommon)
{
	if (m_HP <= 0.0f)return;

	confueffect->Draw(dxCommon);
	noteeffect->Draw(dxCommon);
}

void FirstBoss::Draw(DirectXCommon* dxCommon)
{
	IKETexture::PreDraw2(dxCommon, AlphaBlendType);
	if (Display == true&&m_HP>0) {
		tex->Draw();
	}
	IKETexture::PostDraw();
	Obj_Draw();
	if (m_HP > 0) {
		for (std::unique_ptr<Fraction>& fraction : fraction_) {
			fraction->Draw(dxCommon);
		}
	}

}
//攻撃後のインターバル
void FirstBoss::InterValMove()
{
	if (m_Rotation.x != 0) {
		XMFLOAT3 s_rot = m_Rotation; 
		returntimer_ += 1.f / 60.f;
		XMFLOAT3 e_rot{ 0.f,90.f,0.f };
		Helper::GetInstance()->Clamp(returntimer_, 0.0f, 1.0f);
		m_Rotation = {
		Ease(Out, Quart, bounceTimer, s_rot.x, e_rot.x),
		Ease(Out, Quart, bounceTimer, s_rot.y, e_rot.y),
		Ease(Out, Quart, bounceTimer, s_rot.z, e_rot.z),
		};
		if (returntimer_ == 1) {
			returntimer_ = 0;
		}
	}

	if (m_Scale.x != e_scl.x || m_Scale.y != e_scl.y || m_Scale.z != e_scl.z) {
		XMFLOAT3 s_scl = m_Scale;
		returntimer_ += 1.f / 60.f;

		Helper::GetInstance()->Clamp(returntimer_, 0.0f, 1.0f);
		m_Scale = {
		Ease(Out, Quart, bounceTimer, s_scl.x, e_scl.x),
		Ease(Out, Quart, bounceTimer, s_scl.y, e_scl.y),
		Ease(Out, Quart, bounceTimer, s_scl.z, e_scl.z),
		};
		if (returntimer_ == 1) {
			returntimer_ = 0;
		}
	}

	Display = false;
	jumpCount = 0;
	commandTimer += 1.0f / 50;
	Helper::GetInstance()->Clamp(commandTimer, 0.0f, 1.0f);
	s_posY = m_Position.y;
	if (act_ == ActionFase::Before) {
		e_posY = 5.f;
		if (commandTimer >= 1) {
			commandTimer = 0.f;
			act_ = ActionFase::After;
		}
	}
	else {
		e_posY = 0.f;
		if (commandTimer >= 1) {
			commandTimer = 0.f;
			act_ = ActionFase::Before;
		}
	}
	m_Position.y = Ease(In, Quart, commandTimer, s_posY, e_posY);


	m_ActionTimer += 1.0f / DecisionCount;
	if (m_ActionTimer >= 1.f && m_Position.y <= 1) {
		_charstate = STATE_CHOICE;
		m_ActionTimer = 0;
		commandTimer = 0.f;
	}
}
//攻撃の選択
void FirstBoss::Choice()
{
	fase_ = AttackFase::AttackBefore;
	act_ = ActionFase::Before;
	int l_RandState = 0;
	//乱数指定
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandomMove(0, 30);
	m_StopTimer++;
	//一定時間立ったらランダムで行動選択
	if (m_StopTimer > m_ChoiceInterval) {
		m_StopTimer = 0;
		l_RandState = int(l_RandomMove(mt));
		//_InterValState = UpState;
		//_charaState = STATE_MOVE;
		m_FollowSpeed = 1.0f;
		m_AfterPos.y = 30.0f;
		////RandStateが30以下ならそれに応じた移動にする、
		if (l_RandState < 12) {
			_charstate = CharaState::STATE_RAND;
		}
		else if (12 <= l_RandState && l_RandState < 26) {
			_charstate = CharaState::STATE_ROCKON;
			bounce_ = Bounce::UP;
			_rockonstate = RockonState::STATE_AIM;
		}
		else if (26 <= l_RandState && l_RandState <= 30) {
			_charstate = CharaState::STATE_HIT;
		}
	}
}
//ロックオン突進
void FirstBoss::RockOnAttack()
{
	if (fase_ == AttackFase::AttackBefore) {
		if (act_ == ActionFase::Before) {
			m_Position.y += 1.f;
			if (m_Position.y >= 15) {
				act_ = ActionFase::After;
			}
		}
		if (act_ == ActionFase::After) {
			m_Position.y -= 1.f;
			if (m_Position.y <= 0) {
				act_ = ActionFase::Before;
				waitCount += 1;
			}
		}
		if (waitCount >= 1) {
			fase_ = AttackFase::AttackAfter;
			waitCount = 0;
			commandTimer = 0.f;
			act_ = ActionFase::Before;
		}
	}
	if (fase_ != AttackFase::AttackAfter) { return; }
	
	Bounce();

	Areia();

	RockOn();

	Attack();
}
//動いてる途中に落とす
void FirstBoss::DropAttack()
{
	if (act_ == ActionFase::Before) {
		commandTimer += 1.0f / 30;
		Helper::GetInstance()->Clamp(commandTimer, 0.0f, 1.0f);
		float s_rot = 0.f;
		float e_rot = 90.f;
		m_Rotation = {
			Ease(Out, Quart, commandTimer, s_rot, e_rot),
			0.f,
			0.f,
		};
		if (m_Rotation.x >= 90) {
			act_ = ActionFase::After;
			commandTimer = 0;
		}
	}
	else {
		commandTimer += 1.0f / 30;
		Helper::GetInstance()->Clamp(commandTimer, 0.0f, 1.0f);
		float s_rot = 90.f;
		float e_rot = 0.f;
		m_Rotation = {
			Ease(Out, Quart, commandTimer, s_rot, e_rot),
			0.f,
			0.f,
		};
		if (m_Rotation.x <= 10) {
			FractionRockOn();
			CreateFraction(e_pos, m_Position);
			commandTimer = 0.f;
			m_Rotation.x = 0;
			_charstate = CharaState::STATE_INTER;
			act_ = ActionFase::Before;
			commandTimer = 0;
		}
	}
}
//中央行ってから拡散
void FirstBoss::Hit()
{
	if (fase_ == AttackFase::AttackBefore) {
		if (move_ == MoveFase::Move) {
			commandTimer += 1.0f / kMoveTimeMax;
			Helper::GetInstance()->Clamp(commandTimer, 0.0f, 1.0f);
			s_pos = m_Position;
			e_pos = { 0.f,0.f,0.f };
			m_Position = {
		Ease(Out, Quart, commandTimer, s_pos.x, e_pos.x),
		0.0f,
		Ease(Out, Quart, commandTimer, s_pos.z, e_pos.z),
			};
			if (m_Position.x == 0.f && m_Position.z == 0) {
				move_ = MoveFase::Stop;
				commandTimer = 0.f;
			}
		}
		if (move_ != MoveFase::Stop) { return; }
		if (act_ == ActionFase::Before) {
			commandTimer += 1.0f / kJumpTimeMax;
			Helper::GetInstance()->Clamp(commandTimer, 0.0f, 1.0f);
			//m_Position.y= Ease(In, Quad, commandTimer, 1.0f, 0.0f);
			m_Position.y += 0.5f;
			m_Rotation.x = Ease(In, Quad, commandTimer, 0.0f, 360.0f);
			if (m_Rotation.x < 360) { return; }
			act_ = ActionFase::After;
			m_Rotation.x = 0.f;
		}
		else if (act_ == ActionFase::After) {
			if (m_Position.y >= 0) {
				m_Position.y -= 2.f;
			}
			else if (m_Position.y <= 0) {
				fase_ = AttackFase::AttackAfter;
			}
		}
	}
	else if (fase_ == AttackFase::AttackAfter) {
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_distX(-50, -20);
		uniform_int_distribution<int> l_distZ(20, 50);
		XMFLOAT3 AreaOne = { float(l_distX(mt)),0.0f,float(l_distZ(mt)) };
		uniform_int_distribution<int> l_distX2(-50, -20);
		uniform_int_distribution<int> l_distZ2(-50, -20);
		XMFLOAT3 AreaTwo = { float(l_distX2(mt)),0.0f,float(l_distZ2(mt)) };
		uniform_int_distribution<int> l_distX3(20, 50);
		uniform_int_distribution<int> l_distZ3(-50, -20);
		XMFLOAT3 AreaThree = { float(l_distX3(mt)),0.0f,float(l_distZ3(mt)) };
		uniform_int_distribution<int> l_distX4(20, 50);
		uniform_int_distribution<int> l_distZ4(20, 50);
		XMFLOAT3 AreaFour = { float(l_distX4(mt)),0.0f,float(l_distZ4(mt)) };


		//乱数指定
		uniform_int_distribution<int> l_RandState(0, 40);
		if (attack_count_ < 5) {
			int ans = (l_RandState(mt)) % 4;
			if (ans == 0) {
				CreateFraction(AreaOne, m_Position);
			}
			else if (ans == 1) {
				CreateFraction(AreaTwo, m_Position);
			}
			else if (ans == 2) {
				CreateFraction(AreaThree, m_Position);
			}
			else if (ans == 3) {
				CreateFraction(AreaFour, m_Position);

			}
			attack_count_ += 1;
			_charstate = CharaState::STATE_HIT;
			fase_ = AttackFase::AttackBefore;
			commandTimer = 0.f;
			move_ = MoveFase::Move;
		}

		if (attack_count_ >= 5) {
			m_StopTimer++;
			//一定時間立ったらランダムで行動選択
			if (m_StopTimer > m_ChoiceInterval) {
				m_StopTimer = 0;
				_charstate = CharaState::STATE_INTER;
				fase_ = AttackFase::AttackBefore;
				commandTimer = 0.0f;
				move_ = MoveFase::Move;
				attack_count_ = 0;
				//bottlestate_ = BottleState::FRACTION;
			}
		}
	}
}

void FirstBoss::RockOn()
{
	if (_rockonstate != RockonState::STATE_AIM) { return; }
	beforeTimer += 1.0f / kLockOnTimeMax;
	Helper::GetInstance()->Clamp(beforeTimer, 0.0f, 1.0f);
	if (beforeTimer == 1.0f) {
		beforeTimer = 0.f;
		commandTimer = 0.0f;
		jumpCount++;
		rot = m_Rotation.y;
		s_pos = m_Position;
		e_pos = { m_Position.x + sinf(RottoPlayer) * -(10.f * (float)jumpCount),0.f, m_Position.z + cosf(RottoPlayer) * -(10.0f * (float)jumpCount) };
		//リミット制限
		Helper::GetInstance()->Clamp(e_pos.x, -55.0f, 65.0f);
		Helper::GetInstance()->Clamp(e_pos.z, -60.0f, 60.0f);
		_rockonstate = RockonState::STATE_ATTACK;
		Display = false;
	}
}

void FirstBoss::Attack()
{
	if (_rockonstate != RockonState::STATE_ATTACK) { return; }
	m_Frame++;
	if (m_Frame >= 5) {
		Display = true;
	}
	if (m_Frame >= 30) {
		m_Frame = 0;
		act_ = ActionFase::After;
	}
	//動く
	{
		if (act_ == ActionFase::After) {
			commandTimer += 1.0f / kJumpTimeMax;
			Helper::GetInstance()->Clamp(commandTimer, 0.0f, 1.0f);
		}
		m_Position = {
		Ease(Out, Quart, commandTimer, s_pos.x, e_pos.x),
		0.0f,
		Ease(Out, Quart, commandTimer, s_pos.z, e_pos.z),
		};
		
	}
	if (commandTimer == 1.0f) {
		if (jumpCount < kJumpCountMax) {
			_rockonstate = RockonState::STATE_AIM;
			fase_ = AttackFase::AttackBefore;
		}
		else {
			jumpCount = 0;
			_charstate = CharaState::STATE_INTER;
			bounce_ = Bounce::SOURCE;
			bounceTimer = 0.f;
			m_Alpha = {};
			Display = false;
		}
		commandTimer = 0.0f;
	}
}


void FirstBoss::FaceToOrientation()
{
	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();
	//角度の取得 プレイヤーが敵の索敵位置に入ったら向きをプレイヤーの方に
	XMVECTOR PositionA = { l_player.x,l_player.y,l_player.z };
	XMVECTOR PositionB = { m_Position.x,m_Position.y,m_Position.z };
	//プレイヤーと敵のベクトルの長さ(差)を求める
	XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;
	RottoPlayer = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);
	m_Rotation.y = RottoPlayer * 60.0f + (PI_90 + PI_180) + 180.f;
}

void FirstBoss::CreateFraction(const XMFLOAT3& DropPos, const XMFLOAT3& BossPos)
{
	unique_ptr<Fraction> fraction = make_unique<Fraction>();
	fraction->Init(BossPos);
	fraction->Drop(DropPos);
	fraction_.push_back(std::move(fraction));
}

void FirstBoss::Bounce()
{
	if (bounce_ == Bounce::SOURCE) { return; }
	XMFLOAT3 s_scl = m_Scale;
	bounceTimer += 1.0f / 60;
	Helper::GetInstance()->Clamp(bounceTimer, 0.0f, 1.0f);
	if (bounce_ == Bounce::UP) {
		XMFLOAT3 E_scl{ e_scl.x-5.f,e_scl.y,e_scl.z-5.f };
		m_Scale = {
	Ease(In, Quart, bounceTimer, s_scl.x, E_scl.x),
	Ease(In, Quart, bounceTimer, s_scl.y, E_scl.y),
	Ease(In, Quart, bounceTimer, s_scl.z, E_scl.z)
		};
		if (bounceTimer >= 1) {
			bounce_ = Bounce::DOWN;
			bounceTimer = 0.f;
		}
	}
	else if (bounce_ == Bounce::DOWN) {
		XMFLOAT3 E_scl{ e_scl.x,e_scl.y-10.f,e_scl.z };
		m_Scale = {
	Ease(In, Quart, bounceTimer, s_scl.x, E_scl.x),
	Ease(In, Quart, bounceTimer, s_scl.y, E_scl.y),
	Ease(In, Quart, bounceTimer, s_scl.z, E_scl.z)
		};
		if (bounceTimer >= 1) {
			bounce_ = Bounce::UP;
			bounceTimer = 0.f;
		}
	}
}

void FirstBoss::Areia()
{
	m_TexPos = {
	m_TexPos.x = (m_Position.x + e_pos.x) / 2,
	0.0f,
	m_TexPos.z = (m_Position.z + e_pos.z) / 2,
	};

	m_TexScale.y = Helper::GetInstance()->ChechLength(m_Position, e_pos) * 0.1f;
	tex->Update();
	tex->SetPosition(m_TexPos);
	tex->SetRotation(m_TexRot);
	tex->SetScale(m_TexScale);
	tex->SetColor({ m_TexColor.x,m_TexColor.y,m_TexColor.z,1 });

	m_TexRot.y = Helper::GetInstance()->DirRotation(m_Position, e_pos, -PI_180);
}


void FirstBoss::FractionRockOn()
{
	rot = m_Rotation.y;
	jumpCount = 3;
	s_pos = m_Position;
	e_pos = { m_Position.x + sinf(RottoPlayer) * -(20.f * (float)jumpCount),0.f, m_Position.z + cosf(RottoPlayer) * -(20.0f * (float)jumpCount) };
}

void FirstBoss::Big()
{
	Revert();

	if (returnflag) { return; }
	XMFLOAT3 s_scl = m_Scale;
	bigtimer_ += 1.0f / 60;
	Helper::GetInstance()->Clamp(bigtimer_, 0.0f, 1.0f);
	m_Scale = {
	Ease(In, Quart, bigtimer_, s_scl.x, e_scl.x),
	Ease(In, Quart, bigtimer_, s_scl.y, e_scl.y),
	Ease(In, Quart, bigtimer_, s_scl.z, e_scl.z),
	};


	if (bigtimer_ == 1) {
		_charstate = CharaState::STATE_INTER;
	}
}

void FirstBoss::Revert()
{
	if (!returnflag) { return; }

	XMFLOAT3 s_scl = m_Scale;
	reverttimer_ += 1.0f / 60;
	Helper::GetInstance()->Clamp(reverttimer_, 0.0f, 1.0f);
	m_Scale = {
	Ease(In, Quart, reverttimer_, s_scl.x, e_scl.x),
	Ease(In, Quart, reverttimer_, s_scl.y, e_scl.y),
	Ease(In, Quart, reverttimer_, s_scl.z, e_scl.z),
	};

	float s_rot = nowrot_;
	float e_rot = 0.f;
	m_Rotation = {
		Ease(In, Quart, reverttimer_, s_rot, e_rot),
		0,
		0,
	};

	XMFLOAT3 s_pos = m_Position;
	m_Position = {
		s_pos.x,
		Ease(In, Quart, reverttimer_, s_pos.y, e_pos.y),
		s_pos.z,
	};

	if (reverttimer_ == 1) {
		e_scl = { 25.3f,25.3f,25.3f };
		returnflag = false;
	}
}


void FirstBoss::InitAwake() {

}

void FirstBoss::EndRollAction() {
	const float l_AddPosX = 0.4f;
	const float l_AddFrame = 0.01f;
	m_EndTimer++;
	if (_EndState == END_SET) {
		if (m_EndTimer == 1) {
			m_Color = { 1.0f,1.0f,1.0f,1.0f };
			m_Position = { 55.0f,2.0f,10.0f };
			m_Rotation = { 0.0f,180.0f,0.0f };
		}
		else if (m_EndTimer == 290) {
			_EndState = END_WALK;
		}
	}
	else if (_EndState == END_WALK) {
		if (Helper::GetInstance()->CheckMax(m_Position.x, 10.0f, -l_AddPosX)) {
			_EndState = END_DIR_CAMERA;
		}
	}
	else {
		Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame);
		m_Rotation.y = Ease(In, Cubic, m_Frame, m_Rotation.y, 90.0f);

		if (m_EndTimer == 1670) {
			m_EndStop = true;
		}
	}
	//sin波によって上下に動く
	if (!m_EndStop) {
		m_SinAngle += 6.0f;
		m_SinAngle2 = m_SinAngle * (3.14f / 180.0f);
		m_Position.y = (sin(m_SinAngle2) * 0.5f + 2.0f);
	}
	//OBJのステータスのセット
	Obj_SetParam();
}
//会話
void FirstBoss::AfterAction() {

}
