#pragma once
#include"IKEObject3d.h"
#include<DirectXMath.h>
#include "DirectXCommon.h"
#include<memory>

using namespace DirectX;

class LoadManager
{
public:

protected:
	std::vector<XMFLOAT3>Pos;
	std::vector<XMFLOAT3>Rot;
	std::vector<XMFLOAT3>Scl;

public:
	/*‰Šú‰»*/
	virtual void Initialize() = 0;

	/*•`‰æ*/
	virtual void Draw(DirectXCommon* dxCommon)=0;


};

