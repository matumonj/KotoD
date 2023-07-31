#pragma once
#include "IKESprite.h"
#include <array>
using namespace std;         //  ���O��Ԏw��
//�������̃G�t�F�N�g
class NoteEffect {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	NoteEffect();
	//������
	void Initialize();
	//�X�V
	void Update();
	//�`��
	void Draw(DirectXCommon* dxCommon);

	void ImGuiDraw();
private:
	//�����������G�t�F�N�g
	void FlowNote();
private:
	static const int NOTE_MAX = 15;
	static const int NOTE_MAX2 = 15;
public:
	//gettersetter
	bool GetAlive() { return m_Alive; }

	void SetAlive(const bool Alive) { m_Alive = Alive; }
private:
	array<unique_ptr<IKESprite>,NOTE_MAX> NoteSprite;
	array<unique_ptr<IKESprite>, NOTE_MAX> NoteSprite2;
	bool m_Alive = false;
	//�m�[�c�G�t�F�N�g�̕ϐ�
	array<XMFLOAT2, NOTE_MAX> m_NotePos;
	array<XMFLOAT3, NOTE_MAX> m_NoteScale;
	array<XMFLOAT4, NOTE_MAX> m_NoteColor;
	//�m�[�c�G�t�F�N�g�̕ϐ�
	array<XMFLOAT2, NOTE_MAX> m_NotePos2;
	array<XMFLOAT3, NOTE_MAX> m_NoteScale2;
	array<XMFLOAT4, NOTE_MAX> m_NoteColor2;

	//���̏��
	enum EmiState {
		NOTE_SET,
		NOTE_MOVE
	}m_NoteState = NOTE_SET;

	int m_FlowTimer = {};
};