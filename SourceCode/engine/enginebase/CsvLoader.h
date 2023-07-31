#pragma once
#include <any>
#include <string>

#include<fstream>
#include<string>
#include<sstream>
#include <vector>

#include "DirectXMath.h"
using namespace DirectX;
namespace LoadCSV
{
	//CSV�ǂݍ���
	//�ԐړI�ɂ����@���̊֐����ŗ�O�����Ƃ�������
	void LoadCsvParam(std::string FileName,std::string LoadName,std::any&p );

	//���ړI�ɂ����@�R�[�h���Z�����ށE�i�[�p�̕ϐ����K�v�Ȃ�
	std::any LoadCsvParam(std::string FileName, std::string LoadName);

	void LoadCsvParam_XMFLOAT3(std::string FileName, std::vector<XMFLOAT3>&obj,std::string LoadName);
	void LoadCsvParam_String(std::string FileName, std::vector<std::string>& obj, std::string LoadName);
	void LoadCsvParam_Float(std::string FileName, std::vector<float>& obj, std::string LoadName);
	void LoadCsvParam_Int(std::string FileName, std::vector<int>& obj, std::string LoadName);
}


