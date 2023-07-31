#include "JsonLoader.h"


#include <fstream>
#include <cassert>
#include "json.hpp"

const std::string JsonLoader::kDefaultBaseDirectory = "Resources/Json/";
const std::string JsonLoader::kExtension = ".json";
const std::string JsonLoader::filename = "Yura";

JsonData* JsonLoader::LoadFile(const std::string& fileName) {
	// �A�����ăt���p�X�𓾂�
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	// �t�@�C���X�g���[��
	std::ifstream file;

	// �t�@�C�����J��
	file.open(fullpath);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	// JSON�����񂩂�𓀂����f�[�^
	nlohmann::json deserialized;

	// ��
	file >> deserialized;

	// ���������x���f�[�^�t�@�C�����`�F�b�N
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"�𕶎���Ƃ��Ď擾
	std::string name = deserialized["name"].get<std::string>();
	// ���������x���f�[�^�t�@�C�����`�F�b�N
	assert(name.compare("scene") == 0);

	// ���x���f�[�^�i�[�p�C���X�^���X�𐶐�
	JsonData* jsondata = new JsonData();

	// "objects"�̑S�I�u�W�F�N�g�𑖍�
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		// ��ʂ��擾
		std::string type = object["type"].get<std::string>();

		// MESH
		if (type.compare("MESH") == 0) {
			// �v�f�ǉ�
			jsondata->objects.emplace_back(JsonData::ObjectData{});
			// ���ǉ������v�f�̎Q�Ƃ𓾂�
			JsonData::ObjectData& objectData = jsondata->objects.back();


			if (object.contains("file_name")) {
				// �t�@�C����
				objectData.fileName = object["file_name"];
			}

			if (object.contains("resporn_name")) {
				// �t�@�C����
				objectData.ResPornName = object["resporn_name"];
			}

			if (object.contains("gimmick_name")) {
				// �t�@�C����
				objectData.GimmickName = object["gimmick_name"];
			}

			// �g�����X�t�H�[���̃p�����[�^�ǂݍ���
			nlohmann::json& transform = object["transform"];
			// ���s�ړ�
			objectData.translation.m128_f32[0] = (float)transform["translation"][1];
			objectData.translation.m128_f32[1] = (float)transform["translation"][2];
			objectData.translation.m128_f32[2] = (float)-transform["translation"][0];
			objectData.translation.m128_f32[3] = 1.0f;
			// ��]�p
			objectData.rotation.m128_f32[0] = (float)-transform["rotation"][1];
			objectData.rotation.m128_f32[1] = (float)-transform["rotation"][2];
			objectData.rotation.m128_f32[2] = (float)transform["rotation"][0];
			objectData.rotation.m128_f32[3] = 0.0f;
			// �X�P�[�����O
			objectData.scaling.m128_f32[0] = (float)transform["scaling"][1];
			objectData.scaling.m128_f32[1] = (float)transform["scaling"][2];
			objectData.scaling.m128_f32[2] = (float)transform["scaling"][0];
			objectData.scaling.m128_f32[3] = 0.0f;
		}
		else if (type.compare("CAMERA") == 0) {
			// �v�f�ǉ�
			jsondata->objects.emplace_back(JsonData::ObjectData{});
			// ���ǉ������v�f�̎Q�Ƃ𓾂�
			JsonData::ObjectData& objectData = jsondata->objects.back();


			if (object.contains("file_name")) {
				// �t�@�C����
				objectData.fileName = object["file_name"];
			}

			if (object.contains("resporn_name")) {
				// �t�@�C����
				objectData.ResPornName = object["resporn_name"];
			}

			if (object.contains("gimmick_name")) {
				// �t�@�C����
				objectData.GimmickName = object["gimmick_name"];
			}

			// �g�����X�t�H�[���̃p�����[�^�ǂݍ���
			nlohmann::json& transform = object["transform"];
			// ���s�ړ�
			objectData.translation.m128_f32[0] = (float)transform["translation"][1];
			objectData.translation.m128_f32[1] = (float)transform["translation"][2];
			objectData.translation.m128_f32[2] = (float)-transform["translation"][0];
			objectData.translation.m128_f32[3] = 1.0f;
		}
		// TODO: �R���C�_�[�̃p�����[�^�ǂݍ���


	// TODO: �I�u�W�F�N�g�������ċA�֐��ɂ܂Ƃ߁A�ċA�ďo�Ŏ}�𑖍�����
		if (object.contains("children")) {

		}
	}

	return jsondata;
}
