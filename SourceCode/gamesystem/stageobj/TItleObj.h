#pragma once
#include"IKEObject3d.h"
#include"IKEModel.h"

#include"DirectXCommon.h"
#include"ModelManager.h"
#include<memory>
#include <array>
using namespace std;         //  名前空間指定

//タイトルのOBJ
class TitleObj {
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
public:
	static TitleObj* GetInstance();
public:
	void LoadResource();
	void Initialize();

	void Update();

	void Draw(DirectXCommon* dxCommon);

	void ImGuiDraw();

private:
	void GhostUpdate();

private:
	static const int GHOST_NUM = 2;
	static const int TREE_NUM = 4;
private:
	IKEModel* ghostmodel = nullptr;
	IKEModel* treemodel = nullptr;
	IKEModel* foodmodel = nullptr;
	IKEModel* groundmodel = nullptr;
	IKEModel* homemodel = nullptr;

	array<unique_ptr<IKEObject3d>, GHOST_NUM> ghostobj;
	array<unique_ptr<IKEObject3d>, TREE_NUM> treeobj;
	unique_ptr<IKEObject3d> foodobj;
	unique_ptr<IKEObject3d> groundobj;
	unique_ptr<IKEObject3d> homeobj;
	unique_ptr<IKEObject3d>BackSkyDome = nullptr;

	array<XMFLOAT3, GHOST_NUM> m_GhostPos = {};
	array<XMFLOAT3, GHOST_NUM> m_GhostRot = {};

	XMFLOAT3 m_SinAngle = {};
	XMFLOAT3 m_SinAngle2 = {};

	float m_Frame = {};
	bool m_Rot = false;
	int m_RotTimer = {};
	float m_AfterRot = {};
	XMFLOAT3 m_DomeRot = {};
};