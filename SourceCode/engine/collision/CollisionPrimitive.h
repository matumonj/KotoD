#pragma once

#include <DirectXMath.h>

using namespace DirectX;
//��
struct Sphere
{
	//���S���W
	XMVECTOR center = { 0,0,0,1 };
	//���a
	float radius = 1.0f;
};

struct Box //��
{
	//���S���W
	DirectX::XMVECTOR center = { 0,0,0,1 };
	//���a
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
};

//����
struct Plane
{
	//�@���x�N�g��
	XMVECTOR normal = { 0,1,0,0 };
	//���_(0,0,0)����̋���
	float m_Distance = 0.0f;
};

//�@���t���O�p�`
class Triangle
{
public:
	//���_���W�@3��
	XMVECTOR p0;
	XMVECTOR p1;
	XMVECTOR p2;

	//�@���x�N�g��
	XMVECTOR normal;

	//�@���̌v�Z	
	void ComputeNormal();
};

//���C
class Ray
{
public:
	//�n�_���W
	XMVECTOR start = { 0,0,0,1 };
	//����
	XMVECTOR dir = { 1,0,0,0 };
};
struct Point
{
	float x;
	float y;
};

struct Line2D
{
	DirectX::XMFLOAT2 start;
	DirectX::XMFLOAT2 end;
};

//OBB
class OBB
{
private:
	XMVECTOR m_Pos;//���W
	XMVECTOR m_NormalDirect[3];//�����x�N�g��	
	float m_length[3];//�e�������̒���

public:
	DirectX::XMVECTOR GetDirect(int elem) { return m_NormalDirect[elem]; } // �w�莲�ԍ��̕����x�N�g�����擾
	float GetLen_W(int elem) { return m_length[elem]; } // �w�莲�����̒������擾
	DirectX::XMVECTOR GetPos_W() { return m_Pos; }//�ʒu���擾


	void SetParam_Pos(XMMATRIX posMat);
	void SetParam_Pos(XMFLOAT3 pos);
	void SetParam_Scl(XMFLOAT3 scl);
	void SetParam_Rot(XMMATRIX rotMat);
};

