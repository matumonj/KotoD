#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

// Json�f�[�^
struct JsonData {

	struct ObjectData {
		// �t�@�C����
		std::string fileName;
		// ���X�|�[����
		std::string ResPornName;
		// �M�~�b�N��
		std::string GimmickName;
		// ���s�ړ�
		DirectX::XMVECTOR translation;
		// ��]�p
		DirectX::XMVECTOR rotation;
		// �X�P�[�����O
		DirectX::XMVECTOR scaling;
	};

	// �I�u�W�F�N�g�z��
	std::vector<ObjectData> objects;
};

//Json�ǂݍ���
class JsonLoader {
private:	//�G�C���A�X
//std::���ȗ�
	using string = std::string;

public:
	//�p�X
	static const string kDefaultBaseDirectory;
	//��L�̃e�N�X�`����
	static const string filename;
	//�e�N�X�`�����Ȃ��ꍇ�̕W���e�N�X�`���t�@�C����
	static const string kExtension;
public:
	static JsonData* LoadFile(const std::string& fileName);
};