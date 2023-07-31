#pragma once
#include "ParticleManager.h"

using namespace std;         //  名前空間指定
/// <summary>
/// パーティクルエミッター
/// </summary>
class ParticleEmitter
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private: //シングルトン化
	//コンストラクタを隠蔽
	ParticleEmitter() = default;
	//デストラクタを隠蔽
	~ParticleEmitter() = default;
public: //メンバ関数
	static ParticleEmitter* GetInstance();

	//初期化
	void Initialize();

	//更新
	void Update();

	void IntroDraw();
	//描画
	void FlontDrawAll();

	void DeathDrawAll();
	//壁のパーティクルの描画
	void BackDrawAll();
	//エフェクト
	void FireEffect(const int life,const XMFLOAT3& pos,const float startscale, const float endscale,const XMFLOAT4& startcolor,const XMFLOAT4& endcolor);
	void DarkOtiEffect(const int life, const XMFLOAT3& l_pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//コトコエフェクト
	void KotokoEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//制御点のエフェクト
	void LimitEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//ダークコトコののエフェクト
	void DarkEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//爆発エフェクト
	void Explosion(const int life, const XMFLOAT3& pos, const float size,const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor,const int ExploType);
	//ダメージエフェクト
	void Break(const int life, const XMFLOAT3& pos,const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor,const float Gravity,float divi);
	//死んだ時のエフェクト
	void DeathEffect(int life, XMFLOAT3 l_pos, float startscale, float endscale, XMFLOAT4 startcolor, XMFLOAT4 endcolor, float velocity);
	//ヒールエフェクト
	void HealEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//カメラボスUltimateエフェクト
	void CameraEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//セレクトエフェクト
	void SelectEffect(const int life, const XMFLOAT3& pos, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//削除
	void AllDelete();
	//爆発　ボス
	void DeathEffectBoss(const int life, const XMFLOAT3& pos, const float size, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);
	//爆発　ボス
	void ExproEffectBoss(const int life, const XMFLOAT3& pos, const float size, const float startscale, const float endscale, const XMFLOAT4& startcolor, const XMFLOAT4& endcolor);

private: //メンバ関数
	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture();

private: //メンバ変数
	//パーティクルマネージャー(丸)
	unique_ptr<ParticleManager> circleParticle;
	//パーティクルマネージャー(丸)
	unique_ptr<ParticleManager> darkcircleParticle;
	//パーティクルマネージャー(丸)
	unique_ptr<ParticleManager> kotokoParticle;
	//パーティクルマネージャー(ダークコトコ)
	unique_ptr<ParticleManager> darkParticle;
	//パーティクルマネージャー(壁)
	unique_ptr<ParticleManager> wallParticle;
	//パーティクルマネージャー(死亡)
	unique_ptr<ParticleManager> deathParticle;
	//パーティクルマネージャー(ヒール)
	unique_ptr<ParticleManager> healParticle;
	//パーティクルマネージャー(ボスデス)
	unique_ptr<ParticleManager>BossDeadParticle;
	//パーティクルマネージャー(cameraBOss)
	unique_ptr<ParticleManager> PhotoParticle;

private:
	int m_Timer = 0;
};
