#include "InterAttack.h"
#include "Player.h"

void InterAttack::FollowPlayer()
{
	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();

	//�p�x�̎擾 �v���C���[���G�̍��G�ʒu�ɓ�������������v���C���[�̕���
	XMVECTOR PositionA = { l_player.x,l_player.y,l_player.z };
	XMVECTOR PositionB = { boss->GetPosition().x,
		boss->GetPosition().y,
	boss->GetPosition().z };

	//�v���C���[�ƓG�̃x�N�g���̒���(��)�����߂�
	XMVECTOR SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;

	float RottoPlayer;
	RottoPlayer = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);

	boss->SetRotation({ 0.f, RottoPlayer * 60.f + 180.f,0.f });
}
