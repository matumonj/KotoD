#pragma once

#include "IKEFBXModel.h"
#include "Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

class IKEFBXObject3d
{
protected: //�G�C���A�X
	//Microosoft::WRL::���ȗ�
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: //�ÓI�����o�֐�
	//setter
	static void SetDevice(ID3D12Device* device) { IKEFBXObject3d::device = device; }
	static void SetCamera(Camera* camera) { IKEFBXObject3d::camera = camera; }

	/// <summary>
	///�O���t�B�b�N�p�C�v���C���̐���
	/// </summary>
	static void CreateGraphicsPipeline();

private: //�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device;
	//�J����
	static Camera* camera;
	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate;

public: //�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\���́i���W�ϊ��s��j
	struct ConstBufferDataTransform
	{
		bool ShadeSet;
		XMMATRIX viewproj; //�r���[�v���W�F�N�V�����s��
		XMMATRIX world; //���[���h�s��
		XMFLOAT3 cameraPos; //�J�����s��i���[���h���W�j
		float pad; //�p�f�B���O
		XMFLOAT4 color; //�F���
	};

	// �A�j���[�V�����p�f�[�^�\����
	struct Animation
	{
		FbxAnimStack* stack;
		FbxTakeInfo* info;
	};

public: //�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// ���t���[��
	/// </summary>
	void Update(bool Loop, int Speed, bool& Stop);
	//�Ǐ]������悤�̍X�V
	void FollowUpdate(bool Loop, int Speed, bool& Stop);

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList"></param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="model">���f��</param>
	void SetModel(IKEFBXModel* model) { this->model = model; }

	//�A�j���[�V�����̃��[�h
	void LoadAnimation();
	/// <summary>
	/// �A�j���[�V����
	/// </summary>
	void PlayAnimation(int Number);

	void StopAnimation();

	XMMATRIX ExtractPositionMat(XMMATRIX matworld);
	XMMATRIX ExtractRotationMat(XMMATRIX matworld);
	XMMATRIX ExtractScaleMat(XMMATRIX matworld);

	void SetPosition(const XMFLOAT3& position) { this->position = position; }

	void SetRotation(const XMFLOAT3& rotation) { this->rotation = rotation; }

	void SetColor(const XMFLOAT4& color) { this->color = color; }
	//�f�B�]���u�̐ݒ�
	void SetDisolve(float Disolve) { this->Disolve = Disolve; }
	void SetAddColor(const XMFLOAT4& Addcolor) { this->Addcolor = Addcolor; }

	void SetChangeColor(bool ChangeColor) { this->ChangeColor = ChangeColor; }
	void SetShadeSet(bool ShadeSet) { this->ShadeSet = ShadeSet; }
	void SetReverse(bool Reverse) { this->m_Reverse = Reverse; }
	XMMATRIX GetWorldMat() { return WorldMat; }
	XMMATRIX GetMatrot() { return matRot; }
	XMMATRIX GetMatScl() { return matScale; }
	const int GetBoneNumber() { return BoneNumber; }
	const bool GetReverse() { return m_Reverse; }
	void GetBoneIndexMat(int index, XMMATRIX& matworld);

	void SetBoneNumber(int BoneNumber) { this->BoneNumber = BoneNumber; }
	/// <summary>
	/// �X�P�[���̐ݒ�
	/// </summary>
	/// <param name="position">�X�P�[��</param>
	void SetScale(const XMFLOAT3& scale) { this->scale = scale; }

	//FBX�^�C���̏I���l
	int GetFbxTime_End() { return endTime.GetSecondCount(); }
	//
	int GetFbxTime_Current() { return currentTime.GetSecondCount(); }
protected: //�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffTransform;
	//�f�B�]���u
	float Disolve = -1.0f;
	XMFLOAT4 Addcolor = {1, 1, 1, 1};
	//���[�J���X�P�[��
	XMFLOAT3 scale = {1, 1, 1};
	//X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = {0, 0, 0};
	//���[�J�����W
	XMFLOAT3 position = {0, 0, 0};
	//�F
	XMFLOAT4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	//���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld;
	//���f��
	IKEFBXModel* model = nullptr;
	size_t BoneSize;
	bool ChangeColor = false; //�F�ύX

	bool ShadeSet = true;
	//�萔�o�b�t�@�i�X�L���j
	ComPtr<ID3D12Resource> constBuffSkin;
	//1�t���[��
	FbxTime frameTime;
	//�A�j���[�V�����J�n����
	FbxTime startTime;
	//�A�j���[�V�����I������
	FbxTime endTime;
	//���ݎ��ԁi�A�j���[�V�����j
	FbxTime currentTime;
	//�A�j���[�V�����Đ���
	bool isPlay = false;
	//�A�j���[�V�����̃f�[�^
	std::vector<Animation> animationData;
	//�t�Đ����ǂ���
	bool m_Reverse = false;
public: //�萔
	//��ɒǏ]�֌W
	XMMATRIX matScale, matRot, matTrans;
	XMMATRIX WorldMat;
	FbxTime SetCurrent() { return currentTime = startTime; }
	bool GetIsPlay() { return isPlay; }
	FbxTime GetCurrent() { return currentTime; }
	size_t GetBoneSize() { return BoneSize; }
	FbxTime GetEndTime() { return endTime; }
	//�{�[���̍ő吔
	static const int MAX_BONES = 90;
	int BoneNumber = 0;
	int m_FBXTimer = 0;

	//�萔�o�b�t�@�p�f�[�^�\���́i�X�L�j���O�j
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};
};
