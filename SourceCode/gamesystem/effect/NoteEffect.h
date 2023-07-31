#pragma once
#include "IKESprite.h"
#include <array>
using namespace std;         //  名前空間指定
//混乱時のエフェクト
class NoteEffect {
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	NoteEffect();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(DirectXCommon* dxCommon);

	void ImGuiDraw();
private:
	//音符が流れるエフェクト
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
	//ノーツエフェクトの変数
	array<XMFLOAT2, NOTE_MAX> m_NotePos;
	array<XMFLOAT3, NOTE_MAX> m_NoteScale;
	array<XMFLOAT4, NOTE_MAX> m_NoteColor;
	//ノーツエフェクトの変数
	array<XMFLOAT2, NOTE_MAX> m_NotePos2;
	array<XMFLOAT3, NOTE_MAX> m_NoteScale2;
	array<XMFLOAT4, NOTE_MAX> m_NoteColor2;

	//光の状態
	enum EmiState {
		NOTE_SET,
		NOTE_MOVE
	}m_NoteState = NOTE_SET;

	int m_FlowTimer = {};
};