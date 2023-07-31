#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
//�I�[�f�B�I�̎��
enum AudioLoad {
	AUDIO_TITLE,
	AUDIO_LOAD,
	AUDIO_BATTLE,
	AUDIO_BATTLE2
};
//�I�[�f�B�I�̃R�[���o�b�N
class XAudio2VoiceCallBack : public IXAudio2VoiceCallback {
public:
	void OnVoiceProcessingPassStart(UINT32 BytesRequired) {};
	// �{�C�X�����p�X�̏I����
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
	//�o�b�t�@�X�g���[���̍Đ��I����
	STDMETHOD_(void, OnStreamEnd) (THIS) {};


	//�o�b�t�@�̎g�p��
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
	//�o�b�t�@�̖����ɒB�����Ƃ�
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {
		delete[] pBufferContext;//�o�b�t�@�J��
	};
	//���[�v�̒B�����Ƃ�
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
	//�{�C�X�̃G���[��
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
};

class Audio {
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	//�\���̂̒�`
	//�`�����N�w�b�_
	struct ChunkHeader
	{
		char id[4];//�`�����N����ID
		int32_t size;//�`�����N�T�C�Y
	};

	//RIFF�w�b�_�`�����N
	struct RiffHeader
	{
		ChunkHeader chunk;//RIFF
		char type[4];//WAVE
	};
	
	//FMT�`�����N
	struct FormatChunk
	{
		ChunkHeader chunk;//fmt
		WAVEFORMAT fmt;//�g�`�t�H�[�}�b�g
	};

public://�����o�֐�

	static Audio* GetInstance();
	//������
	bool Initialize();

	//�T�E���h�t�@�C���̓ǂݍ���
	void LoadSound(int texnumber, const char* filename);
	//�T�E���h�̍Đ�
	void PlayWave(const char* filename,float Volume);
	//�T�E���h�̃��[�v
	void LoopWave(int texnumber, float Volume);
	//�T�E���h�̒�~
	void StopWave(int texnumber);
	void VolumChange(int texnumber, float Volume);
private://�����o�ϐ�
	static const int SrvCount = 50;
	int texNumber = 0;
	RiffHeader riff[SrvCount] = {};
	FormatChunk format[SrvCount] = {};
	ChunkHeader data[SrvCount] = {};
	char* pBuffer[SrvCount] = {};
	WAVEFORMATEX wfex[SrvCount]{};
	IXAudio2SourceVoice* pSourceVoice[SrvCount] = { nullptr };
	XAUDIO2_BUFFER buf[SrvCount]{};
	char fileStamp[SrvCount][50]{};
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	 XAudio2VoiceCallBack voiceCallback;
};