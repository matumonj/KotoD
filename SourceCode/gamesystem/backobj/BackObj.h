#pragma once
#include "ObjCommon.h"
#include <map>
#include"IKETexture.h"
struct JsonData;
//wiΜβΘΗ€ΚwiΜNX
class BackObj :
	public ObjCommon {
protected:
	// DirectX::πΘͺ
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	static BackObj* GetInstance();
	//JsonΗέέ
	void LoadMap();
	bool Initialize() override;//ϊ»
	/// <summary>
	/// t[
	/// </summary>
	void Update() override;

	/// <summary>
	/// `ζ
	/// </summary>
	void Draw(DirectXCommon* dxCommon) override;

private:
	//Jsonp
	JsonData* jsonData = nullptr;
	std::map<std::string, IKEModel*> models;
	std::vector<IKEObject3d*> objects;
	std::unique_ptr<IKETexture>ground;
	XMFLOAT3 m_ParticlePos = {};
};
