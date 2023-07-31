#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include "IKEModel.h"
#include "Camera.h"
#include "LightGroup.h"

#include "CollisionInfo.h"

class BaseCollider;

/// 3D�I�u�W�F�N�g
class IKEObject3d
{
protected: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X	

	// �p�C�v���C���Z�b�g
	struct PipelineSet
	{
		// ���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootsignature;
		// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
		ComPtr<ID3D12PipelineState> pipelinestate;
	};

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		XMFLOAT2 offset;//�I�t�Z�b�g�l
		XMMATRIX viewproj;    // �r���[�v���W�F�N�V�����s��
		XMMATRIX world; // ���[���h�s��
		XMFLOAT3 cameraPos; // �J�������W�i���[���h���W�j
		float pad;//�p�f�B���O
		XMFLOAT4 color;//�F���
		float uvval;
	};

private: // �萔


public: //�ÓI�����o�֐�

	//�ÓI������
	static bool StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, int window_width, int window_height, Camera* camera = nullptr);


	//�O���t�B�b�N�p�C�v���C���̐���
	static void CreateGraphicsPipeline();


	//�`��O����
	static void PreDraw();
	//�`��㏈��
	static void PostDraw();
	//3D�I�u�W�F�N�g����
	static IKEObject3d* Create();

	static void SetCamera(Camera* camera) {
		IKEObject3d::camera = camera;
	}

	static void SetLightGroup(LightGroup* lightGroup) {
		IKEObject3d::lightGroup = lightGroup;
	}
	//�x�N�g���ɂ��ړ�
	static void CameraMoveVector(const XMFLOAT3& move);


	// �x�N�g���ɂ�鎋�_�ړ�
	static void CameraMoveEyeVector(const XMFLOAT3& move);

private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	// �e�N�X�`������p�p�C�v���C��
	static PipelineSet pipelineSet;
	// �r���[�s��
	static XMMATRIX matView;
	// �ˉe�s��
	static XMMATRIX matProjection;
	// ���_���W
	static XMFLOAT3 eye;
	// �����_���W
	static XMFLOAT3 target;
	// ������x�N�g��
	static XMFLOAT3 up;
	// �r���{�[�h�s��
	static XMMATRIX matBillboard;
	// Y�����r���{�[�h�s��
	static XMMATRIX matBillboardY;
	// �J����
	static Camera* camera;
	// ���C�g
	static LightGroup* lightGroup;


private:// �ÓI�����o�֐�	
	//�J����������
	static void InitializeCamera(int window_width, int window_height);


	//�r���[�s����X�V
	static void UpdateViewMatrix();

public: // �����o�֐�

	IKEObject3d() = default;

	virtual ~IKEObject3d();

	virtual	bool Initialize();

	//���t���[������
	virtual void Update();

	virtual void AffineUpdate();

	//�Ǐ]��������̂̍X�V
	void FollowUpdate(XMMATRIX matworld);
	// �`��
	virtual void Draw();
	//�s��̍X�V
	void UpdateWorldMatrix();


	//���W�̎擾
	const XMFLOAT3& GetPosition() { return position; }

	//��]�̎擾
	const XMFLOAT3& GetRotation() { return rotation; }

	//�g��k���̎擾
	const XMFLOAT3& GetScale() { return scale; }
	const XMMATRIX& GetMatWorld() { return matWorld; }
	XMMATRIX GetMatrot() { return matRot; }
	XMMATRIX GetMatScl() { return matScale; }

	/// <summary>
/// ���f�����擾
/// </summary>
	inline IKEModel* GetModel() { return model; }

	//���W�̐ݒ�
	void SetPosition(const XMFLOAT3& position) { this->position = position; }

	void SetUvScrollY(float uvs) { uv = uvs; }

	void SetRotation(const XMFLOAT3& rotation) { this->rotation = rotation; }
	//�X�P�[���̐ݒ�
	void SetScale(const XMFLOAT3& scale) { this->scale = scale; }
	//�F�̐ݒ�
	void SetColor(const XMFLOAT4& color) { this->color = color; }
	//�F��ω������邩�ǂ���
	void SetChangeColor(bool ChangeColor) { this->ChangeColor = ChangeColor; }
	void SetFog(bool Fog) { this->Fog = Fog; }
	void SetLightEffect(bool LightEffect) { this->LightEffect = LightEffect; }
	void SetAddColor(const XMFLOAT4& Addcolor) { this->Addcolor = Addcolor; }
	//�I�t�Z�b�g�̐ݒ�
	void SetAddOffset(float addoffset) { this->addoffset = addoffset; }
	//�^�C�����O�̐ݒ�
	void SetTiling(float Tiling) { this->Tiling = Tiling; }
	//�f�B�]���u�̐ݒ�
	void SetDisolve(float Disolve) { this->Disolve = Disolve; }
	//�f�B�]���u�̐ݒ�
	void SetLightPower(float lightPower) { this->lightPower = lightPower; }
	//���f���̃Z�b�g
	void SetModel(IKEModel* model) { this->model = model; }

	void SetBillboard(bool isBillboard) { this->isBillboard = isBillboard; }

	//�����蔻��Z�b�g
	void SetCollider(BaseCollider* collider);

	void CollisionField();

	//�R�[���o�b�N
	virtual void OnCollision(const CollisionInfo& info) {}


	//�A�t�B���ϊ��s�����Z����
	void AddMatrix(XMMATRIX matrix)
	{
		Affine = true;
		this->matrix = matrix;
	}

	//���[���h�s��擾
	const XMMATRIX GetMatrix()
	{
		XMFLOAT3 l_scale = {};
		l_scale.x = 1 / scale.x;
		l_scale.y = 1 / scale.y;
		l_scale.z = 1 / scale.z;

		XMMATRIX l_mat = matWorld;
		l_mat.r[0].m128_f32[0] *= l_scale.x;
		l_mat.r[0].m128_f32[1] *= l_scale.x;
		l_mat.r[0].m128_f32[2] *= l_scale.x;
		l_mat.r[1].m128_f32[0] *= l_scale.y;
		l_mat.r[1].m128_f32[1] *= l_scale.y;
		l_mat.r[1].m128_f32[2] *= l_scale.y;
		l_mat.r[2].m128_f32[0] *= l_scale.z;
		l_mat.r[2].m128_f32[1] *= l_scale.z;
		l_mat.r[2].m128_f32[2] *= l_scale.z;
		return l_mat;
	}

	//���[���h�s��擾
	const XMMATRIX GetMatrix2(XMMATRIX matWorld2)
	{
		XMFLOAT3 l_scale = {};
		l_scale.x = 1 / scale.x;
		l_scale.y = 1 / scale.y;
		l_scale.z = 1 / scale.z;

		XMMATRIX l_mat = matWorld * matWorld2;
		l_mat.r[0].m128_f32[0] *= l_scale.x;
		l_mat.r[0].m128_f32[1] *= l_scale.x;
		l_mat.r[0].m128_f32[2] *= l_scale.x;
		l_mat.r[1].m128_f32[0] *= l_scale.y;
		l_mat.r[1].m128_f32[1] *= l_scale.y;
		l_mat.r[1].m128_f32[2] *= l_scale.y;
		l_mat.r[2].m128_f32[0] *= l_scale.z;
		l_mat.r[2].m128_f32[1] *= l_scale.z;
		l_mat.r[2].m128_f32[2] *= l_scale.z;
		return l_mat;
	}

protected: // �����o�ϐ�
	ComPtr<ID3D12Resource> constBuffB0; // �萔�o�b�t�@
	//�I�t�Z�b�g�l
	XMFLOAT2 offset = { 0.0f,0.0f };
	float addoffset = 0.0f;
	float Tiling = 1.0f;
	float Disolve = -1.5f;
	bool ChangeColor = false;//�F�ύX
	bool Fog = false;//�t�H�O
	bool LightEffect = true;//���C�g�̉e�����󂯂邩
	float lightPower = 15.0f;
	// �F
	XMFLOAT4 color = { 1,1,1,1 };
	//���Z����F
	XMFLOAT4 Addcolor = { 1,1,1,1 };
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	// ���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld;
	// �e�I�u�W�F�N�g
	IKEObject3d* parent = nullptr;
	// ���f��
	IKEModel* model = nullptr;
	// �r���{�[�h
	bool isBillboard = false;
	//�N���X��
	const char* name = nullptr;

	//�A�t�B���ϊ��p
	bool Affine = false;

	//�e�q�\���p
	DirectX::XMMATRIX matrix = {};
private:
	XMMATRIX matScale, matRot, matTrans;
	float uv;
public:
	//�R���C�_�[
	BaseCollider* collider = nullptr;
private:
	//�ڒn�t���O
	bool onGround = true;
	//�����x�N�g��
	DirectX::XMVECTOR fallV = {};
	float radius_adjustment = 0.0f;
	//FBX
	int FallGroundTime = 0;

private:
	std::string modelname;
public:
	void SetModeName(std::string name) { modelname = name; }
	std::string GetModelName() { return modelname; }
};

