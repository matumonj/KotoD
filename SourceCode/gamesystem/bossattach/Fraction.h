#pragma once
#include"InterBullet.h"
#include"BreakEffect.h"
#include"IKEObject3d.h"
#include"IKEModel.h"
#include"IKETexture.h"
#include"InterBullet.h"
#include"BreakEffect.h"

class Fraction 
{
private:
    enum PopState {
        Before=0,
        After,
    }pop_;
private:
    // DirectX::Çè»ó™
    using XMFLOAT2 = DirectX::XMFLOAT2;
    using XMFLOAT3 = DirectX::XMFLOAT3;
    using XMFLOAT4 = DirectX::XMFLOAT4;
    using XMVECTOR = DirectX::XMVECTOR;
    using XMMATRIX = DirectX::XMMATRIX;

public:
    ~Fraction();

    void Init(const XMFLOAT3& BossPos);
    
    void Update(vector<InterBullet*> bullet);

    void Draw(DirectXCommon* dxCommon);

    bool IsDelete()const { return Isdelete; }

    void Obj_Set();

    void Pop();

    void Spatter();

    void Drop(const XMFLOAT3& dropposiition);

    void ColPlayer(vector<InterBullet*> bullet);

    void Break();
public:
private:
    vector<InterEffect*> effects;
    unique_ptr<IKEObject3d> m_Object;
    unique_ptr<IKETexture> tex;
    XMFLOAT3 boss_pos_{};
    XMFLOAT3 m_Scale{};
    XMFLOAT3 m_Position{0.f,-1000.f,0.f};
    XMFLOAT3 m_Rotation{};
    XMFLOAT4 m_Color{ 1.f,1.f,1.f,1.f };
    XMFLOAT3 pop_pos_{};

    XMFLOAT3 drop_pos_{};
    float m_HP = 2.f;

    bool Isdelete = false;
    bool drop_F = false;
    float commandTimer = 0.0f;

    float m_Radius = 3.f;
};

