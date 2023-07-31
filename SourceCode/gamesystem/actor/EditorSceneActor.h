#pragma once
#include "BaseActor.h"
#include "OpenBrowser.h"
#include"PlaceManager.h"
#include"PlaceBox.h"
class LoadBox;

/// �^�C�g���V�[��
class EditorSceneActor : public BaseActor {
public:
	/// ������
	void Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// �I��
	void Finalize() override;
	/// ���t���[���X�V
	void Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// �`��
	void Draw(DirectXCommon* dxCommon) override;
	void FrontDraw(DirectXCommon* dxCommon);
	void BackDraw(DirectXCommon* dxCommon);
	void ImGuiDraw(DirectXCommon* dxCommon);
	void CollisionField();
private:
	bool item[3] = { false,false,false };
	LoadBox* load;
	PlaceMap* load2;

	unique_ptr<PlaceBox>placeObj;
	unique_ptr<IKEObject3d>ground;
	unique_ptr<OpenBrowser> openbrowser;
};