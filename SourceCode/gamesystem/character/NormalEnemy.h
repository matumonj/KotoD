#pragma once
#include"InterEnemy.h"
using namespace std;         //  名前空間指定
//普通の敵
class NormalEnemy :public InterEnemy {
public:
	NormalEnemy();
	bool Initialize() override;//初期化
	void Finalize() override;//開放
	void Action()override;//更新
	void ImGuiDraw()override;
	//void Circle();//円運動
	//void Follow();//追従
	void Draw(DirectXCommon* dxCommon) override;//描画
private:
	void Appearance()override;
	void Particle();//パーティクル
	void RushAction();
	void DeathEffect();
protected:

private:
	//円運動の変数
	float m_CircleAngle = 0.0f;
	float m_CircleRadius = 0.0f;
	float m_CircleSpeed = 0.0f;
	float m_CircleScale = 20.0f;
	//追従用変数
	XMFLOAT3 m_FollowVel{};
	float old;
	bool ret;
	bool appF;
	XMVECTOR PositionA, PositionB;
	XMVECTOR SubVector;
	float t; float RotY;
	int randMove;
	float s_scale = 0.0f;
	int MoveTimer;

	XMFLOAT3 OldPos_BefRot;
	float tmpLength;

	float SummonSpeed;
};

