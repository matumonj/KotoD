#pragma once
#include<DirectXMath.h>
#include"IKEObject3d.h"
#include<memory>

#include<fstream>
#include<string>
#include<sstream>

#include "LoadManager.h"

using namespace DirectX;
class LoadBox:public LoadManager
{
private:
	
	std::vector<std::string> Name;

	std::vector<std::unique_ptr< IKEObject3d>>boxes;
	std::string line;
	std::stringstream popcom;
	std::ifstream file;

public:
	void Load();

	void Initialize()override;

	void FirstUpdate();

	void SecondUpdate();

	void Draw(DirectXCommon* dxCommon)override;

	IKEObject3d* GetBox(int i) { return boxes[i].get(); }
};

