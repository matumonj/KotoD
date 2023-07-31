#include "PlaceBox.h"

#include <atlstr.h>

#include "Collision.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "ImageManager.h"
#include "Input.h"
#include "ModelManager.h"
#include "SphereCollider.h"


void PlaceBox::Initialize()
{
	SampleObj = std::make_unique<IKEObject3d>();
	SampleObj->Initialize();
	SampleObj->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Bullet));


	float radius = 5.0f;
	SampleObj->SetCollider(new SphereCollider(XMVECTOR({ 10, radius, 10, 0 }), radius));
	SampleObj->collider->SetAttribute(COLLISION_ATTR_ALLIES);
}


void PlaceBox::Update()
{
	SampleObj->SetScale({ 5.0f,5.0f,5.0f });
	SampleObj->SetPosition(S_Pos);

	SampleObj->Update();

	SampleObj->CollisionField();
	SampleObj->SetColor({ 1.f,1.f,1.f,0.5f });

	for (auto i = 0; i < boxes.size(); i++)
	{
		if (imguilist[i] == nullptr)continue;
		if (boxes[i] == nullptr)continue;

		boxes[i]->SetColor({ 1.f,1.f,1.f,1.f });
		boxes[i]->SetPosition(imguilist[i]->GetPos());
		boxes[i]->SetRotation(imguilist[i]->GetRot());
		boxes[i]->SetScale({ 5.0f,5.0f,5.0f });
		boxes[i]->Update();
		boxes[i]->CollisionField();

	}

	for (int i = 0; i < imguilist.size(); i++)
	{
		if (imguilist[i] == nullptr)
			continue;
		if (i != 0) {
			if (imguilist[i]->GetImguiNumber() - imguilist[i - 1]->GetImguiNumber() > 1)
			{
				imguilist[i]->SetImguiNumber(imguilist[i - 1]->GetImguiNumber() + 1);
			}
		}

		/**/
		if (imguilist[i]->GetBoxnumber() == CreateImGui::WOOD && BoxsList[i] != Boxs::WOOD)
		{
			boxes[i]->SetModeName("Wood");
			boxes[i]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Bullet));
			BoxsList[i] = Boxs::WOOD;
		}

		if (imguilist[i]->GetBoxnumber() == CreateImGui::STONE && BoxsList[i] != Boxs::CUBE)
		{
			boxes[i]->SetModeName("Cube");
			boxes[i]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Bullet));
			BoxsList[i] = Boxs::CUBE;
		}

		if (imguilist[i]->GetDelF())
		{
			boxes.erase(boxes.begin() + i);
			imguilist.erase(imguilist.begin() + i);// erase(std::cbegin(enemys) + i);
		}
	}


	if (ArgmentFlag) {
		BoxsList.push_back(Boxs::WOOD);
		ArgmentObj(ArgmentFlag, "Box", ModelManager::GetInstance()->GetModel(ModelManager::Bullet));
	}

	if (StoneArgment) {
		BoxsList.push_back(Boxs::CUBE);
		ArgmentObj(StoneArgment, "Cube", ModelManager::GetInstance()->GetModel(ModelManager::Bullet));
	}

}

void PlaceBox::Draw(DirectXCommon* DxCommon)
{
	SampleObj->Draw();

	for (auto i = 0; i < boxes.size(); i++)
	{
		boxes[i]->Draw();
	}
}

void PlaceBox::FileWriting()
{
	if (!LoadFlag)return;

	file.open("stage1.csv");
	popcom << file.rdbuf();

	file.close();
	//std::ofstream pofs("EnemyParam_CSV/position.csv");
	std::ofstream ofs("Resources/csv/stage/stage1.csv"); // ファイルパスを指定する
	ofs << "Quantity" << "," << boxes.size() << std::endl;

	for (int i = 0; i < boxes.size(); i++)
	{
		ofs << "*---------------------------*" << std::endl;
		//座標
		ofs << "POSITION" << "," << boxes[i]->GetPosition().x
			<< "," << boxes[i]->GetPosition().y
			<< "," << boxes[i]->GetPosition().z << std::endl;
		//回転
		ofs << "ROTATION" << "," << boxes[i]->GetRotation().x
			<< "," << boxes[i]->GetRotation().y
			<< "," << boxes[i]->GetRotation().z << std::endl;
		//スケール
		ofs << "SCALE" << "," << boxes[i]->GetScale().x
			<< "," << boxes[i]->GetScale().y
			<< "," << boxes[i]->GetScale().z << std::endl;
		ofs << "ModelName" << "," << boxes[i]->GetModelName() << std::endl;
		ofs << "*---------------------------*" << std::endl;
	}
}
#include<stdio.h>
void PlaceBox::ImGui_Draw()
{


	ImGui::Begin("BoxParam");

	ImGui::SetWindowPos(ImVec2(0, 400));
	ImGui::SetWindowSize(ImVec2(300, 300));
	if (ImGui::Button("WoodBox", ImVec2(50, 50)))
	{
		ArgmentFlag = true;
	}
	ImGui::SameLine();

	if (ImGui::Button("StoneBox", ImVec2(50, 50)))
	{
		StoneArgment = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("OtherBox", ImVec2(50, 50)))
	{
		OtherArgment = true;
	}
	ImGui::Text("Position");
	ImGui::SliderFloat("Position_X", &S_Pos.x, -200.f, 200.f);
	ImGui::SliderFloat("Position_Y", &S_Pos.y, -20.f, 20.f);
	ImGui::SliderFloat("Position_Z", &S_Pos.z, -200.f, 200.f);

	ImGui::Text("Rotation");
	ImGui::SliderFloat("Rotation_X", &S_Rot.x, 0.f, 360.f);
	ImGui::SliderFloat("Rotation_Y", &S_Rot.y, 0.f, 360.f);
	ImGui::SliderFloat("Rotation_Z", &S_Rot.z, 0.f, 360.f);

	ImGui::Text("Scale");
	ImGui::SliderFloat("Sclae_X", &S_Scl.x, 0.f, 10.f);
	ImGui::SliderFloat("Scale_Y", &S_Scl.y, 0.f, 10.f);
	ImGui::SliderFloat("Scale_Z", &S_Scl.z, 0.f, 10.f);


	/*CSV開く*/
	ImGui::Text("OpenCSV");
	if(ImGui::Button("BoxCSV", ImVec2(100, 50)))
	{
		CString default = _T("TD4\\Resources\\csv\\stage\\stage1");

		ShellExecute(nullptr, _T("open"), default ,nullptr, nullptr, SW_SHOW);
	}
	ImGui::End();

	//配置されたBOXに対応付けれるImguiのリスト(要素ごとの編集が可能)
	ImGui::Begin("BoxList");
	ImGui::SetWindowPos(ImVec2(300, 400));
	ImGui::SetWindowSize(ImVec2(300, 300));
	for (int i = 0; i < imguilist.size(); i++)
	{
		if (imguilist[i] == nullptr)
		{
			continue;
		}

		imguilist[i]->CreateImguiList();
	}

	ImGui::End();
}



PlaceBox::CreateImGui::CreateImGui(std::string num, float scl, XMFLOAT3 pos)
{
	Scl = scl;
	Pos = pos;
	listnum.push_back("Box" + num);
}
void PlaceBox::CreateImGui::CreateImguiList()
{
	std::string TitName = modelname + std::to_string(imnumber) + "----------------------";
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), TitName.c_str());
	//ImGui::StyleColorsClassic();

	if (ImGui::CollapsingHeader(TitName.c_str()))
	{
		/*ImGui::Text("Scale");
		std::string sclname = "Scl" + TitName;
		ImGui::SliderFloat(sclname.c_str(), &Scl, 0.f, 1.f);*/

		ImGui::Text("Position");

		float pos[3] = { Pos.x, Pos.y, Pos.z };
		std::string posname_x = "Pos.x" + TitName;
		std::string posname_z = "Pos.z" + TitName;
		ImGui::SliderFloat(posname_x.c_str(), &Pos.x, -200.f, 200.f);
		ImGui::SliderFloat(posname_z.c_str(), &Pos.z, -200.f, 200.f);

		/*回転*/
		ImGui::Text("Rotation");
		std::string rotname = "Rot" + TitName;
		ImGui::SliderFloat(rotname.c_str(), &Rot.y, 0.f, 360.f);

		/*モデル変更*/
		ImGui::Text("SelectBox");
		std::string enumynum_g = "WoodBox" + TitName;
		std::string enumynum_l = "StoneBox" + TitName;
		std::string enumynum_t = "Other" + TitName;

		if (ImGui::Button(enumynum_g.c_str(), ImVec2(70, 30)))
		{
			//
			bnumber = WOOD;
		}
		ImGui::SameLine();
		if (ImGui::Button(enumynum_l.c_str(), ImVec2(70, 30)))
		{
			//
			bnumber = STONE;
		}
		ImGui::SameLine();

		ImGui::Text("DeleteInstance");
		std::string delname = "Delete" + TitName;
		if (ImGui::Button(delname.c_str(), ImVec2(100, 30)))
		{
			Del = true;
		}

	//	 s=TitName;
	//std::string  nowname = "NowModel" + TitName;
	// std::string inputname="now" + TitName;
	////strcat_s(input,sizeof input, inputname.c_str());
	//strncpy_s( buf, s.c_str(), sizeof(buf)-1 );
	//ImGui::InputText(inputname.c_str(), buf, sizeof(buf));
	//s = buf;
	}	
	//ImGui::InputText(nowname.c_str(), input, 256);
}


void PlaceBox::ArgmentObj(bool& aflag, std::string mname, IKEModel* model)
{

	SampleObj->SetModel(model);

	std::unique_ptr<IKEObject3d> newobj;
	newobj = std::make_unique<IKEObject3d>();
	newobj->Initialize();
	newobj->SetModel(model);
	newobj->SetModeName(mname);
	newobj->SetPosition(SampleObj->GetPosition());


	float radius = 5.0f;
	newobj->SetCollider(new SphereCollider(XMVECTOR({ 10, radius, 10, 0 }), radius));
	newobj->collider->SetAttribute(COLLISION_ATTR_ALLIES);
	boxes.push_back(std::move(newobj));

	std::unique_ptr<CreateImGui> newlist;
	newlist = std::make_unique<CreateImGui>(mname + std::to_string(static_cast<int>(boxes.size())), 0.02f, S_Pos);
	newlist->SetModelName(mname);
	newlist->SetImguiNumber(static_cast<int>(boxes.size() - 1));

	imguilist.push_back(std::move(newlist));

	aflag = false;


}


void PlaceMap::Initialize()
{
	OldSize = 10;
	MapStage.resize(10);
	for(int i=0;i<MapStage.size();i++)
	{
		MapStage[i].resize(10);

		for(int j=0;j<MapStage[i].size();j++)
		{
			MapStage[i][j]=IKESprite::Create(ImageManager::GAMEOVER, { 0.f,0.f });
			MapStage[i][j]->SetAnchorPoint({ 0.50f,0.50f });
			MapStage[i][j]->SetSize({ 50.f,50.f });

			MapStage[i][j]->SetPosition({ i * 50.f,j * 50.f });
		}
	}

}

void PlaceMap::Update()
{
	int size = 25;
	CollideMap();
}

void PlaceMap::CollideMap()
{
	Input::GetInstance()->MouseMovePos(MousePoint);

	for(int i=0;i<MapStage.size();i++)
	{
		for(int j=0;j<MapStage[i].size();j++)
		{
			if (MapStage[i][j] == nullptr)continue;
			if(Input::GetInstance()->TriggerMouseLeft()&& Collision::CircleCollision(MousePoint.x,MousePoint.y,10.f,MapStage[i][j]->GetPosition().x, MapStage[i][j]->GetPosition().y,10))
			{
				MapStage[i][j]=IKESprite::Create(ChangeSprite(), { 0.f,0.f });
				MapStage[i][j]->SetAnchorPoint({ 0.50f,0.50f });
				MapStage[i][j]->SetSize({ 50.f,50.f });
				MapStage[i][j]->SetPosition({ i * 50.f,j * 50.f });
				MapStage[i][j]->SetColor({ 1.f,1.f,0.f,1.f });
			}
			else
			{
				MapStage[i][j]->SetColor({ 1.f,1.f,1.f,1.f });
			}
		}
	}
	
}

UINT PlaceMap::ChangeSprite()
{
	if (_icon == Icon::BOX)
		return ImageManager::GAMEOVER;
	if (_icon == Icon::CUBE)
		return ImageManager::GAMEOVER;

	return ImageManager::PLAY;
}

void PlaceMap::Draw()
{
	for (int i = 0; i < MapStage.size(); i++)
	{
		for (int j = 0; j < MapStage[i].size(); j++)
		{
			if (MapStage[i][j] == nullptr)continue;
			MapStage[i][j]->Draw();
		}
	}
}

void PlaceMap::ImguiDraw()
{
	ImGui::Begin("2DEditor");

	ImGui::InputInt("SizeX", &size);
	if(ImGui::RadioButton("CheckSize",false))
	{
		
			MapStage.resize(5);
			for(int i=0;i<size;i++)
		{
			
		}
	//	if(OldSize<=size)
			for (int i = 0; i < MapStage.size(); i++)
			{MapStage[i].resize(size);
				for (int j = 0; j < MapStage[i].size(); j++)
				{
					if (MapStage[i][j] == nullptr)
					{
						MapStage[i][j]=IKESprite::Create(ImageManager::GAMEOVER, { 0.f,0.f });
						MapStage[i][j]->SetAnchorPoint({ 0.50f,0.50f });
						MapStage[i][j]->SetSize({ 50.f,50.f });

						MapStage[i][j]->SetPosition({ i * 50.f,j * 50.f });
					}
		}
		}
		
		OldSize = size;

	}
	if(ImGui::Button("Edit_Box",ImVec2(100,100)))
	{
		_icon = Icon::BOX;
	}
	if (ImGui::Button("Edit_Cube", ImVec2(100, 100)))
	{
		_icon = Icon::CUBE;
	}
	ImGui::End();
}
