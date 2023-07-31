#pragma once

#include "IKESprite.h"
#include <Windows.h>
#include <string>

class DebugText
{
public:
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	static const int maxCharCount = 256; // �ő啶����
	static const int fontWidth = 9; // �t�H���g�摜��1�������̉���
	static const int fontHeight = 18; // �t�H���g�摜��1�������̏c��
	static const int fontLineCount = 14; // �t�H���g�摜��1�s���̕�����

	DebugText();
	~DebugText();
public: // �ÓI�����o�֐�
	static DebugText* GetInstance();

	void Initialize(UINT texnumber);

	void Print(const std::string& text, float x, float y, float size);

	void DrawAll();

private:
	// �X�v���C�g�f�[�^�̔z��
	std::unique_ptr<IKESprite> spriteDatas[maxCharCount] = {};
	// �X�v���C�g�f�[�^�z��̓Y�����ԍ�
	int spriteIndex = 0;
};
