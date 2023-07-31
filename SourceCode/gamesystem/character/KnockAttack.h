#pragma once
#include "InterAttack.h"
#include"IKETexture.h"
class KnockAttack :
    public InterAttack
{
public:
    void Init() override;

    void Upda() override;

    void Draw(DirectXCommon* dxCommon) override;

    void SpriteDraw() override;

    void DeathUpdate(int Timer) override;

public:
    void AppearUpdate(int Timer);

private:
    void DeathParticle();
private:
    bool KnockF=false;
    //CSV用
    float KnockVal;
    float KnockAreaRad;

    std::unique_ptr<IKETexture>KnockImpTex;
    std::unique_ptr<IKETexture>KnockImpTex2;

    XMFLOAT3 TexScl, TexScl2;
    float TexAlpha, TexAlpha2;

    std::unique_ptr<IKEObject3d>darksutopon;
    XMFLOAT3 stopos;
    XMFLOAT3 strot;
    float impactMoving;
    float SutoponCircleAngle;


    float GuardEaseT;
    bool GuardStart = false;
    int GuardTime;

    float JFrame;

    float DownMoveVal;

    float ReturnEaseT;
    float OldPosY;
    void ImpactAction();

    bool m_End = false;

    enum EndState {
        END_SET,
        END_MOVE,
        END_STOP
    }_EndState;

    float m_Frame = {};

    enum AppState {
        APP_SET,
        APP_MOVE,
        APP_END,
    }_AppState;
    float m_SinAngle = {};
    float m_SinAngle2 = {};

    //上昇度
    float m_AddPower = 0.0f;
    //重力加速度
    float m_Gravity = 0.03f;
public:
    inline void setKnockF(bool f) { KnockF = f; TexAlpha = 1.f; TexAlpha2 = 1.f; }
    bool GetKnockF() { return KnockF; }
    IKEObject3d* Sutoobj() { return darksutopon.get(); }
};

