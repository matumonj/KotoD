#pragma once
#include "InterAttack.h"

class ShadowSlashAttack :
    public InterAttack
{
public:
    void Init() override;

    void Upda() override;

    void Draw(DirectXCommon* dxCommon) override;

    void SpriteDraw() override;

    void DeathUpdate(int Timer) override;

    struct Sword
    {
        std::unique_ptr<IKEObject3d>Obj;

        XMFLOAT3 Pos, Rot, Scl;
        float Alpha;
        float EaseCount;
        float YSclRandMax;
    };

    //モデル、攻撃予測の範囲
    static constexpr int SwordSize = 18;
    static constexpr int KotokoShadow = 4;

    std::array<Sword, SwordSize>Swords_H;
    std::array<Sword, SwordSize>Swords_W;


    std::array<std::unique_ptr<IKEObject3d>, SwordSize>ShotObj;
    std::array<std::unique_ptr<IKEObject3d>, KotokoShadow>Kotoko;
    std::array<float, KotokoShadow>KotokoAlpha;
    std::array<XMFLOAT3, 2>KotokoPos;

    std::array<std::unique_ptr<IKETexture>, 2>ShotArea;
    std::array<float, 2>AreaScl;
    std::array<float, 2>AreaAlpha;
    std::array<XMFLOAT3, 2>ImpactTexPos;

    std::array<XMVECTOR, SwordSize>move;
    std::array<XMMATRIX, SwordSize>matRot;
    int index = 0;
    std::array<float, 2> Cinter;
    int IdleCount;
    XMFLOAT3 OldRot;

    //フェーズ
    enum Phase
    {
        NON,
        AREAVIEW,
        IMPACTSLASH,
        NAILDEST,
        END
    }_phase = Phase::NON;
    int PhaseCount = 0;

    static void (ShadowSlashAttack::* stateTable[])();

    void Phase_Idle();

    void Phase_ViewArea();

    void Phase_Impact();

    void Phase_Dest();
    void Phase_End();

};

