#include "ParticleEmitter.h"
#include "VariableCommon.h"
#include "ImageManager.h"
#include "Helper.h"
#include <random>
ParticleEmitter* ParticleEmitter::GetInstance()
{
	static ParticleEmitter instance;

	return &instance;
}

void ParticleEmitter::Initialize()
{
	//テクスチャ全読み込み
	LoadTexture();
	//パーティクルマネージャー生成
	circleParticle.reset(ParticleManager::Create(ImageManager::Normal));
	darkParticle.reset(ParticleManager::Create(ImageManager::Normal));
	kotokoParticle.reset(ParticleManager::Create(ImageManager::Normal));
	darkcircleParticle.reset(ParticleManager::Create(ImageManager::Normal));
	healParticle.reset(ParticleManager::Create(ImageManager::Heal));
	deathParticle.reset(ParticleManager::Create(ImageManager::Normal));
	BossDeadParticle.reset(ParticleManager::Create(ImageManager::Normal));
	wallParticle.reset(ParticleManager::Create(ImageManager::Normal));
	PhotoParticle.reset(ParticleManager::Create(ImageManager::Photo));
}


void ParticleEmitter::Update()
{
	//パーティクルマネージャー更新
	circleParticle->Update();
	darkcircleParticle->Update();
	darkParticle->Update();
	kotokoParticle->Update();
	healParticle->Update();
	deathParticle->Update();
	BossDeadParticle->Update();
	wallParticle->Update();
	PhotoParticle->Update();
}

void ParticleEmitter::IntroDraw() {
	darkParticle->Draw(AddBlendType);
}
void ParticleEmitter::FlontDrawAll() {
	circleParticle->Draw(AddBlendType);
	PhotoParticle->Draw(AddBlendType);
}

void ParticleEmitter::DeathDrawAll() {
	BossDeadParticle->Draw(AddBlendType);
	deathParticle->Draw(AddBlendType);
}
void ParticleEmitter::BackDrawAll() {
	wallParticle->Draw(AddBlendType);
	healParticle->Draw(AddBlendType);
	kotokoParticle->Draw(AddBlendType);
	darkcircleParticle->Draw(AddBlendType);
}

void ParticleEmitter::FireEffect(const int life, const XMFLOAT3& l_pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor)
{
	XMFLOAT3 pos = l_pos;
	const float rnd_vel = 0.05f;
	XMFLOAT3 vel{};
	vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
	vel.y = (float)rand() / RAND_MAX * rnd_vel * 2.0f;// -rnd_vel / 2.0f;
	vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

	circleParticle->Add(life, { pos.x,pos.y,pos.z }, vel, {}, startscale, endscale, startcolor, endcolor,{});
}

void ParticleEmitter::DarkOtiEffect(const int life, const XMFLOAT3& l_pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor)
{
	XMFLOAT3 pos = l_pos;
	const float rnd_vel = 0.05f;
	XMFLOAT3 vel{};
	
	float angle = (float)rand() / RAND_MAX * 360.0f;

	vel.x = 0.2f * sinf(angle);
	vel.y =0.2f * cosf(angle);
	vel.z = 0.0f;
	darkcircleParticle->Add(life, { pos.x,pos.y,pos.z }, vel, {}, startscale, endscale, startcolor, endcolor, {});
}

void ParticleEmitter::KotokoEffect(const int life, const XMFLOAT3& l_pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor)
{
	XMFLOAT3 pos = l_pos;
	const float rnd_vel = 0.05f;
	XMFLOAT3 vel{};
	vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
	vel.y = (float)rand() / RAND_MAX * rnd_vel * 2.0f;// -rnd_vel / 2.0f;
	vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

	kotokoParticle->Add(life, { pos.x,pos.y,pos.z }, vel, {}, startscale, endscale, startcolor, endcolor, {});
}

//移動制御
void ParticleEmitter::LimitEffect(const int life, const XMFLOAT3& l_pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor)
{
	XMFLOAT3 pos = l_pos;
	const float rnd_vel = 0.05f;
	XMFLOAT3 vel{};
	vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f;
	vel.y = (float)rand() / RAND_MAX * rnd_vel * 2.0f;// -rnd_vel / 2.0f;
	vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f;

	wallParticle->Add(life, { pos.x,pos.y,pos.z }, vel, {}, startscale, endscale, startcolor, endcolor, {});
}

//移動制御
void ParticleEmitter::DarkEffect(const int life, const XMFLOAT3& l_pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor)
{
	XMFLOAT3 pos = l_pos;
	const float rnd_vel = 0.05f;
	XMFLOAT3 vel{};
	vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f;
	vel.y = (float)rand() / RAND_MAX * rnd_vel * 6.0f;// -rnd_vel / 2.0f;
	vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 4.0f;

	darkParticle->Add(life, { pos.x,pos.y,pos.z }, vel, {}, startscale, endscale, startcolor, endcolor, {});
}
//爆発
void ParticleEmitter::Explosion(const int life, const XMFLOAT3& pos, const float size, const float startscale,
	const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor, const int ExploType) {
	float RandPos = {};
	//X,Y,Z全て[-0.8f, +0.8f]でランダムに分布
	float RandPos2 = {};
	//X,Y,Z全て[-0.05f, +0.05f]でランダムに分布
	float RandVel = {};
	//爆発の規模でエフェクトも変わる
	if (ExploType == 0) {
		RandPos = 2.0f;
		RandPos2 = 0.8f;
		RandVel = 0.05f;
	}
	else if (ExploType == 1) {
		RandPos = 9.0f;
		RandPos2 = 0.8f;
		RandVel = 0.05f;
	}
	else {
		RandPos = 12.0f;
		RandPos2 = 0.8f;
		RandVel = 0.05f;
	}
	for (int j = 0; j < 3; j++) {
		XMFLOAT3 l_pos = pos;
		l_pos.x += ((float)rand() / RAND_MAX * RandPos - RandPos / 2.0f) * size;
		l_pos.y += ((float)rand() / RAND_MAX * RandPos - RandPos / 2.0f) * size;
		l_pos.z += ((float)rand() / RAND_MAX * RandPos - RandPos / 2.0f) * size;

		for (int i = 0; i < 10; i++) {
			
			l_pos.x += ((float)rand() / RAND_MAX * RandPos2 - RandPos2 / 2.0f) * size;
			l_pos.y += ((float)rand() / RAND_MAX * RandPos2 - RandPos2 / 2.0f) * size;
			l_pos.z += ((float)rand() / RAND_MAX * RandPos2 - RandPos2 / 2.0f) * size;

			
			XMFLOAT3 vel{};
			vel.x = ((float)rand() / RAND_MAX * RandVel - RandVel / 2.0f) * size;
			vel.y = ((float)rand() / RAND_MAX * RandVel - RandVel / 2.0f) * size;
			vel.z = ((float)rand() / RAND_MAX * RandVel - RandVel / 2.0f) * size;


			//追加
			circleParticle->Add(life, l_pos, vel, {}, startscale, endscale, startcolor, endcolor,{});
		}
	}
}

//ダメージ時のエフェクト
void ParticleEmitter::Break(const int life, const XMFLOAT3& pos,const float startscale, const float endscale,
	const XMFLOAT4& startcolor, const XMFLOAT4& endcolor,const float Gravity,float divi) {
	const int l_Division = 80;
	float l_AddPowerY = 0.0f;
	//乱数の範囲(-50から50)
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_bounddist(-50, 50);

	XMFLOAT3 l_pos = pos;

	XMFLOAT3 vel{};
	
	vel = {
		(float)(l_bounddist(mt)) / l_Division,
		(float)(l_bounddist(mt)) / l_Division,
		(float)(l_bounddist(mt)) / l_Division,
	};

	vel.y += l_AddPowerY;

	circleParticle->Add(life, l_pos, vel, {}, startscale, endscale, startcolor, endcolor,Gravity,divi);
}

void ParticleEmitter::DeathEffect(int life, XMFLOAT3 l_pos, float startscale, float endscale, XMFLOAT4 startcolor, XMFLOAT4 endcolor, float velocity) {
	XMFLOAT3 pos = l_pos;
	float angle = (float)rand() / RAND_MAX * 360.0f;
	const float rnd_vel = 0.1f;
	XMFLOAT3 vel{};
	vel.x = velocity * sinf(angle);
	vel.y = velocity * cosf(angle);
	vel.z = 0.0f;
	deathParticle->Add(life, pos, vel, XMFLOAT3(), startscale, endscale, startcolor, endcolor,{});
}

void ParticleEmitter::HealEffect(const int life, const XMFLOAT3& l_pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor) {
	XMFLOAT3 pos = l_pos;
	const float rnd_vel = 0.05f;
	XMFLOAT3 vel{};
	vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
	vel.y = (float)rand() / RAND_MAX * rnd_vel * 2.0f;// -rnd_vel / 2.0f;
	vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

	healParticle->Add(life, { pos.x,pos.y,pos.z }, vel, {}, startscale, endscale, startcolor, endcolor, {});
}
void ParticleEmitter::CameraEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor) {
	const float rnd_vel = 0.1f;
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandPos(-10, 10);
	XMFLOAT3 vel{};
	vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
	vel.y = (float)rand() / RAND_MAX * rnd_vel * 2.0f;// -rnd_vel / 2.0f;
	vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

	PhotoParticle->Add(life, { pos.x+ (float)l_RandPos(mt),pos.y,pos.z + (float)l_RandPos(mt) }, vel, {}, startscale, endscale, startcolor, endcolor, {});

}

void ParticleEmitter::SelectEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor) {
	const float rnd_vel = 0.1f;
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandPos(-3, 3);
	XMFLOAT3 vel{};
	vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
	vel.y = (float)rand() / RAND_MAX * rnd_vel * 2.0f;// -rnd_vel / 2.0f;
	vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

	PhotoParticle->Add(life, { pos.x + (float)l_RandPos(mt),pos.y,pos.z + (float)l_RandPos(mt) }, vel, {}, startscale, endscale, startcolor, endcolor, {});

}
void ParticleEmitter::AllDelete()
{
	//パーティクルマネージャー更新
	circleParticle->AllDelete();
	darkcircleParticle->AllDelete();
	darkParticle->AllDelete();
	kotokoParticle->AllDelete();
	healParticle->AllDelete();
	deathParticle->AllDelete();
	BossDeadParticle->AllDelete();
	wallParticle->AllDelete();
	PhotoParticle->AllDelete();
}

void ParticleEmitter::LoadTexture() {
	ImageManager::GetInstance()->LoadParticle();
}

void ParticleEmitter::DeathEffectBoss(const int life, const XMFLOAT3& pos, const float size, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor)
{
		for (int j = 0; j < 3; j++) {
			//X,Y,Z全て[-2.0f, +2.0f]でランダムに分布
			const float RandPos = 2.0f;
			XMFLOAT3 l_pos = pos;
			l_pos.x += ((float)rand() / RAND_MAX * RandPos - RandPos / 2.0f) * size;
			l_pos.y += ((float)rand() / RAND_MAX * RandPos - RandPos / 2.0f) * size;
			l_pos.z += ((float)rand() / RAND_MAX * RandPos - RandPos / 2.0f) * size;

			for (int i = 0; i < 10; i++) {
				//X,Y,Z全て[-0.8f, +0.8f]でランダムに分布
				const float RandPos2 = 0.8f;
				l_pos.x += ((float)rand() / RAND_MAX * RandPos2 - RandPos2 / 2.0f) * size;
				l_pos.y += ((float)rand() / RAND_MAX * RandPos2 - RandPos2 / 2.0f) * size;
				l_pos.z += ((float)rand() / RAND_MAX * RandPos2 - RandPos2 / 2.0f) * size;

				//X,Y,Z全て[-0.05f, +0.05f]でランダムに分布
				const float RandVel = 0.05f;
				XMFLOAT3 vel{};
				vel.x = ((float)rand() / RAND_MAX * RandVel - RandVel / 2.0f) * size;
				vel.y = ((float)rand() / RAND_MAX * RandVel - RandVel / 2.0f) * size;
				vel.z = ((float)rand() / RAND_MAX * RandVel - RandVel / 2.0f) * size;

				//追加
				BossDeadParticle->Add(life, l_pos, vel, {}, startscale, endscale, startcolor, endcolor, {});
			}
		}
}

void ParticleEmitter::ExproEffectBoss(const int life, const XMFLOAT3& pos, const float size, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor) {
	for (int j = 0; j < 3; j++) {
		//X,Y,Z全て[-2.0f, +2.0f]でランダムに分布
		const float RandPos = 2.0f;
		XMFLOAT3 l_pos = pos;
		l_pos.x += ((float)rand() / RAND_MAX * RandPos - RandPos / 2.0f) * size;
		l_pos.y += ((float)rand() / RAND_MAX * RandPos - RandPos / 2.0f) * size;
		l_pos.z += ((float)rand() / RAND_MAX * RandPos - RandPos / 2.0f) * size;

		for (int i = 0; i < 10; i++) {
			//X,Y,Z全て[-0.8f, +0.8f]でランダムに分布
			const float RandPos2 = 0.8f;
			l_pos.x += ((float)rand() / RAND_MAX * RandPos2 - RandPos2 / 2.0f) * size;
			l_pos.y += ((float)rand() / RAND_MAX * RandPos2 - RandPos2 / 2.0f) * size;
			l_pos.z += ((float)rand() / RAND_MAX * RandPos2 - RandPos2 / 2.0f) * size;

			//X,Y,Z全て[-0.05f, +0.05f]でランダムに分布
			const float RandVel = 0.05f;
			XMFLOAT3 vel{};
			vel.x = ((float)rand() / RAND_MAX * RandVel - RandVel / 2.0f) * size;
			vel.y = ((float)rand() / RAND_MAX * RandVel - RandVel / 2.0f) * size;
			vel.z = ((float)rand() / RAND_MAX * RandVel - RandVel / 2.0f) * size;


			//追加
			BossDeadParticle->Add(life, l_pos, vel, {}, startscale, endscale, startcolor, endcolor, {});
		}
	}

}
