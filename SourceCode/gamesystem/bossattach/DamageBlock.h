#pragma once
#include "ObjCommon.h"
#include "IKETexture.h"
enum DirState {
	DIR_UP,
	DIR_DOWN,
	DIR_RIGHT,
	DIR_LEFT,
};
//�΂̋ʃN���X
class DamageBlock :
	public ObjCommon {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	DamageBlock();
	bool Initialize() override;//������
	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(DirectXCommon* dxCommon) override;

	/// <summary>
	/// ImGui�`��
	/// </summary>
	void ImGuiDraw();

	bool Collide();	//�����蔻��

	void Move();

	void BirthParticle();
public:
	void InitPos(const int Num,const XMFLOAT3& Pos);
public:
	//gettersetter
	const bool& GetAlive() { return m_Alive; }
	void SetAttackDir(const int AttackDir) { m_AttackDir = AttackDir; }
private:
	unique_ptr<IKETexture> tex;//�G���A
	int m_AttackDir = {};
	float m_Frame = {};
	bool m_Alive = false;
	XMFLOAT3 m_AfterPos = {};
	float m_Damage = {};

	//�e�N�X�`���֘A
	XMFLOAT3 m_TexPos = {};
	XMFLOAT3 m_TexRot = {};
	XMFLOAT3 m_TexScale = {};
	XMFLOAT4 m_TexColor = { 1.0f,1.0f,1.0f,0.0f };
	float m_Alpha = 0.0f;
	float m_AfterAlpha = 0.5f;
private:
	enum BlockState {
		BLOCK_SET,
		BLOCK_PLAY,
	}_BlockState;
public:
	
};