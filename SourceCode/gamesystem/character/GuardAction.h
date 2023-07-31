#pragma once
#include"IKETexture.h"
#include "InterAttack.h"
class GuardAction :
    public InterAttack
{
public:
    void Init() override;

    void Upda() override;

    void Draw(DirectXCommon* dxCommon) override;

    void SpriteDraw() override;

    void DeathUpdate(int Timer) override;
private:
    static constexpr int GuardSize = 6;

    std::array<unique_ptr<IKEObject3d>, GuardSize>guardtex;
    std::array<float, GuardSize>guardtexEaseT;
    std::array<XMFLOAT3, GuardSize>guardRot;
    std::array<XMFLOAT3, GuardSize>guardScl;
    std::array<XMFLOAT3, GuardSize>guardPos;
    std::array<XMFLOAT3, GuardSize>guardOldPos;
	std::array<float, GuardSize>guardAlpha;
    std::array<float, GuardSize>guardUvY;



    float GuardEaseT;
    bool GuardStart = false;
    float GuardTime;
private:
    void GuardAreacreate();
public:
    bool GetGuardStart() { return GuardStart; }
    void SetGuardStart(bool f) { GuardStart = f; }
};

