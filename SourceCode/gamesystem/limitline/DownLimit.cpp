#include "DownLimit.h"
#include "ParticleEmitter.h"
#include <random>

DownLimit* DownLimit::GetInstance()
{
	static DownLimit instance;

	return &instance;
}

void DownLimit::LimitParticle() {
	XMFLOAT4 s_color = { 1.0f,1.0f,1.0f,1.0f };
	XMFLOAT4 e_color = { 1.0f,1.0f,1.0f,1.0f };
	float s_scale = 3.0f;
	float e_scale = 0.0f;
	XMFLOAT3 l_birthPos = {};
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> l_RandX(-50, 68);
	uniform_int_distribution<int> l_RandZ(-68, -67);
	l_birthPos.x = float(l_RandX(mt)) + m_ParticlePos.x;
	l_birthPos.z = float(l_RandZ(mt)) + m_ParticlePos.z;
	l_birthPos.y = 0.0f;

	for (int i = 0; i < 3; i++) {
		ParticleEmitter::GetInstance()->LimitEffect(100, l_birthPos, s_scale, e_scale, s_color, e_color);
	}
}