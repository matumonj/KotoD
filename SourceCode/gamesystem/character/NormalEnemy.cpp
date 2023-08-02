#include "NormalEnemy.h"
#include <random>
#include "Player.h"
#include "Collision.h"
#include "CsvLoader.h"
#include "Helper.h"
//���f���ǂݍ���
NormalEnemy::NormalEnemy() {
	m_fbxModel = ModelManager::GetInstance()->GetFBXModel(ModelManager::SIORI);
}
//������
bool NormalEnemy::Initialize() {

	m_fbxObject = make_unique<IKEFBXObject3d>();
	m_fbxObject->Initialize();
	m_fbxObject->SetModel(m_fbxModel);
	m_fbxObject->LoadAnimation();

	m_Scale = { 1.10f,1.10f,1.10f };
	m_Color = { 1.0f,1.0f,1.0f,1.0f };
	m_Position.y = 0.0f;
	ret = true;
	HP = 1;
	isAlive = true;

	m_fbxObject->PlayAnimation(0);

	SummonSpeed = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/SECOND/SECONDboss.csv", "SummonSpeed")));

	return true;
}
//�s��
void NormalEnemy::Action() {
	float l_Vel = 0.15f;
	//m_CircleSpeed += 1.0f;
	m_Rotation.x = 0.f;

	if (ShotF) {
		Appearance();
		RushAction();
	}
	else
	{
		t = 0.f;
	}

	//Obj_SetParam();	//m_Position = Helper::GetInstance()->CircleMove({ 0.0f,5.0f,0.0f }, m_CircleScale, m_CircleSpeed);
	m_fbxObject->SetScale({ 0.1f,0.1f,0.1f });
	m_Scale = { 0.010f,0.010f,0.010f };
	Fbx_SetParam();
	m_fbxObject->Update(m_LoopFlag, m_AnimationSpeed, m_StopFlag);
	OnCollision();
	ColPlayer();
	Particle();
	
	DeathEffect();
	DeathAction();
}
//�`��
void NormalEnemy::Draw(DirectXCommon* dxCommon) {

	if (m_Color.w <= 0.f)return;
	IKEObject3d::PreDraw();
	Fbx_Draw(dxCommon);
	//Obj_Draw();
}
//ImGui�`��
void NormalEnemy::ImGuiDraw() {
	ImGui::Begin("Enemy");
	//ImGui::Text("time %f", t);
	ImGui::Text("RotOld %f", old);

	ImGui::Text("EnePosX:%f", m_Position.x);
	ImGui::Text("EnePosZ:%f", m_Position.z);
	ImGui::End();
}
//�J��
void NormalEnemy::Finalize() {

}

//�p�[�e�B�N��
void NormalEnemy::Particle() {
	if (!isAlive)return;
	XMFLOAT4 s_color = { 1.0f,0.5f,1.0f,1.5f };
	XMFLOAT4 e_color = { 1.0f,0.5f,1.0f,0.1f };
	//float s_scale = 3.0f;
	float e_scale = 0.0f;
	ParticleEmitter::GetInstance()->FireEffect(10, m_Position, s_scale, e_scale, s_color, e_color);
}


void NormalEnemy::DeathEffect()
{
	if (!DeathEffectF)return;

		float l_AddSize = 2.5f;
		const float RandScale = 3.0f;
		float s_scale = 0.3f * l_AddSize;
		float e_scale = (4.0f + (float)rand() / RAND_MAX * RandScale - RandScale / 2.0f) * l_AddSize;

		//�F
		const float RandRed = 0.2f;
		const float red = 0.2f + (float)rand() / RAND_MAX * RandRed;
		const XMFLOAT4 s_color = { 0.9f, red, 0.1f, 1.0f }; //�Z����
		const XMFLOAT4 e_color = { 0, 0, 0, 1.0f }; //���F

		//�����w��
		mt19937 mt{ std::random_device{}() };
		uniform_int_distribution<int> l_Randlife(10, 40);
		int l_Life = int(l_Randlife(mt));

		ParticleEmitter::GetInstance()->Explosion(l_Life, m_Position, l_AddSize, s_scale, e_scale, s_color, e_color,0);
		DeathEffectF = false;
}

void NormalEnemy::Appearance()
{
	if (Rush)return;
	XMFLOAT3 l_player = Player::GetInstance()->GetPosition();

	//�p�x�̎擾 �v���C���[���G�̍��G�ʒu�ɓ�������������v���C���[�̕���
	PositionA = { l_player.x,l_player.y,l_player.z };
	PositionB = { m_Position.x,m_Position.y,m_Position.z };
	//�v���C���[�ƓG�̃x�N�g���̒���(��)�����߂�
	SubVector = XMVectorSubtract(PositionB, PositionA); // positionA - positionB;

	constexpr float AddScaling = 0.101f;

	m_Scale.x += AddScaling;
	m_Scale.y += AddScaling;
	m_Scale.z += AddScaling;

	Helper::GetInstance()->Clamp(m_Scale.x, 0.f, 1.1010f);
	Helper::GetInstance()->Clamp(m_Scale.y, 0.f, 1.0910f);
	Helper::GetInstance()->Clamp(m_Scale.z, 0.f, 1.1006f);

	float RottoPlayer;
	RottoPlayer = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);
	m_Rotation.y = RottoPlayer * 60.f + 180.f;


	ShotCount++;

	if (ShotCount>200)
	{
		Rush = true;
	}




}

void NormalEnemy::RushAction()
{
	if (!Rush)return;

	XMFLOAT3 PPos = Player::GetInstance()->GetPosition();

	ShotCount = 0;
	s_scale += 0.02f;

	Helper::GetInstance()->Clamp(s_scale, 0.f, 5.f);

	 // positionA - positionB;
	//��]�����v���C���[�̕���
		//����������

	
	if (ret)
	{
		MoveTimer = 0;

		XMFLOAT3 l_player = Player::GetInstance()->GetPosition();
		PositionA = { l_player.x,l_player.y,l_player.z };
		PositionB = { m_Position.x,m_Position.y,m_Position.z };
		//�v���C���[�ƓG�̃x�N�g���̒���(��)�����߂�
		SubVector = XMVectorSubtract(PositionB, PositionA);
		RotY = atan2f(SubVector.m128_f32[0], SubVector.m128_f32[2]);

		//�C�[�W���O�J�E���^�{�{
		t += 0.03f;
		//Rotation���f
		//if (randMove > 50) {
		if(canRot)
		m_Rotation.y = Easing::EaseOut(t, old * 60.f + 180.f, RotY * 60.f+180.f);
		if (t >= 1.f) {
			canRot = false;
			ret = false;
		}
		tmpLength = Collision::GetLength(XMFLOAT2(m_Position.x, m_Position.z), XMFLOAT2(PPos.x, PPos.z));
	} else
	{
		float NowLength= Collision::GetLength(XMFLOAT2(m_Position.x, m_Position.z), XMFLOAT2(PPos.x, PPos.z));;

		bool JudgRet = MoveTimer>120&&sqrtf((NowLength-tmpLength)* (NowLength - tmpLength))<20;
		
		t = 0.f;
		old = RotY;
		MoveTimer++;

		if ((JudgRet)||MoveTimer>200) {
			OldPos_BefRot = m_Position;
			canRot = true;
			randMove = rand() % 100;
			ret = true;
		}
	}
	XMVECTOR move = { 0.f,0.f, 0.1f, 0.0f };

	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));

	move = XMVector3TransformNormal(move, matRot);

	//�����ς��Ă�Œ��͓����~�߂�
	bool stopMove = !(t > 0.01f);
	if (stopMove) {
		if (isAlive) {
			m_Position = {
					m_Position.x + move.m128_f32[0] * SummonSpeed,
				m_Position.y,
				m_Position.z + move.m128_f32[2] * SummonSpeed
			};
		}
	}
	Helper::GetInstance()->Clamp(t, 0.f, 1.f);

}


