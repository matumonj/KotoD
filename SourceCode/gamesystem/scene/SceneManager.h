#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include <string>
#include <memory>
#include <future>
//�V�[���Ǘ�
class SceneManager
{
public:
	static SceneManager* GetInstance();
	//�X�V
	void Update(DirectXCommon* dxCommon);
	/// �`��
	void Draw(DirectXCommon* dxCommon);
	//�J��
	void Finalize();

public:
	//���V�[���\��
	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

	// �񓯊����[�h
	void AsyncLoad();

public:
	//getter setter
	void SetLoad(bool Load) { this->m_Load = Load; }
	bool GetLoad() { return  m_Load; }

private:
	//���̃V�[��
	BaseScene* scene_ = nullptr;
	//���̃V�[��
	BaseScene* nextScene_ = nullptr;

	AbstractSceneFactory* sceneFactory_ = nullptr;

	~SceneManager() = default;
	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	void operator =(const SceneManager&) = delete;

	// �񓯊�����
	std::thread m_th = {};
	// ���[�h���
	int m_loadType = NoLoad;
	// ���[�h���Ă��邩
	bool m_Load = false;
	// �X���b�h�ԂŎg�p���鋤�L���\�[�X��r�����䂷��
	std::mutex isLoadedMutex = {};
	//���[�h�̃^�C�v
	enum LoadType
	{
		NoLoad,
		LoadStart,
		LoadEnd
	};
};