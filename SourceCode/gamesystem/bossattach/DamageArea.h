#pragma once
#include "IKETexture.h"
#include "IKEObject3d.h"
#include "IKEModel.h"
#include <vector>
#include <any>
#include <memory>
using namespace DirectX;
using namespace std;         //  ���O��Ԏw��
//�_���[�W�G���A
class DamageArea {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	DamageArea(const int Num);
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw(DirectXCommon* dxCommon);
	//IMGUI
	void ImGuiDraw();

private:
	void PointUpdate();	//�_�̍X�V
	void LineUpdate();	//���̍X�V
	void StateManager();//��Ԃ��Ǘ�����
	bool Collide();//�����蔻��
private:
	IKEModel* model = nullptr;
	vector<IKEObject3d*> obj;//���f��
	vector<IKETexture*> tex;//�G���A

	//�_�ɕK�v�ȕϐ�
	vector<XMFLOAT3> m_Position;
	vector<XMFLOAT3> m_Rotation;
	vector<XMFLOAT3> m_Scale;
	vector<XMFLOAT4> m_Color;
	vector<bool> m_Alive;

	//�G���A�ɕK�v�ȕϐ�
	vector<XMFLOAT3> m_TexPosition;
	vector<XMFLOAT3> m_TexRotation;
	vector<XMFLOAT3> m_TexScale;
	vector<XMFLOAT4> m_TexColor;
	vector<bool> m_TexAlive;
	vector<int> m_Hit;
	enum AreaState {
		POINT_BIRTH,
		LINE_BIRTH,
		STAY,
		VANISH_AREA,
	};

	int m_AreaState = POINT_BIRTH;

	float m_Frame = {};
	float m_Alpha = {};
	float m_CommonScale = {};
	float m_AfterAlpha = 1.0f;
	float m_AfterScale = 1.3f;

	int m_StayTimer = 0;
	int m_VanishLimit = {};

	float m_Damage = {};
	XMFLOAT2 m_OffSet = { -0.01f,0.0f };
};