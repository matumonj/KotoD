#pragma once
#include "BaseActor.h"
class SelectSceneActor :
    public BaseActor
{
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
	void ColEnemy(std::vector<InterEnemy*> enelist);
private:
	float Rads;
	int textT;
	unique_ptr<IKESprite> backScreen_ = nullptr;

	Spline* spline;
	vector<XMFLOAT3> pointsList;

	enum class TextScene
	{
		NON,
		TIEYOSHI_EXP,
		STOPON_SPK,
		KILL_TIEYOSHI,
		LET_GO,
		ENDTEXT
	}_Tscne = TextScene::ENDTEXT;

	//�ۉe(�{�X)
	float BosscircleShadowDir[3] = { 0,-1,0 };
	float BosscircleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float BosscircleShadowFactorAngle[2] = { 0.0f, 2.0f };
};

