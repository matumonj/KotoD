#pragma once
#include "InterAttack.h"
class ShotBul;

class DarkSutoponShot :
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

    int ShotCount;
    bool ActionStart;
    std::vector<ShotBul*>bullist;

    std::array<std::unique_ptr<IKEObject3d>, BulSize>ShotObj;
    std::array<bool, BulSize>ShotAlive;
    std::array<float, 4>AreaAngle = {};
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
    //フェーズ
    enum Phase
    {
        NON,
        SHOT,
        END
    }_phase = Phase::NON;
    int PhaseCount = 0;

    int ShotSize;
    bool CanAction;
    int DarkCount;
   
    static void (DarkSutoponShot::* stateTable[])();

    void Phase_Idle();

    void Phase_Shot();

    void Phase_End();


    void RottoPlayer();

    void CollideGhost();

    void FollowPlayerAct();
    
public:
    bool GetActionStart() { return ActionStart; }
	void SetActionStart(bool f) { ActionStart = f; }
    int GetDarkCount() { return DarkCount; }
    int SetDarkCount(int count) { DarkCount = count; }
    bool GetIdleDam() { return IdleRecv; }
    void SetIdleDam(bool f) { IdleRecv = f; }
    bool GetCanAction() { return CanAction; }
    void SetCanAction(bool f) { CanAction = f; }

};


class ShotBul
{
public:
    void init();

    void Upda();

    void Draw();

    void Collide();

    void SetSutopos(XMFLOAT3 pos) { Sutopos = pos; }
   void SetSutorot(XMFLOAT3 rot) { SutoRot = rot; }
    void SetBoss(InterBoss* boss) { this->boss = boss; }
    
    bool ShotEnd() { if (!BulAlive[0] && !BulAlive[1] && !BulAlive[2])return true; else return false; }
private:
    static const int BulSize = 3;

    InterBoss* boss;
    XMFLOAT3 Sutopos,SutoRot;
    std::array < std::unique_ptr<IKEObject3d>, BulSize > ShotObj;
	std::array<XMFLOAT3, BulSize>BulPos;
    std::array<bool, BulSize>BulAlive;
    std::array<XMFLOAT3, BulSize>BulRot;
    std::array<float, BulSize>BulAlpha;

    std::array<XMVECTOR, BulSize>move;
    std::array<XMMATRIX, BulSize>matRot;
};