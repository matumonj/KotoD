#include "BackObj.h"
#include "JsonLoader.h"
#include "ParticleEmitter.h"
#include "LeftLimit.h"
#include "RightLimit.h"
#include "DownLimit.h"
#include "ImageManager.h"
#include "SceneManager.h"
#include "UpLimit.h"
BackObj* BackObj::GetInstance()
{
	static BackObj instance;

	return &instance;
}
//�}�b�v�ǂݍ���
void BackObj::LoadMap() {

	jsonData = JsonLoader::LoadFile("GameScene");

	//���X�|�[���ʒu
	for (auto& objectData : jsonData->objects) {
		if (objectData.GimmickName == "No") {
			// �t�@�C��������o�^�ς݃��f��������
			IKEModel* model = IKEModel::LoadFromOBJ(objectData.fileName, true);
			decltype(models)::iterator it = models.find(objectData.fileName);
			if (it != models.end()) {
				model = it->second;
			}

			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			IKEObject3d* newObject = IKEObject3d::Create();
			newObject->SetModel(model);
			XMFLOAT3 Startpos;

			// ���W
			DirectX::XMStoreFloat3(&Startpos, objectData.translation);

			newObject->SetPosition(Startpos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rotation);
			newObject->SetRotation(rot);

			// ���W
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scaling);
			newObject->SetScale(scale);

			newObject->SetTiling(30.0f);
			// �z��ɓo�^
			objects.push_back(newObject);
		}
	}
}

//������
bool BackObj::Initialize() {
	m_Position = { 0.0f,15.0f,0.0f };
	m_Scale = { 0.3f,0.3f,0.3f };
	m_Color = { 0.5f,0.5f,0.5f,1.0f };

	ground.reset(IKETexture::Create(ImageManager::GROUND, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	ground->TextureCreate();


	return true;
}

//�X�V
void BackObj::Update() {
	//Json�p
	for (auto& object : objects) {
		object->Update();
	}
	ground->SetPosition({ 0,0,0 });
	ground->SetScale({ 30,30,30 });
	ground->SetRotation({ 90,0,0 });
	ground->SetColor({ 0.6f,0.6f,0.6f,0.5f });
	ground->Update();
	//�ړ�����̃��C���Ƀp�[�e�B�N���ݒu
	DownLimit::GetInstance()->LimitParticle();
	UpLimit::GetInstance()->LimitParticle();
	LeftLimit::GetInstance()->LimitParticle();
	RightLimit::GetInstance()->LimitParticle();
	//Obj_SetParam();
}

//�`��
void BackObj::Draw(DirectXCommon* dxCommon) {
	//Json�p
	for (auto& object : objects) {
		object->Draw();
	}
	IKETexture::PreDraw2(dxCommon, 0);
	
	ground->Draw();
	IKETexture::PostDraw();
	//Obj_Draw();
}
