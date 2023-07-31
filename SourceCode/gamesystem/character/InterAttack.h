#pragma once
#include<memory>
#include<array>
#include"IKEObject3d.h"
#include"InterBoss.h"
#include<DirectXMath.h>

using namespace DirectX;
class InterAttack
{
public:
	virtual void Init() = 0;

	virtual void Upda() = 0;

	virtual void Draw(DirectXCommon* dxCommon) = 0;

	virtual void SpriteDraw() = 0;

	virtual void DeathUpdate(int Timer) = 0;

public:
	bool GetActionEnd() { return ActionEnd; }

	void SetActionEnd(bool f) { ActionEnd = f; }
	void SetBoss(InterBoss* boss) { this->boss = boss; };
protected:
	InterBoss* boss = nullptr;

	int AttackTimer = 0;

	float FollowAngle;

	bool ActionEnd;

	XMFLOAT3 m_Position, m_Rotation;

	int CanRand;

	float Dam;

	float WalkSpeed;
	float GuardTimes;

	XMFLOAT3 SutoPos, SutoRot;
public:
	void FollowPlayer();
	int GetCanRand() { return CanRand; }
	void SetCanRand(int num) { CanRand = num; }
	void SetDam(float dam) { Dam = dam; }
	void SetSutoPos(XMFLOAT3 pos) { SutoPos = pos; }
	void SetSutoRot(XMFLOAT3 rot) { SutoRot = rot; }
};

