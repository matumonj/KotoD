#pragma once

#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "IKEObject3d.h"

/// <summary>
/// �R���C�_�[���N���X
/// </summary>
class BaseCollider
{
public:
	friend class CollisionManager;
	BaseCollider() = default;
	virtual ~BaseCollider() = default;

	inline void SetObject(IKEObject3d* object) {
		this->object3d = object;
	}

	inline IKEObject3d* GetObject3d() {
		return object3d;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	inline CollisionShapeType& GetShapeType() { return shapeType; }

	///// <summary>
	///// �Փˎ��R�[���o�b�N�֐�
	///// </summary>
	///// <param name="info">�Փˏ��</param>
	inline void OnCollision(const CollisionInfo& info) {
		object3d->OnCollision(info);
	}


	/// <summary>
	/// �����蔻�葮�����Z�b�g
	/// </summary>
	/// <param name="attribute">�����蔻�葮��</param>
	inline void SetAttribute(unsigned short attribute) {
		this->attribute = attribute;
	}

	/// <summary>
	/// �����蔻�葮����ǉ�
	/// </summary>
	/// <param name="attribute">�����蔻�葮��</param>
	inline void AddAttribute(unsigned short attribute) {
		this->attribute |= attribute;
	}

	/// <summary>
	/// �����蔻�葮�����폜
	/// </summary>
	/// <param name="attribute">�����蔻�葮��</param>
	inline void RemoveAttribute(unsigned short attribute) {
		this->attribute &= !attribute;
	}


protected:
	IKEObject3d* object3d = nullptr;
	// �`��^�C�v
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
	// �����蔻�葮��
	unsigned short attribute = 0b1111111111111111;
};

