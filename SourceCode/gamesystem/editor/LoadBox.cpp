#include "LoadBox.h"
#include "CsvLoader.h"
#include "ModelManager.h"

void LoadBox::Load()
{
	auto Size= static_cast<int>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/stage/stage1.csv", "Quantity")));

	boxes.resize(Size);
	
	/**/
	Pos.resize(Size);
	Rot.resize(Size);
	Scl.resize(Size);
	/**/

	Name.resize(Size);


	//èâä˙âª
	for (auto i = 0; i < boxes.size(); i++) {
		boxes[i].reset(new IKEObject3d());
		boxes[i]->Initialize();
	}


	LoadCSV::LoadCsvParam_XMFLOAT3("Resources/csv/stage/stage1.csv", Pos, "POSITION");
	LoadCSV::LoadCsvParam_XMFLOAT3("Resources/csv/stage/stage1.csv", Rot, "ROTATION");
	LoadCSV::LoadCsvParam_XMFLOAT3("Resources/csv/stage/stage1.csv", Scl, "SCALE");

	//
	LoadCSV::LoadCsvParam_String("Resources/csv/stage/stage1.csv", Name, "Model");

	//ÉÇÉfÉãïœçXÇ…ëŒâû
	for (auto i = 0; i < boxes.size(); i++) {
		if(Name[i]=="Box")
		{
			boxes[i]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Bullet));
		}
		else if(Name[i]=="Cube")
		{
			boxes[i]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Bullet));
		}

		boxes[i]->SetPosition(Pos[i]);
		boxes[i]->SetRotation(Rot[i]);
		boxes[i]->SetScale(Scl[i]);
	}
}

void LoadBox::Initialize()
{
	Load();
}

void LoadBox::FirstUpdate()
{
	for(auto i=0;i<boxes.size();i++)
	{

		boxes[i]->Update();
	}
}


void LoadBox::SecondUpdate()
{
	for (auto i = 0; i < boxes.size(); i++)
	{

		boxes[i]->Update();
	}
}

void LoadBox::Draw(DirectXCommon* dxCommon)
{//tes
	for (auto i = 0; i < boxes.size(); i++)
	{
		boxes[i]->Draw();
	}
	
}