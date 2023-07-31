#pragma once
#include "InterAttack.h"
class SmashShotAttack :
    public InterAttack
{
public:
    void Init() override;

    void Upda() override;

    void Draw(DirectXCommon* dxCommon) override;

    void SpriteDraw() override;

    void BomParticle();

    void DeathUpdate(int Timer) override;
private:
    //モデル、攻撃予測の範囲
    static constexpr int BulSize = 10;
    std::array<std::unique_ptr<IKEObject3d>, BulSize>ShotObj;
    std::array<std::unique_ptr<IKETexture>, BulSize>ShotArea;
    std::array<float, BulSize>AreaScl;
    std::array<float, BulSize>AreaAlpha;
    std::array<float, BulSize>BulEaseCount;
    std::array<float, BulSize>PosXVal, PosZVal;
    std::array<float, BulSize>BulSpeed;
    std::array<float, BulSize>BulRotZ;
    std::array<XMFLOAT3, BulSize>BulPos;
    std::array<XMFLOAT3, BulSize>BulRot;
    std::array<float, BulSize>BulAlpha;
    std::array<XMFLOAT3, BulSize>ImpactTexPos;

    std::array<XMVECTOR, BulSize>move;
    std::array<XMMATRIX, BulSize>matRot;
    int index = 0;

    XMFLOAT3 OldRot;
public:
    //フェーズ
    enum Phase
    {
        NON,
        SHOT,
        IMPACT,
        END
    }_phase = Phase::NON;
    Phase GetPhase() { return _phase; }
private:

    int PhaseCount = 0;

    static void (SmashShotAttack::* stateTable[])();

    void Phase_Idle();

    void Phase_Shot();

    void Phase_Impact();
    void Phase_End();


    void RottoPlayer();

public:
    float GetBulpos(int index) { return BulPos[index].y; }
    //Phase GetPhase();
};

