#pragma once
#include "IKESprite.h"
#include<memory>
#include <array>
using namespace std;         //  名前空間指定
//クリアのテキスト
class ClearText {
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
public:
	static ClearText* GetInstance();
public:
	void LoadResource();

	void Initialize();

	void Update();

	void Draw();

	void ImGuiDraw();

public:
	void SetAlive(const bool Alive) { m_Alive = Alive; }
private:
	float m_Frame = {};
	XMFLOAT2 m_Size = {};
	bool m_Alive = false;
	bool m_Sound = false;

	unique_ptr<IKESprite> ClearSprite;
};