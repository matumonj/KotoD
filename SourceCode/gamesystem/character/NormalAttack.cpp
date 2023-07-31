#include "NormalAttack.h"
void NormalAttack::Init()
{

}

void NormalAttack::Upda()
{
	FollowPlayer();
	XMVECTOR move = { 0.f,0.f, 0.1f, 0.0f };

	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(boss->GetRotation().y));

	move = XMVector3TransformNormal(move, matRot);

	float SummonSpeed = 2.f;
	m_Position = {
					m_Position.x + move.m128_f32[0] * 6,
				m_Position.y,
				m_Position.z + move.m128_f32[2] * 6
	};

	boss->SetPosition(m_Position);
}


void NormalAttack::Draw(DirectXCommon* dxCommon)
{

}

void NormalAttack::SpriteDraw()
{

}

void NormalAttack::DeathUpdate(int Timer) {

}


