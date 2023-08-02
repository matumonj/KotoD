#include "BreakEffect.h"
#include "Helper.h"
#include <random>

BreakEffect::BreakEffect() {

}

bool BreakEffect::Initialize() {
	m_Alive = true;
	return true;
}

void BreakEffect::Action() {
	const int l_TargetTimer = 5;
	if (m_Alive) {
		BirthEffect();

		if (Helper::GetInstance()->CheckMax(m_AliveTimer, l_TargetTimer, 1)) {
			m_Alive = false;
		}
	}
}

void BreakEffect::BirthEffect() {

	const XMFLOAT4 s_color = { 1.0f,1.0f,1.0f,1.0f };
	const XMFLOAT4 e_color = m_Color;
	const float s_scale = 3.0f;
	const float e_scale = 0.0f;
	for (int i = 0; i < 30; i++) {
		ParticleEmitter::GetInstance()->Break(m_Life, m_Position, s_scale, e_scale, s_color, e_color,0.02f,DiviSpeed);
	}
}

void BreakEffect::ImGuiOrigin() {
}