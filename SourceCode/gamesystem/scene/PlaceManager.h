#pragma once
#include"IKEObject3d.h"
#include<DirectXMath.h>
#include<memory>

#include<fstream>
#include<string>
#include<sstream>
#include "DirectXCommon.h"
using namespace DirectX;
class PlaceManager
{
protected:
	std::unique_ptr<IKEObject3d>Obj;
	std::unique_ptr<IKEObject3d>SampleObj;
	std::vector<XMFLOAT3> Pos;
	std::vector < XMFLOAT3> Scl;
	std::vector < XMFLOAT3> Rot;

	XMFLOAT3 S_Pos;
	XMFLOAT3 S_Scl={2.f,2.f,2.f};
	XMFLOAT3 S_Rot;

public:
	virtual void Initialize()=0;
	//
	virtual void Update() = 0;
	//
	virtual void Draw(DirectXCommon* DxCommon) = 0;

	virtual void FileWriting() = 0;

	virtual void ImGui_Draw() = 0;

	virtual void ArgmentObj(bool &aflag, std::string mname, IKEModel*model)=0;

	void SetP(XMFLOAT3 pos) { S_Pos = pos; }

	void SetLoad(bool f) { LoadFlag = f; }
protected:
	int Quantity;
	std::vector<int> Number;
	std::ifstream file;
	std::stringstream popcom;
	//配置フラグ
	bool ArgmentFlag;
	//削除フラグ
	bool DeleteFlag;

	bool LoadFlag;
	void CollisionField();

	std::vector<std::unique_ptr<IKEObject3d>> boxes;
protected:

	class CreateImGui
	{
	public:
		CreateImGui(std::string num,float scl, XMFLOAT3 pos);
		void CreateImguiList();
	private:
		std::vector<std::string> listnum;

		float CreateScl;
		XMFLOAT3 CreatePos;
		XMFLOAT3 Pos;
		XMFLOAT3 Rot;
		float Scl;
		XMFLOAT4 Col;
		bool Del;
		bool Select;
		int imnumber;
		std::string modelname;
		std::string ChangeModelName; char buf[255]{};
		static std::string s;
	public:
		float GetScl() { return Scl; }
		XMFLOAT3 GetPos() { return Pos; }
		XMFLOAT3 GetRot() { return Rot; }
		bool GetDelF() { return Del; }
		void SetDelF(bool f) {  Del=f; }
		bool GetSelectThis() { return Select; }
		void SetModelName(std::string name) { modelname = name; }
		void SetImguiNumber(int index) { imnumber = index; }
		int GetImguiNumber() { return imnumber; }
		std::string GetChangeName() { return ChangeModelName; }
	public:
		enum BoxNumber
		{
			NON,
			WOOD,
			STONE,
		} bnumber;


		BoxNumber GetBoxnumber() { return bnumber; }
	};


	std::vector<std::unique_ptr<CreateImGui>> imguilist;
};

