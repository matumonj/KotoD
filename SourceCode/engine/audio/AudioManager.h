#pragma once

//�I�[�f�B�I�̓ǂݍ��݃N���X
class AudioManager {
public:
	static AudioManager* GetInstance();

	void StartLoadAudio();
	void SecondLoadAudio();
};