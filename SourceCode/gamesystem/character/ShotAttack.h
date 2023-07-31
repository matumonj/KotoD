#pragma once
#include "InterAttack.h"
#include"IKESprite.h"
#include "Ghost.h"
class ShotAttack :
    public InterAttack
{
public:
    void Init() override;

    void Upda() override;

    void Draw(DirectXCommon* dxCommon) override;

    void SpriteDraw() override;

    void DeathUpdate(int Timer) override;
private:
    //モデル、攻撃予測の範囲
    static constexpr int BulSize = 3;
    std::array<std::unique_ptr<IKEObject3d>, BulSize>ShotObj;
	std::array<std::unique_ptr<IKESprite>, 4>ShotArea;
    std::array<float, 4>AreaAngle={};
    std::array<XMFLOAT3, BulSize>BulPos;
    std::array<bool, BulSize>BulAlive;
    std::array<XMFLOAT3, BulSize>BulRot;
    std::array<float, BulSize>BulAlpha;

    std::array<XMVECTOR, BulSize>move;
    std::array<XMMATRIX, BulSize>matRot;
    int index = 0;
    float RotEaseTime;
    XMFLOAT3 OldRot;
    int TargetGhost;
    float RottoGhost;
    bool IdleRecv;
    //std::vector<XMVECTOR>PositionB;
    float AddRot;
public:
	//フェーズ
    enum Phase
    {
        NON,
        SHOT,
        END
    }_phase=Phase::NON;
    Phase GetPhase() { return _phase; }

private:
    bool TriggerAttack;
	int PhaseCount = 0;

    int DarkCount;

    static void (ShotAttack::* stateTable[])();

    void Phase_Idle();

    void Phase_Shot();

    void Phase_End();

    void Collide();
    OBB obb[3];
    float oldroty;
    float currentEase;
    XMFLOAT3 TargetPos;
    void RottoPlayer();

    void CollideGhost();

    void FollowPlayerAct();
public:
    int GetDarkCount() { return DarkCount; }
    int SetDarkCount(int count) { DarkCount = count; }
    bool GetIdleDam() { return IdleRecv; }
    void SetIdleDam(bool f) { IdleRecv=f; }
    Phase GEtPhase() { return _phase; }
    int GetTargetGhost() { return TargetGhost; }
    float GetBulAlpha(int i) { return BulAlpha[i]; }

    //Phase GetPhase();
};

