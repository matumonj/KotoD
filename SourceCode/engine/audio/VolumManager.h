#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
//���ʒ����p�̃N���X
class VolumManager {
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	static VolumManager* GetInstance();
	//�����Update�ȊO�g��Ȃ�
	void Initialize();
	void Update();
	void Draw();
	void Finalize();
public:
	float GetSEVolum() { return   m_SEVolum; }
	float GetBGMVolum() { return   m_BGMVolum; }

	void SetSEVolum(float SEVolum) { this->m_SEVolum = SEVolum; }
	void SetBGMVolum(float BGMVolum) { this->m_BGMVolum = BGMVolum; }
private:
	//SE��BGM�̉���
	float m_SEVolum = 0.3f;
	float m_BGMVolum = 0.1f;
};