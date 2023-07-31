#pragma once
#include "Easing.h"
#include "IKESprite.h"
#include <WinApp.h>

#include<vector>
#include <memory>
#include<string>
#include <list>

#include "Feed.h"

//シーンチェンジクラス
class SceneChanger {
public:
	SceneChanger();
	~SceneChanger();

	/// <summary>
	/// 初期k処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 初期k処理
	/// </summary>
	void InitializeOver();

	static Feed* feed;
	static bool feedF;
	static void FeedInit();
	static void FeedChange();
	void SetFeedF(bool f) { feedF = f; }
	static void FeedDraw();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	enum ReverseType {
		NonReverse = 1,
		Reverse = -1
	};
	/// <summary>
	/// シーン切り替え
	/// </summary>
	/// <param name="sceneName"></param>
	bool ChangeScene(const std::string& sceneName, const ReverseType _reverse);
	/// <summary>
/// シーン切り替え
/// </summary>
/// <param name="sceneName"></param>
	bool ChangeSceneLose(const std::string& sceneName);
	/// <summary>
	/// シーン切り替え
	/// </summary>
	/// <param name="sceneName"></param>
	bool ChangeSceneExtra(const std::string& sceneName, const ReverseType _reverse);

	/// <summary>
	/// シーン切り替え
	/// </summary>
	/// <param name="sceneName"></param>
	void ChangeGameOver();


	void  ChangeFeedStart() {
		feedin_start = false; feedin_frame= 0.f;
	}
	//const bool& GetEasingStart() { return feedin_start; }

	/// <summary>
	/// イージングスタート
	/// </summary>
	void  ChangeStart() { easing_start = true; }
	const bool& GetEasingStart() { return easing_start; }
private:
	std::vector<std::unique_ptr<IKESprite>> sprites;
	std::list<std::unique_ptr<IKESprite>> over_sprites;

	std::unique_ptr<IKESprite> start_sprites;


	const float width = WinApp::window_width;
	const float height = WinApp::window_height;


	const int base_size = 80;
	const int width_num = (int)width / base_size;
	const int height_num = (int)height / base_size;

	std::vector<float> frame;
	std::vector<DirectX::XMFLOAT4> color_;


	bool feedin_start = true;
	float feedin_frame = 0.0f;
	//何フレームで終わるか
	float end_feedin = 20.0f;

	bool easing_start = false;
	float ease_frame = 0.0f;
	//何フレームで終わるか
	float end_frame = 100.0f;
};