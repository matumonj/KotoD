#pragma once
#include "BaseActor.h"
#include "IKESprite.h"
#include "JsonLoader.h"
#include <map>
#include <array>
/// �^�C�g���V�[��
class LoadSceneActor : public BaseActor {
public:
	/// ������
	void Initialize(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// �I��
	void Finalize() override;
	/// ���t���[���X�V
	void Update(DirectXCommon* dxCommon, DebugCamera* camera, LightGroup* lightgroup) override;
	/// �`��
	void Draw(DirectXCommon* dxCommon) override;
	void BackDraw(DirectXCommon* dxCommon);
	void ImGuiDraw(DirectXCommon* dxCommon);
	void SpriteDraw();
private:
	void IntroUpdate(DebugCamera* camera)override;
	void MainUpdate(DebugCamera* camera)override;
	void FinishUpdate(DebugCamera* camera)override;


private://�����o�ϐ�
	int m_LoadTimer = 0;
	const int LoadTimerMax = 60;
	//�㉺�^���̒��S
	const XMFLOAT2 CenterPos = { 130.f,630.0f-32.f };
	//�����x
	const float AddMovingVal = 4.0f;
	//�����̊Ԋu
	const float WordsInter = 100.f;
	//�c���̊Ԋu
	const float space = 30.0f;

	string str = "SELECT";

	bool feedf=true;
	//Loading
	enum {
		text_L = 0,
		text_O,
		text_A,
		text_D,
		text_I,
		Sutopon_1,
		Sutopon_2,
		Sutopon_3,
		Sutopon_4,
		BackScreen,
		SpriteMax
	};

	int sutoponNow = Sutopon_1;
	int sutoponTimer = 0;

	std::array<std::unique_ptr<IKESprite>, SpriteMax> m_Sprites = {};
	std::array<XMFLOAT2, SpriteMax> m_SpritesPos = {};
	std::array<float, SpriteMax> m_SpritesAngle = {};
	std::array<XMFLOAT2, SpriteMax> m_StopPos = {};
};