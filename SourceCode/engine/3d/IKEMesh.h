#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "IKEMaterial.h"
#include <vector>
#include <unordered_map>

// �`��f�[�^
class IKEMesh
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X	
	// ���_�f�[�^�\���́i�e�N�X�`������j
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz���W
		XMFLOAT3 normal; // �@���x�N�g��
		XMFLOAT2 uv; // uv���W
	};

public: // �ÓI�����o�֐�
	// �ÓI������
	static void StaticInitialize(ID3D12Device* device);

private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;
public: // �����o�֐�
	// ���O���擾
	const std::string& GetName() { return name; }
	// ���O���Z�b�g
	void SetName(const std::string& name);
	// ���_�f�[�^�̒ǉ�
	void AddVertex(const VertexPosNormalUv& vertex);
	// ���_�C���f�b�N�X�̒ǉ�
	// <param name="index">�C���f�b�N�X</param>
	void AddIndex(unsigned short index);
	// ���_�f�[�^�̐����擾
	// <returns>���_�f�[�^�̐�</returns>
	size_t GetVertexCount() { return vertices.size(); }
	//�G�b�W�������f�[�^�̒ǉ�
	void AddSmoothData(const unsigned short& indexPosition, const unsigned short& indexVertex);
	//���������ꂽ���_�@���̌v�Z
	void CalculateSmoothedVertexNormals();
	// �}�e���A���̎擾
	IKEMaterial* GetMaterial() { return material; }
	// �}�e���A���̊��蓖��
	void SetMaterial(IKEMaterial* material);
	// �o�b�t�@�̐���
	void CreateBuffers();
	// ���_�o�b�t�@�擾
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView; }
	// �C���f�b�N�X�o�b�t�@�擾
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView; }
	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	/// </summary>
/// <returns>���_�z��</returns>
	const std::vector<VertexPosNormalUv>& GetVertices() { return vertices; }

	/// <summary>
	/// �C���f�b�N�X�z����擾
	/// </summary>
	/// <returns>�C���f�b�N�X�z��</returns>
	const std::vector<unsigned short>& GetIndices() { return indices; }


private: // �����o�ϐ�
	// ���O
	std::string name;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	// ���_�@���X���[�W���O�p�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;
	// �}�e���A��
	IKEMaterial* material = nullptr;
};
