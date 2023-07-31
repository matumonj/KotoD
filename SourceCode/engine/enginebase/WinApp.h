#pragma once
#include <windows.h>
#include "imgui.h"

//�E�B���h�E�Y�A�v���P�[�V����
class WinApp
{
	//�萔
public:
	static const int window_width = 1280;
	static const int window_height = 720;
	// �ėp�@�\
	//WinApp* win = nullptr;
	//�|�C���^�u����

public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public:
	//������
	void Initialize();
	//�X�V
	void Update();

	//�I��
	void Finalize();

	//���b�Z�[�W�̏���
	bool ProcessMessage();
	HWND GetHwnd() { return hwnd; }
	HINSTANCE GetHInstance() { return w.hInstance; }
private:
	HWND hwnd = nullptr;
	WNDCLASSEX w{};
};