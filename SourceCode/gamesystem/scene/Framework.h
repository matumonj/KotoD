#pragma once
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "Input.h"
#include "IKESprite.h"
#include "IKEObject3d.h"
#include "Audio.h"
#include "DebugText.h"
#include "WinApp.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#include "Camera.h"
#include "LightGroup.h"
#include "IKETexture.h"
#include "ParticleManager.h"
#include"FPSManager.h"
#include"ModelManager.h"
#include "ImageManager.h"
#include "ShadowMap.h"
//�t���[�����[�N
class Framework
{
public:

	//���s
	void Run();
	//������
	virtual void Initialize(DirectXCommon* dxCommon);

	//������
	virtual void Finalize();

	//���t���[������
	virtual void Update(DirectXCommon* dxCommon);

	////�`��
	virtual void Draw(DirectXCommon* dxCommon) = 0;

	bool GetEndRequst() { return endRequst; }
protected:
	DirectXCommon* dxcommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	DebugText* debugText = nullptr;
	bool endRequst = false;
	WinApp* winApp = nullptr;
	Camera* camera = nullptr;
	AbstractSceneFactory* sceneFactory_ = nullptr;
	FPSManager* fpsManager = nullptr;
	ShadowMap* shadowmap = nullptr;
};