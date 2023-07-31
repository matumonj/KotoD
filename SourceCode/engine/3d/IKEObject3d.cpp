#include "IKEObject3d.h"

#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <fstream>
#include<sstream>
#include<string>
#include<vector>
#include "BaseCollider.h"
#include "CollisionAttribute.h"
#include "CollisionManager.h"
#include "SphereCollider.h"
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

/// �ÓI�����o�ϐ��̎���
ID3D12Device* IKEObject3d::device = nullptr;
ID3D12GraphicsCommandList* IKEObject3d::cmdList = nullptr;
IKEObject3d::PipelineSet IKEObject3d::pipelineSet;
XMMATRIX IKEObject3d::matView{};
XMMATRIX IKEObject3d::matProjection{};
XMFLOAT3 IKEObject3d::eye = { 0, 3.0f, -10.0f };
XMFLOAT3 IKEObject3d::target = { 0, 0, 0 };
XMFLOAT3 IKEObject3d::up = { 0, 1, 0 };
XMMATRIX IKEObject3d::matBillboard = XMMatrixIdentity();
XMMATRIX IKEObject3d::matBillboardY = XMMatrixIdentity();
Camera* IKEObject3d::camera = nullptr;
LightGroup* IKEObject3d::lightGroup = nullptr;


bool IKEObject3d::StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, int window_width,
	int window_height, Camera* camera)
{
	// nullptr�`�F�b�N
	assert(device);

	IKEObject3d::device = device;
	IKEObject3d::cmdList = cmdList;
	IKEObject3d::camera = camera;
	// �O���t�B�b�N�p�C�v���C���̐���
	CreateGraphicsPipeline();

	// ���f���̐ÓI������
	IKEModel::StaticInitialize(device);

	// �J����������
	//InitializeCamera(window_width, window_height);

	return true;
}


void IKEObject3d::PreDraw()
{
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	//assert(IKEObject3d::cmdList == nullptr);

	// �R�}���h���X�g���Z�b�g
	//IKEObject3d::cmdList = cmdList;

	//// �p�C�v���C���X�e�[�g�̐ݒ�
	//cmdList->SetPipelineState(pipelinestate.Get());
	//// ���[�g�V�O�l�`���̐ݒ�
	//cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void IKEObject3d::PostDraw()
{
	// �R�}���h���X�g������
	//IKEObject3d::cmdList = nullptr;
}

IKEObject3d* IKEObject3d::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	auto object3d = new IKEObject3d();
	if (object3d == nullptr)
	{
		return nullptr;
	}

	// ������
	if (!object3d->Initialize())
	{
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}

void IKEObject3d::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		//L"Resources/shaders/BasicVertexShader.hlsl",	// �V�F�[�_�t�@�C����
		L"Resources/shaders/BasicVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(static_cast<char*>(errorBlob->GetBufferPointer()),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		//L"Resources/shaders/BasicPixelShader.hlsl",	// �V�F�[�_�t�@�C����
		L"Resources/shaders/BasicPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	if (FAILED(result))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(static_cast<char*>(errorBlob->GetBufferPointer()),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			// xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			// �@���x�N�g��(1�s�ŏ������ق������₷��)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			// uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 2; // �`��Ώۂ�2��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R11G11B10_FLOAT; // 0�`255�w���RGBA
	gpipeline.RTVFormats[1] = DXGI_FORMAT_R11G11B10_FLOAT; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	//CD3DX12_ROOT_PARAMETER rootparams[2];
	//rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	CD3DX12_ROOT_PARAMETER rootparams[4];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	auto samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob,
		&errorBlob);
	// ���[�g�V�O�l�`���̐���
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&pipelineSet.rootsignature));
	if (FAILED(result))
	{
		assert(0);
	}

	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

	if (FAILED(result))
	{
		assert(0);
	}
}


void IKEObject3d::UpdateViewMatrix()
{
	// �r���[�s��̍X�V
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

IKEObject3d::~IKEObject3d()
{
	if (collider)
	{
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
}

bool IKEObject3d::Initialize()
{
	// nullptr�`�F�b�N
	assert(device);

	HRESULT result;
	//// �萔�o�b�t�@�̐���
	//result = device->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&constBuff));

	// �萔�o�b�t�@�̐���B0
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	//�N���X���̕�������擾
	name = typeid(*this).name();

	return true;
}

void IKEObject3d::Update()
{
	assert(camera);

	HRESULT result;
	//�s��̍X�V
	UpdateWorldMatrix();

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();
	offset.y += addoffset;
	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	constMap->offset = offset;
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constMap->color = color;
	constMap->uvval = uv;
	constBuffB0->Unmap(0, nullptr);
	//�����蔻��X�V
	if (collider)
	{
		collider->Update();
	}
}


void IKEObject3d::FollowUpdate(XMMATRIX matworld)
{
	assert(camera);

	HRESULT result;


	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

	//�s��̍X�V
	UpdateWorldMatrix();
	offset.y += addoffset;
	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	constMap->offset = offset;
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld * matworld;
	constMap->cameraPos = cameraPos;
	constMap->color = color;
	constMap->uvval = uv;
	constBuffB0->Unmap(0, nullptr);
	//�����蔻��X�V
	if (collider)
	{
		collider->Update();
	}
}

//�A�t�B���ϊ��p
void IKEObject3d::AffineUpdate()
{
	assert(camera);

	HRESULT result;
	//�s��̍X�V
	UpdateWorldMatrix();

	if (Affine)
	{
		matWorld *= matrix;
	}

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	constMap->offset = offset;
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constMap->color = color;
	constMap->uvval = uv;
	constBuffB0->Unmap(0, nullptr);
	//�����蔻��X�V
	if (collider)
	{
		collider->Update();
	}
}


void IKEObject3d::Draw()
{
	// nullptr�`�F�b�N
	assert(device);
	assert(IKEObject3d::cmdList);

	// ���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
	if (model == nullptr)
	{
		return;
	}

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelineSet.pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature.Get());
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	// ���C�g�̕`��
	lightGroup->Draw(cmdList, 3);
	// ���f���`��
	model->Draw(cmdList);
}

void IKEObject3d::UpdateWorldMatrix()
{
	assert(camera);

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	if (isBillboard && camera)
	{
		const XMMATRIX& matBillboard = camera->GetBillboardMatrix();

		matWorld = XMMatrixIdentity();
		matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
		matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
		matWorld *= matBillboard;
		matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f
	} else
	{
		matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
		matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
		matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
		matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f
	}

	// �e�I�u�W�F�N�g�������
	if (parent != nullptr)
	{
		// �e�I�u�W�F�N�g�̃��[���h�s����|����
		matWorld *= parent->matWorld;
	}
}


void IKEObject3d::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	//�R���W�����}�l�[�W���[�ɓo�^
	CollisionManager::GetInstance()->AddCollider(collider);
	UpdateWorldMatrix();

	collider->Update();
}

void IKEObject3d::CollisionField()
{
	// ���[���h�s��X�V
	this->UpdateWorldMatrix();

	// ��������
	if (!onGround)
	{
		// �����������x
		const float fallAcc = -0.01f;
		const float fallVYMin = -0.8f;
		// ����
		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);
		// �ړ�
		position.y += fallV.m128_f32[1];
	}

	// ���[���h�s��X�V
	this->UpdateWorldMatrix();
	this->collider->Update();

	auto sphereCollider = dynamic_cast<SphereCollider*>(this->collider);
	//assert(sphereCollider);


	// �N�G���[�R�[���o�b�N�N���X
	class PlayerQueryCallback : public QueryCallback
	{
	public:
		PlayerQueryCallback(Sphere* sphere) : sphere(sphere)
		{
		};

		// �Փˎ��R�[���o�b�N�֐�
		bool OnQueryHit(const QueryHit& info) override
		{
			const XMVECTOR up = { 0, 1, 0, 0 };

			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			// �n�ʔ��肵�����l
			const float threshold = cosf(XMConvertToRadians(30.0f));

			if (-threshold < cos && cos < threshold)
			{
				sphere->center += info.reject;
				move += info.reject;
			}

			return true;
		}

		Sphere* sphere = nullptr;
		XMVECTOR move = {};
	};

	PlayerQueryCallback callback(sphereCollider);

	// ���ƒn�`�̌�����S����
	CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);
	// �����ɂ��r�˕�������
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];
	// ���[���h�s��X�V
	this->UpdateWorldMatrix();
	this->collider->Update();

	// ���̏�[���狅�̉��[�܂ł̃��C�L���X�g
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius() + 3.f;
	ray.dir = { 0, -1, 0, 0 };
	RaycastHit raycastHit;

	// �ڒn���
	if (onGround)
	{
		// �X���[�Y�ɍ������ׂ̋z������
		const float adsDistance = 1.2f;
		// �ڒn���ێ�
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * 2.5f + adsDistance))
		{
			onGround = true;
			position.y -= (raycastHit.m_Distance - sphereCollider->GetRadius() * 3.0f);
		}
		// �n�ʂ��Ȃ��̂ŗ���
		else
		{
			onGround = false;
			fallV = {};
		}
	}
	// �������

	else if (fallV.m128_f32[1] <= 0.0f)
	{
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit,
			sphereCollider->GetRadius() * 3.0f))
		{
			// ���n
			onGround = true;
			position.y -= (raycastHit.m_Distance - sphereCollider->GetRadius() * 3.0f);
		}
	}


	this->Update();
}
