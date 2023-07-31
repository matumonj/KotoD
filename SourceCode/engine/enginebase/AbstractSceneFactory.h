#pragma once
#include "BaseScene.h"
#include<string>

// �V�[���H��i�T�O�j
class AbstractSceneFactory {
public:
	virtual ~AbstractSceneFactory()=default;
	// �V�[������
	virtual BaseScene* CreateScene(const std::string& sceneName)=0;
};

