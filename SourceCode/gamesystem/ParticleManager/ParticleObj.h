#pragma once
#include "IKEModel.h"
#include "IKEObject3d.h"

#include <array>   
#include <memory>
using namespace std;         //  ���O��Ԏw��
//OBJ�̃p�[�e�B�N��
class ParticleObj {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	ParticleObj();
public:

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update(const XMFLOAT3& StartPos, int Timer, int TargetTimer, int ParticleType);

	//�e�p�[�e�B�N���̓���
	void NormalParticle(const XMFLOAT3& StartPos, int Timer, int TargetTimer);

	void WideParticle(const XMFLOAT3& StartPos, int Timer, int TargetTimer);

	void UpParticle(const XMFLOAT3& StartPos, int Timer, int TargetTimer);

	void EndParticle(const XMFLOAT3& StartPos, int Timer, int TargetTimer);
	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	void ImGuiDraw();
private:
	//�萔
	static const int Particle_Max = 40;//�p�[�e�B�N���̐�
public:
	void SetStartColor(const XMFLOAT4& m_color) { this->m_StartColor = m_color; }
	void SetParticleType(int m_ParticleType) { this->m_ParticleType = m_ParticleType; }
	void SetParticleBreak(bool m_ParticleBreak) { this->m_ParticleBreak = m_ParticleBreak; }
	void SetStartScale(float m_StartScale) { this->m_StartScale = m_StartScale; }
	void SetAddScale(float  m_AddScale) { this->m_AddScale = m_AddScale; }
	void SetEndCount(int m_EndCount) { this->m_EndCount = m_EndCount; }
private:
	//���f����OBJ
	IKEModel* model = nullptr;
	array<unique_ptr <IKEObject3d>, Particle_Max> object;

	//�ϐ�
	array<XMFLOAT3, Particle_Max> m_pos;//���W
	array<float, Particle_Max> m_Angle;//�p�x
	array<XMFLOAT4, Particle_Max> m_color;//�F
	array<XMFLOAT2, Particle_Max> m_speed;//�ړ����x
	array<XMFLOAT3, Particle_Max> m_scale;//�傫��
	array<XMFLOAT3, Particle_Max> m_AddPower;//������
	array<float, Particle_Max> m_power;
	XMFLOAT4 m_StartColor = { 1.0f,1.0f,1.0f,1.0f };//�n�܂�̐F


	array<bool, Particle_Max> m_Alive;//�������Ă��邩
	array<bool, Particle_Max> m_End;//�������Ă��邩
	array<bool, Particle_Max> m_ScaleChange;//�傫���̕ύX
	array<int, Particle_Max> m_Number;//�p�[�e�B�N���̓�������
	array<int, Particle_Max> m_Timer;//�p�[�e�B�N���̎���
	XMFLOAT3 m_StartPos = { 0.0f,0.0f,0.0f };

	//�e�p�[�e�B�N���̐�
	//unique_ptr<GamePlayer> player;
	int m_NormalParticleCount = 60;
	int m_WideParticleCount = 10;
	int m_ParticleType = 0;
	int m_EndCount = 0;
	bool m_ParticleBreak = false;
	float m_StartScale = 0.1f;
	float m_AddScale = 0.02f;
	enum Type {
		Normal,
		Up,
		End,
	};
};