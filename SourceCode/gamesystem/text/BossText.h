#pragma once
#include "TextManager.h"
#include "IKESprite.h"
using namespace std;         //  ���O��Ԏw��
enum TextState {
	TEXTANGER_START,
	TEXTANGER_SECOND,
	TEXTJOY_START,
	TEXTJOY_SECOND,
	TEXTJOY_THIRD,
	TEXTSELECT_EMO
};
//�G�̊Ǘ��n�N���X
class BossText {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	BossText();
	//������
	void Initialize(DirectXCommon* dxCommon);
	//�`��
	void SpriteDraw(DirectXCommon* dxCommon);
	//���b�Z�[�W���
	void Display();
	//���b�Z�[�W�I��
	void SelectText(TextManager::Name_Second name);
	void SelectText(TextManager::Name_Fourth name);
	void SelectText(TextManager::Name_Last name, const XMVECTOR& color = { 1.f,1.f,1.f,1.f });
	void SelectText(TextManager::Name_CameraBoss name, const XMVECTOR& color = { 1.f,1.f,1.f,1.f });
	void SelectText(TextManager::Name_First name,const XMVECTOR& color={1.f,1.f,1.f,1.f});
	void SelectText(TextManager::Name_Cap name, const XMVECTOR& color = { 1.f,1.f,1.f,1.f });
	void ChangeColor(int row, const XMVECTOR& color);
	void ChangeColor(const XMVECTOR& color);

public:
	void SetTextState(const int TextState) { m_TextState = TextState; }
	void SetWindowInvisible() { this->window_invisible_ = false; }
private:
	unique_ptr<TextManager> text_;
	unique_ptr<IKESprite> window_;

	//�J�����̏��
	int m_TextState = 0;

	//Windows�֌W
	XMFLOAT2 window_pos{ WinApp::window_width / 2.f,WinApp::window_height + 100 };
	XMFLOAT2 window_size{ 0.f,0.f };
	float m_Frame = 0.0f;
	bool window_invisible_ = true;
};