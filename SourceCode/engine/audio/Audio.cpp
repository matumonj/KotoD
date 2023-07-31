#include "Audio.h"
#include <fstream>
#include <cassert>

#pragma comment(lib,"xaudio2.lib")

Audio* Audio::GetInstance()
{
	static Audio instance;

	return &instance;
}
bool Audio::Initialize() {
	HRESULT result;

	//�C���X�^���X�𐶐�
	result = XAudio2Create(&xAudio2, XAUDIO2_DEFAULT_PROCESSOR);
	if FAILED(result) {
		assert(0);
		return false;
	}

	// �}�X�^�[�{�C�X�𐶐�
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	if FAILED(result) {
		assert(0);
		return false;
	}

	return true;
}

void Audio::LoadSound(int texnumber, const char* filename) 
{
	HRESULT result;
	//�t�@�C���X�g���[��
	std::ifstream file;
	file.open(filename, std::ios_base::binary);
	//�t�@�C���I�[�v�����s
	if (file.fail()) {
		assert(0);
	}
	strcpy_s(fileStamp[texnumber], filename);
	//RIFF�w�b�_�[�̓ǂݍ���
	file.read((char*)&riff[texnumber], sizeof(riff[texnumber]));;
	//�t�@�C����RIFF���`�F�b�N
	if (strncmp(riff[texnumber].chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}


	//Format�`�����N�̓ǂݍ���
	file.read((char*)&format[texnumber], sizeof(format[texnumber]));
	//Data�`�����N�̓ǂݍ���
	file.read((char*)&data[texnumber], sizeof(data[texnumber]));

	//Data�`�����N�̔g�`�f�[�^�̓ǂݍ���
	pBuffer[texnumber] = new char[data[texnumber].size];
	file.read(pBuffer[texnumber], data[texnumber].size);
	//Wave�t�@�C�������
	file.close();

	//�g�`�t�H�[�}�b�g�̐ݒ�
	memcpy(&wfex[texnumber], &format[texnumber].fmt, sizeof(format[texnumber].fmt));
	wfex[texnumber].wBitsPerSample = format[texnumber].fmt.nBlockAlign * 8 / format[texnumber].fmt.nChannels;


	// �g�`�t�H�[�}�b�g������SourceVoice�̐���
	result = xAudio2->CreateSourceVoice(&pSourceVoice[texnumber], &wfex[texnumber], 0, 2.0f, &voiceCallback);
	if FAILED(result) {
		delete[] pBuffer[texnumber];
		assert(0);
		return;
	}
}

void Audio::PlayWave(const char* filename, float Volume) {
	HRESULT result;
	// �t�@�C���X�g���[��
	std::ifstream file;
	// Wave�t�@�C�����J��
	file.open(filename, std::ios_base::binary);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	// RIFF�w�b�_�[�̓ǂݍ���
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// �t�@�C����RIFF���`�F�b�N
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}

	// Format�`�����N�̓ǂݍ���
	FormatChunk format;
	file.read((char*)&format, sizeof(format));

	// Data�`�����N�̓ǂݍ���
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	// Data�`�����N�̃f�[�^���i�g�`�f�[�^�j�̓ǂݍ���
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Wave�t�@�C�������
	file.close();

	WAVEFORMATEX wfex{};
	// �g�`�t�H�[�}�b�g�̐ݒ�
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;

	// �g�`�t�H�[�}�b�g������SourceVoice�̐���
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex, 0, 2.0f, &voiceCallback);
	if FAILED(result) {
		delete[] pBuffer;
		assert(0);
		return;
	}

	// �Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)pBuffer;
	buf.pContext = pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = data.size;
	pSourceVoice->SetVolume(Volume);
	// �g�`�f�[�^�̍Đ�
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	if FAILED(result) {
		delete[] pBuffer;
		assert(0);
		return;
	}
	XAUDIO2_VOICE_STATE xaudio2state;
	pSourceVoice->GetState(&xaudio2state);
	result = pSourceVoice->Start(0);
	if FAILED(result) {
		delete[] pBuffer;
		assert(0);
		return;
	}
}

void Audio::LoopWave(int texnumber, float Volume) {
	HRESULT result;

	// �Đ�����g�`�f�[�^�̐ݒ�
	buf[texnumber].pAudioData = (BYTE*)pBuffer[texnumber];
	buf[texnumber].pContext = pBuffer[texnumber];
	buf[texnumber].Flags = XAUDIO2_END_OF_STREAM;
	buf[texnumber].LoopCount = XAUDIO2_LOOP_INFINITE;
	buf[texnumber].AudioBytes = data[texnumber].size;
	pSourceVoice[texnumber]->SetVolume(Volume);
	// �g�`�f�[�^�̍Đ�
	result = pSourceVoice[texnumber]->SubmitSourceBuffer(&buf[texnumber]);
	if FAILED(result) {
		delete[] pBuffer[texnumber];
		assert(0);
		return;
	}

	result = pSourceVoice[texnumber]->Start(0);
	if FAILED(result) {
		delete[] pBuffer[texnumber];
		assert(0);
		return;
	}
}

void Audio::StopWave(int texnumber) {
	if (pSourceVoice[texnumber] == nullptr) {
		return;
	}
	//�f�[�^�̒�~
	XAUDIO2_VOICE_STATE xa2state;
	pSourceVoice[texnumber]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0) {
		pSourceVoice[texnumber]->Stop(0);
		pSourceVoice[texnumber]->DestroyVoice();
		LoadSound(texnumber, fileStamp[texnumber]);
	}
}

void Audio::VolumChange(int texnumber, float Volume) {
	pSourceVoice[texnumber]->SetVolume(Volume);
}