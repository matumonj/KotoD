#pragma once
#include "PlaceManager.h"

class PlaceBox:public PlaceManager
{
public:
	PlaceBox(){};
	~PlaceBox(){};
public:
	void Initialize() override;

	void Update() override;

	void Draw(DirectXCommon* DxCommon) override;

	void ImGui_Draw() override;

	void FileWriting() override;
	ImVec4 p;
	void ArgmentObj(bool& aflag, std::string mname, IKEModel* model) override;;
private:
	std::vector<std::unique_ptr<IKEObject3d>> boxes;

	bool StoneArgment;
	bool OtherArgment;

	std::vector<bool> SelThis;
public:
	enum class Boxs
	{
		WOOD,
		CUBE
	};
	std::vector<Boxs>BoxsList;
};

#include"IKESprite.h"
class PlaceMap
{
public:
	~PlaceMap();
	void Initialize();
	void Update();
	void Draw();
	void ImguiDraw();
private:
	enum class Icon
	{
		NON,
		BOX,
		CUBE
	}_icon;
	XMFLOAT2 MousePoint;
	bool Change;

	UINT ChangeSprite();
	void CollideMap();
private:
	std::vector<std::vector<std::unique_ptr<IKESprite>>>MapStage;
	int OldSize;
	int size = 15;
};