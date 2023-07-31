#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include<wrl.h>
#include <d3dx12.h>
#include<cstdlib>
#include <imgui.h>
#include "WinApp.h"

//DirectX�ėp
class DirectXCommon
{
	//�萔
public:
	static const int normalwindow_width = 1280;
	static const int normalwindow_height = 720;

	static const int smallwindow_width = 1000;
	static const int smallwindow_height = 500;
private:
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	void ClearDepthBuffer();
	void Finalize();
	//������
	void Initialize(WinApp* winApp);

	//�`��O����
	void PreDraw();

	//�`��㏈��
	void PostDraw();


	void ClearRenderTarget();

	//�f�o�C�X�̏�����
	bool InitializeDevice();


	//�X���b�v�`�F�[���̐���
	bool CreateSwapChain();


	//�R�}���h�֘A�̏�����
	bool InitializeCommand();

	//�����_�[�^�[�Q�b�g����
	bool InitializeRenderTargetView();

	//�[�x�o�b�t�@����
	bool InitializeDepthBuffer();

	//�t�F���X����
	bool CreateFence();
	//Imgui������
	bool InitImgui();

	void WindowImGuiDraw();

	ID3D12Device* GetDev() { return dev.Get(); }

	ID3D12CommandQueue* GetQue() { return cmdQueue.Get(); }
public:
	const bool& GetFullScreen() { return  FullScreen; }

	ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }

	void SetFullScreen(const bool& FullScreen) { this->FullScreen = FullScreen; }

	D3D12_VIEWPORT GetViewPort()const { return viewport; }
private:
	//�����o�ϐ�
		// Direct3D�֘A
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<IDXGISwapChain4> swapchain;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
	WinApp* winApp = nullptr;
	ComPtr<ID3D12DescriptorHeap> imguiHeap;
	float m_This_Like_window_x = 1000.0f;
	float m_This_Like_window_y = 500.0f;
	float m_This_Like_save_x = 1000.0f;
	float m_This_Like_save_y = 500.0f;
	bool FullScreen = false;

	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, smallwindow_width, smallwindow_height };
public:
	

	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeapForSproteFont();

};