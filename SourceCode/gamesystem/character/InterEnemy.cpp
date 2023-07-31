#include "InterEnemy.h"
#include "Collision.h"
#include "Helper.h"
#include "Player.h"
//Player* InterEnemy::player = nullptr;
//������
bool InterEnemy::Initialize() {
	return true;
}
//�X�V
void InterEnemy::Update() {
	//�s��
	Action();
}
//�`��
void InterEnemy::Draw(DirectXCommon* dxCommon) {
}
//void InterEnemy::PlayerGet(Player* _player) {
//
//	//InterEnemy::player = _player;
//
//}

void InterEnemy::OnCollision()
{
	if (!isAlive)return;
	//�U���̒e�Ƃ̔���
	for (InterBullet* _bullet : Player::GetInstance()->GetBulllet_attack()) {
		if (_bullet->GetAlive()) {
			if (Collision::CircleCollision(_bullet->GetPosition().x, _bullet->GetPosition().z, _bullet->GetRadius(), m_Position.x, m_Position.z, radius))
			{
				Audio::GetInstance()->PlayWave("Resources/Sound/SE/Attack_Normal.wav", VolumManager::GetInstance()->GetSEVolum());
				//�̗�
				HP--;
				if (HP <= 0)
				{
					DeathEffectF = true;
				}_bullet->SetAlive(false);
				
			}
		}
	}
	if (HP <= 0)
	{
		isAlive = false;
	}
}

void InterEnemy::DeathAction()
{
	if (isAlive)return;

	m_Color.w -= 0.033f;

	Helper::GetInstance()->Clamp(m_Color.w, 0.f, 1.f);
}

void InterEnemy::ColPlayer()
{
	if (!isAlive) { return; }
	if (Collision::CircleCollision(Player::GetInstance()->GetPosition().x, Player::GetInstance()->GetPosition().z, 2.f, m_Position.x, m_Position.z, 1.f)
		&&Player::GetInstance()->GetDamageInterVal()==0)
	{

		Player::GetInstance()->PlayerHit(m_Position);
		if (isUnrival) {
			Player::GetInstance()->RecvDamage(0);
		} else {
			Player::GetInstance()->RecvDamage(0.75f);
		}
	}

	if (HP <= 0)
	{
		isAlive = false;
	}

}


void InterEnemy::EnemyColNormal(const XMFLOAT3& pos) {
	m_Position.x += sin(atan2f((m_Position.x - pos.x), (m_Position.z - pos.z))) * 0.1f;
	m_Position.z += cos(atan2f((m_Position.x - pos.x), (m_Position.z - pos.z))) * 0.1f;
}
