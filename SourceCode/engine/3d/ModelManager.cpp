#include"ModelManager.h"
#include "IKEFbxLoader.h"

std::map<ModelManager::ModelName, IKEModel*> ModelManager::model;
std::map<ModelManager::FBXName, IKEFBXModel*> ModelManager::fbxModel;

ModelManager* ModelManager::GetInstance()
{
	static ModelManager instans;
	return &instans;
}

//モデル読み込み(最初から読み込むもの)
void ModelManager::StartInitialize()
{
	LoadModel(Bullet, "UVSphere");
	LoadModel(SKIRT, "skirt");
	LoadModel(Sutopon, "sutopon");
	LoadModel(TREE, "tree");
	LoadModel(Food, "food");
	LoadModel(Ghost, "ghost");
	LoadModel(Ground, "ground");
	LoadModel(HOME, "home");
	LoadFBXModel(PLAYER, "kotoko");
	LoadModel(Skydome, "skydome");
}

//モデル読み込み(非同期処理で読み込むもの)
void ModelManager::SecondInitialize()
{

	LoadModel(Nucleus, "nucleus");
	LoadModel(GhostBullet, "voice");
	LoadModel(BulEnemy, "tri");
	LoadModel(Tyuta, "tieyosi_idol");
	LoadModel(Buddy,"buddy");
	LoadModel(MobUsa, "mobusa");
	LoadModel(Camera, "A&J");
	LoadModel(Heart, "heart");
	LoadModel(Pedestal, "dodai");
	LoadModel(KIDO_OBJ, "kidokunobj");
	LoadModel(DJ, "DJ");
	LoadModel(CD, "CD");

	LoadModel(NOTE, "NOTE");
	LoadModel(UPBOX, "upbox");
	LoadModel(WIDTH1, "width1");
	LoadModel(WIDTH2, "width2");
	LoadModel(WIDTH3, "width3");
	LoadModel(WIDTH4, "width4");
	LoadModel(DOWNBOX, "downbox");

	LoadModel(GLASS1, "glass1");
	LoadModel(MILKCAP_NORMAL, "milkCap_normal");
	LoadModel(MILKCAP_BREAK, "milkCap_break");
	LoadModel(NAIL, "Nail");
	LoadModel(LASTBOSS, "burapon");
	LoadModel(TITKOTOKO, "titlekotoko");
	LoadModel(THORN, "thorn");

	LoadFBXModel(PLAYER2, "dkotoko");
	LoadFBXModel(SIORI, "siori_idol");
	LoadFBXModel(KIDO, "kidokun_idol");
	LoadFBXModel(TIEYOSHI, "tieyosi_anime");
	
	LoadModel(PEDKOTOKO, "Pedestalkotoko");
}

void ModelManager::Finalize()
{
	model.clear();
	fbxModel.clear();
}

void ModelManager::LoadModel(const ModelName modelName, std::string fileName)
{
	model[modelName] = new IKEModel();
	model[modelName] = IKEModel::LoadFromOBJ(fileName, true);
}

void ModelManager::LoadFBXModel(const FBXName modelName, std::string fileName)
{
	fbxModel[modelName] = new IKEFBXModel();
	fbxModel[modelName] = IKEFbxLoader::GetInstance()->LoadModelFromFile(fileName);
}
