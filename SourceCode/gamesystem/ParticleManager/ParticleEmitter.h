#pragma once
#include "ParticleManager.h"

using namespace std;         //  ���O��Ԏw��
/// <summary>
/// �p�[�e�B�N���G�~�b�^�[
/// </summary>
class ParticleEmitter
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private: //�V���O���g����
	//�R���X�g���N�^���B��
	ParticleEmitter() = default;
	//�f�X�g���N�^���B��
	~ParticleEmitter() = default;
public: //�����o�֐�
	static ParticleEmitter* GetInstance();

	//������
	void Initialize();

	//�X�V
	void Update();

	void IntroDraw();
	//�`��
	void FlontDrawAll();

	void DeathDrawAll();
	//�ǂ̃p�[�e�B�N���̕`��
	void BackDrawAll();
	//�G�t�F�N�g
	void FireEffect(const int life,const XMFLOAT3& pos,const float startscale, const float endscale,const XMFLOAT4& startcolor,const XMFLOAT4& endcolor);
	void DarkOtiEffect(const int life, const XMFLOAT3& l_pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//�R�g�R�G�t�F�N�g
	void KotokoEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//����_�̃G�t�F�N�g
	void LimitEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//�_�[�N�R�g�R�̂̃G�t�F�N�g
	void DarkEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//�����G�t�F�N�g
	void Explosion(const int life, const XMFLOAT3& pos, const float size,const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor,const int ExploType);
	//�_���[�W�G�t�F�N�g
	void Break(const int life, const XMFLOAT3& pos,const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor,const float Gravity,float divi);
	//���񂾎��̃G�t�F�N�g
	void DeathEffect(int life, XMFLOAT3 l_pos, float startscale, float endscale, XMFLOAT4 startcolor, XMFLOAT4 endcolor, float velocity);
	//�q�[���G�t�F�N�g
	void HealEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//�J�����{�XUltimate�G�t�F�N�g
	void CameraEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//�Z���N�g�G�t�F�N�g
	void SelectEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//�폜
	void AllDelete();
	//�����@�{�X
	void DeathEffectBoss(const int life, const XMFLOAT3& pos, const float size, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//�����@�{�X
	void ExproEffectBoss(const int life, const XMFLOAT3& pos, const float size, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);

private: //�����o�֐�
	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void LoadTexture();

private: //�����o�ϐ�
	//�p�[�e�B�N���}�l�[�W���[(��)
	unique_ptr<ParticleManager> circleParticle;
	//�p�[�e�B�N���}�l�[�W���[(��)
	unique_ptr<ParticleManager> darkcircleParticle;
	//�p�[�e�B�N���}�l�[�W���[(��)
	unique_ptr<ParticleManager> kotokoParticle;
	//�p�[�e�B�N���}�l�[�W���[(�_�[�N�R�g�R)
	unique_ptr<ParticleManager> darkParticle;
	//�p�[�e�B�N���}�l�[�W���[(��)
	unique_ptr<ParticleManager> wallParticle;
	//�p�[�e�B�N���}�l�[�W���[(���S)
	unique_ptr<ParticleManager> deathParticle;
	//�p�[�e�B�N���}�l�[�W���[(�q�[��)
	unique_ptr<ParticleManager> healParticle;
	//�p�[�e�B�N���}�l�[�W���[(�{�X�f�X)
	unique_ptr<ParticleManager>BossDeadParticle;
	//�p�[�e�B�N���}�l�[�W���[(cameraBOss)
	unique_ptr<ParticleManager> PhotoParticle;

private:
	int m_Timer = 0;
};
