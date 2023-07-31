#pragma once
#include <string>
#include "IKEModel.h"
#include <map>
#include <IKEFBXModel.h>

class ModelManager final {
private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager& r) = default;
	ModelManager& operator= (const ModelManager& r) = default;

public:
	//OBJの名前
	enum ModelName {
		Ground,
		Bullet,
		Nucleus,
		GhostBullet,
		BulEnemy,
		Tyuta,
		Sutopon,
		Food,
		Ghost,
		Buddy,
		MobUsa,
		Camera,
		Heart,
		Pedestal,
		DJ,
		Skydome,
		KIDO_OBJ,
		CD,
		GLASS1,
		MILKCAP_NORMAL,
		MILKCAP_BREAK,
		SKIRT,
		NOTE,
		NAIL,
		LASTBOSS,
		UPBOX,
		WIDTH1,
		DOWNBOX,
		PEDKOTOKO,
		TREE,
		HOME,
		TITKOTOKO,
		THORN,
	};
	//FBXの名前
	enum FBXName {
		PLAYER,
		PLAYER2,
		KIDO,
		SIORI,
		TIEYOSHI
	};

public:
	static ModelManager* GetInstance();		//取得用
	void StartInitialize();
	void SecondInitialize();//初期化
	void Finalize();//解放
	//読み込み
	void LoadModel(const ModelName modelName, std::string fileName);
	void LoadFBXModel(const FBXName modelName, std::string fileName);
	IKEModel* GetModel(ModelName modelName) { return model[modelName]; }
	IKEFBXModel* GetFBXModel(FBXName FBXName) { return fbxModel[FBXName]; }
private:
	static std::map<ModelName, IKEModel*> model;	//モデル格納マップ
	static std::map<FBXName, IKEFBXModel*> fbxModel;
};