#include "DirectXCommon.h"
#include <vector>
#include <cassert>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;

void DirectXCommon::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	dxgiFactory.Reset();
	cmdList.Reset();
	cmdAllocator.Reset();
	cmdQueue.Reset();
	swapchain.Reset();
	backBuffers.clear();
	depthBuffer.Reset();
	rtvHeaps.Reset();
	dsvHeap.Reset();
	fence.Reset();
	fence.Reset();
	imguiHeap.Reset();

}

void DirectXCommon::Initialize(WinApp* winApp) {
	// nullptr�`�F�b�N
	assert(winApp);

	this->winApp = winApp;

	//DXGI�f�o�C�X������
	if (!InitializeDevice()) {
		assert(0);
	}

	//�R�}���h�֘A������
	if (!InitializeCommand()) {
		assert(0);
	}

	//�X���b�v�`�F�[���̐���
	if (!CreateSwapChain()) {
		assert(0);
	}

	//�����_�[�^�[�Q�b�g����
	if (!InitializeRenderTargetView()) {
		assert(0);
	}

	//�[�x�o�b�t�@����
	if (!InitializeDepthBuffer()) {
		assert(0);
	}

	//�t�F���X����
	if (!CreateFence()) {
		assert(0);
	}

	// imgui������
	if (!InitImgui()) {
		assert(0);
	}
}

void DirectXCommon::PreDraw() {

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// �o�b�N�o�b�t�@�̔ԍ����擾�i2�Ȃ̂�0�Ԃ�1�ԁj
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	//�\����Ԃ���`���ԂɕύX
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	// �����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// �����_�[�^�[�Q�b�g���Z�b�g
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	// �S��ʃN���A
	ClearRenderTarget();
	// �[�x�o�b�t�@�N���A
	ClearDepthBuffer();

	if (FullScreen) {
		cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, normalwindow_width, normalwindow_height));
		cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, normalwindow_width, normalwindow_height));
	}
	else {
		cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, smallwindow_width, smallwindow_height));
		cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, smallwindow_width, smallwindow_height));
	}
}
void DirectXCommon::ClearDepthBuffer() {
	// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());
	// �[�x�o�b�t�@�̃N���A
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
void DirectXCommon::PostDraw() {
	HRESULT result = S_FALSE;
	// imgui�`��
	ImGui::Render();
	ID3D12DescriptorHeap* ppHeaps[] = { imguiHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());
	//#pragma regin �O���t�B�b�N�X�R�}���h
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();
	//���\�[�X�o���A��߂�
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	cmdList->Close();

	ID3D12CommandList* cmdLists[] = { cmdList.Get() };
	cmdQueue->ExecuteCommandLists(1, cmdLists);
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	cmdAllocator->Reset();
	cmdList->Reset(cmdAllocator.Get(), nullptr);
	swapchain->Present(1, 0);
}

bool DirectXCommon::InitializeDevice() {
	HRESULT result = S_FALSE;
#ifdef _Debug
	ComPtr<ID3D12Debug1> debugController;
	//�f�o�b�O���C���[���I����	
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif
	// �Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	// DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	// �A�_�v�^�[�̗񋓗p
	std::vector<ComPtr<IDXGIAdapter1 >> adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr <IDXGIAdapter1> tmpAdapter;
	for (int i = 0;
		dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters.push_back(tmpAdapter); // ���I�z��ɒǉ�����
	}

	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);  // �A�_�v�^�[�̏����擾

		// �\�t�g�E�F�A�f�o�C�X�����
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		std::wstring strDesc = adesc.Description;   // �A�_�v�^�[��
		// Intel UHD Graphics�i�I���{�[�h�O���t�B�b�N�j�����
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i];   // �̗p
			break;
		}
	}

	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		// �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
	return true;
}

bool DirectXCommon::CreateSwapChain() {
	HRESULT result = S_FALSE;
	//�X���b�v�`�F�[���̐���
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = WinApp::window_width;
	swapchainDesc.Height = WinApp::window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain1> swapchain1;
	HWND hwnd = winApp->GetHwnd();
	dxgiFactory->CreateSwapChainForHwnd(cmdQueue.Get(), winApp->GetHwnd(), &swapchainDesc, nullptr, nullptr, &swapchain1);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	swapchain1.As(&swapchain);

	return true;
}

bool DirectXCommon::InitializeCommand() {
	HRESULT result = S_FALSE;

	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�R�}���h���X�g�̐���
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	//�R�}���h�L���[�̐���
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::InitializeRenderTargetView() {
	HRESULT result = S_FALSE;

	//�ŃX�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = 2;
	dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	//�����_�[�^�[�Q�b�gview�̐���
	backBuffers.resize(2);
	for (int i = 0; i < 2; i++) {
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (FAILED(result)) {
			assert(0);
			return result;
		}

		//�f�B�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
			i, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		//D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		//handle.ptr += i * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		dev->CreateRenderTargetView(backBuffers[i].Get(), nullptr, handle);
	}

	return true;
}

//�[�x�o�b�t�@
bool DirectXCommon::InitializeDepthBuffer() {
	HRESULT result = S_FALSE;

	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT, WinApp::window_width, WinApp::window_height, 1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	//���\�[�X����
	result = dev->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&depthResDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0), IID_PPV_ARGS(&depthBuffer));

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�[�xview�p�ŃX�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(depthBuffer.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

bool DirectXCommon::CreateFence() {
	HRESULT result = S_FALSE;
	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	if (FAILED(result)) {
		assert(0);
		return false;
	}
	return true;
}

bool DirectXCommon::InitImgui()
{
	HRESULT result = S_FALSE;

	// �f�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&imguiHeap));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �X���b�v�`�F�[���̏����擾
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	if (ImGui::CreateContext() == nullptr) {
		assert(0);
		return false;
	}
	if (!ImGui_ImplWin32_Init(winApp->GetHwnd())) {
		assert(0);
		return false;
	}
	if (!ImGui_ImplDX12_Init(
		dev.Get(),
		swcDesc.BufferCount,
		swcDesc.BufferDesc.Format,
		imguiHeap.Get(),
		imguiHeap->GetCPUDescriptorHandleForHeapStart(),
		imguiHeap->GetGPUDescriptorHandleForHeapStart()
	)) {
		assert(0);
		return false;
	}

	return true;
}
//
//void DirectXCommon::Reset() {
//	//ID3D12DebugDevice* debugInterface;
//
//	//if (SUCCEEDED(dev->QueryInterface(&debugInterface))) {
//	//	debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
//	//	debugInterface->Release();
//	//}
//}


void DirectXCommon::ClearRenderTarget()
{
	// �o�b�N�o�b�t�@�̔ԍ����擾�i2�Ȃ̂�0�Ԃ�1�ԁj
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();
	// �����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	// �R�D��ʃN���A           R     G     B    A
	float clearColor[] = { 0.0f,0.0f, 0.0f,1.0f }; // ���ۂ��F
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
}


void DirectXCommon::WindowImGuiDraw() {
	//ImGui::Begin("SelectScreen");
	//ImGui::SetWindowPos(ImVec2(1000, 0));
	//ImGui::SetWindowSize(ImVec2(280, 150));
	//if (ImGui::RadioButton("Full", &FullScreen)) {
	//	FullScreen = true;
	//}
	//if (ImGui::RadioButton("NotFull", &FullScreen)) {
	//	FullScreen = false;
	//}
	///*if (!FullScreen) {
	//	ImGui::SliderFloat("window.x", &m_This_Like_window_x, 1280, 0);
	//	ImGui::SliderFloat("window.y", &m_This_Like_window_y, 720, 0);
	//}*/
	//ImGui::Unindent();
	//ImGui::End();

	if (FullScreen) {
		m_This_Like_window_x = 1280;
		m_This_Like_window_y = 720;
	}
	else {
		m_This_Like_window_x = 1000;
		m_This_Like_window_y = 500;

	}
	/*ImGui::Begin("SelectScreen");
	ImGui::SetWindowPos(ImVec2(500, 300));
	ImGui::SetWindowSize(ImVec2(200, 200));

	ImGui::Unindent();
	ImGui::End();*/
}

ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeapForSproteFont()
{
	ComPtr<ID3D12DescriptorHeap> ret;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));

	return ret;
}


