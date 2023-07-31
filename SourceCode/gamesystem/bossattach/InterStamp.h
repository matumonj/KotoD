#pragma once
#include"IKETexture.h"
#include <memory>
#include<DirectXMath.h>
using namespace DirectX;
using namespace std;         //  ���O��Ԏw��
class InterStamp {
protected:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:

	//������
	virtual bool Initialize(const XMFLOAT3& pos) = 0;
	//�X�V
	void Update();
	
	//�`��
	void Draw(DirectXCommon* dxCommon);

	virtual void Action() = 0;//�{�X���L�̏���

	virtual void ImGui_Origin() = 0;//�{�X���ꂼ���ImGui

	virtual void Origin_Draw(DirectXCommon* dxCommon) = 0;//�{�X���ꂼ���ImGui

	void ImGuiDraw();//ImGui�̕`��

public:
	//gettersetter
	bool GetBirth() { return m_Birth; }

	void SetAfterScale(float AfterScale) { m_AfterScale = AfterScale; };
	
protected:
	unique_ptr<IKETexture> texture;
	bool m_Birth = false;//�o��
	bool m_Alive = false;//����
	int m_BirthTimer = 0;//�o������
	float m_Frame = 0.0f;
	XMFLOAT3 m_Position = { 0.0f,-200.0f,0.0f };
	XMFLOAT3 m_Rotation = { 90.0f,0.0f,0.0f };
	XMFLOAT3 m_Scale = { 0.0f,0.0f,0.0f };
	XMFLOAT4 m_Color = { 1.0f,1.0f,1.0f,0.0f };

	float m_AfterScale = {};

	float m_Radius = {};
};

