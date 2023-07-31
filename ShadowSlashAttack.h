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

    //モデル、攻撃予測の範囲
    static constexpr int SwordSize = 10;
    static constexpr int KotokoShadow = 4;

    struct Sword
    {
        std::unique_ptr<IKEObject3d>Obj;

        XMFLOAT3 Pos, Rot, Scl;
        float Alpha;
        float EaseCOunt;
    };

    std::array<Sword, SwordSize>Swords;
	std::array<std::unique_ptr<IKEObject3d>, SwordSize>SwordObj;
    std::array<std::unique_ptr<IKEObject3d>, KotokoShadow>Kotoko;
    std::array<XMFLOAT3, KotokoShadow>KotokoPos;

	std::array<std::unique_ptr<IKETexture>, 2>ShotArea;
    std::array<float,2>AreaScl;
    std::array<float, 2>AreaAlpha;
    std::array<XMFLOAT3, 2>ImpactTexPos;
    std::array<XMVECTOR, SwordSize>move;
    std::array<XMMATRIX, SwordSize>matRot;


	int index = 0;
	float Cinter;

    //フェーズ
    enum Phase
    {
        NON,
        AREAVIEW,
        IMPACTSLASH,
        END
    }_phase = Phase::NON;
    int PhaseCount = 0;
    int IdleCount;
    static void (ShadowSlashAttack::* stateTable[])();

    void Phase_Idle();

    void Phase_ViewArea();

    void Phase_Impact();

	void Phase_End();

};

