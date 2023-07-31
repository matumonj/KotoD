#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "IKEMesh.h"

// ���f��
class IKEModel
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private:
	static const std::string baseDirectory;
private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;

public: // �ÓI�����o�֐�
	// �ÓI������
	static void StaticInitialize(ID3D12Device* device);
	// OBJ�t�@�C�����烁�b�V������
	static IKEModel* LoadFromOBJ(const std::string& modelname, bool smoothing = false);

public: // �����o�֐�
	// �f�X�g���N�^
	~IKEModel();
	// ������
	void Initialize(const std::string& modelname, bool smoothing);
	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	/// <summary>
/// ���b�V���R���e�i���擾
/// </summary>
/// <returns>���b�V���R���e�i</returns>
	const std::vector<IKEMesh*>& GetMeshes() { return meshes; }


private: // �����o�ϐ�
	// ���O
	std::string name;
	// ���b�V���R���e�i
	std::vector<IKEMesh*> meshes;
	// �}�e���A���R���e�i
	std::unordered_map<std::string, IKEMaterial*> materials;
	// �f�t�H���g�}�e���A��
	IKEMaterial* defaultMaterial = nullptr;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;

private: // �����o�֐�
	//���f���ǂݍ���
	void LoadModel(const std::string& modelname, bool smoothing);
	// �}�e���A���ǂݍ���
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	// �}�e���A���o�^
	void AddMaterial(IKEMaterial* material);
	// �f�X�N���v�^�q�[�v�̐���
	void CreateDescriptorHeap();
	// �e�N�X�`���ǂݍ���
	void LoadTextures();
};
