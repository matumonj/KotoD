#include "SceneChanger.h"
#include "ImageManager.h"
#include <SceneManager.h>
#include "Helper.h"

Feed* SceneChanger::feed = nullptr;
bool SceneChanger::feedF = false;

SceneChanger::SceneChanger() {
}

SceneChanger::~SceneChanger() {
	for (std::unique_ptr<IKESprite>& sprite : sprites) {
		sprite.reset();
	}
	sprites.clear();
}


void SceneChanger::Initialize() {
	for (auto i = 0; i < width_num; i++) {
		for (auto j = 0; j < height_num; j++) {
			std::unique_ptr<IKESprite> newSprite;
			newSprite = IKESprite::Create(ImageManager::WHITE, { 0,0 });
			newSprite->SetAnchorPoint({ 0.5f,0.5f });
			newSprite->SetPosition({ (float)(i * base_size) + base_size / 2,(float)(j * base_size) + base_size / 2 });
			newSprite->SetSize({ 0,0 });
			DirectX::XMFLOAT4 col = { 0.f,0.f,0.f,1 };
			newSprite->SetColor(col);

			sprites.push_back(std::move(newSprite));
			frame.push_back(0.f);
		}
	}
	color_.push_back({ 0.3f,0.3f,0.5f,1 });
	color_.push_back({ 0.5f,0.7f,0.7f,1 });
	color_.push_back({ 0.5f,1.f,1.f,1 });


	start_sprites = IKESprite::Create(ImageManager::WHITE, { 0,0 });
	start_sprites->SetSize({ width, height });
	start_sprites->SetColor({0,0,0,0});

}
void SceneChanger::Update() {
	if (feedin_start) {
		feedin_frame += 1.0f / end_feedin;
		float color = Ease(InOut, Quad, feedin_frame, 1.0, 0.f);
		start_sprites->SetColor({ 0.f,0.f,0.f,color });
		Helper::GetInstance()->Clamp(feedin_frame,0.0f,1.0f);
		if (feedin_frame==1.0f) {
			feedin_start = false;
		}
	}

}
void SceneChanger::InitializeOver() {
	for (int i = 0; i < width_num; i++) {
		for (int j = 0; j < height_num; j++) {
			std::unique_ptr<IKESprite> newSprite;
			newSprite = IKESprite::Create(ImageManager::WHITE, { 0,0 });
			newSprite->SetPosition({ (float)(i * base_size),(float)(j * base_size) });
			newSprite->SetSize({ 0,0 });
			DirectX::XMFLOAT4 col = { 0,0,0,1 };
			newSprite->SetColor(col);
			over_sprites.push_back(std::move(newSprite));
		}
	}
}

void SceneChanger::Draw() {
	if (feedin_start) {
		IKESprite::PreDraw();
		start_sprites->Draw();
	}
	if (!easing_start) { return; }
	IKESprite::PreDraw();
	for (std::unique_ptr<IKESprite>& sprite : sprites) {
		sprite->Draw();
	}
	for (std::unique_ptr<IKESprite>& sprite : over_sprites) {
		sprite->Draw();
	}
}

bool SceneChanger::ChangeScene(const std::string& sceneName, const ReverseType _reverse) {
	if (!easing_start) { return false; }
	if (ease_frame > 1.0f) {
		SceneManager::GetInstance()->ChangeScene(sceneName);
		return true;
	} else {
		ease_frame += 1.0f / end_frame;
	}
	float rot = 0;
	rot = Ease(InOut, Quint, ease_frame, 0, 360.f * (float)_reverse);
	DirectX::XMFLOAT2 size{};
	size.x = Ease(InOut, Quint, ease_frame, 0, (float)base_size);
	size.y = Ease(InOut, Quint, ease_frame, 0, (float)base_size);
	for (std::unique_ptr<IKESprite>& sprite : sprites) {
		sprite->SetRotation(rot);
		sprite->SetSize({ size.x,size.y });
	}
	return false;
}

bool SceneChanger::ChangeSceneLose(const std::string& sceneName) {
	if (!easing_start) { return false; }
	if (ease_frame > 1.0f) {
		SceneManager::GetInstance()->ChangeScene(sceneName);
		return true;
	} else {
		ease_frame += 1.0f / end_frame;
	}
	float rot = 0;
	rot = Ease(InOut, Quint, ease_frame, 0, 360.f);
	DirectX::XMFLOAT2 size{};
	size.x = Ease(InOut, Quint, ease_frame, 0, (float)base_size);
	size.y = Ease(InOut, Quint, ease_frame, 0, (float)base_size);
	for (std::unique_ptr<IKESprite>& sprite : sprites) {
		sprite->SetColor({0,0,0,1});
		sprite->SetRotation(rot);
		sprite->SetSize({ size.x,size.y });
	}
	return false;
}

bool SceneChanger::ChangeSceneExtra(const std::string& sceneName, const ReverseType _reverse) {
	if (!easing_start) { return false; }
	//if (ease_frame > 1.0f) {
	size_t squea = 3;
	if (frame[squea - 1] >= 1.0f) {
		SceneManager::GetInstance()->ChangeScene(sceneName);
		return true;
	}
	//
	frame[0] += 1.0f / 25.f;
	for (auto i = 0; i < squea; i++) {
		if (i != 0 && frame[i - 1] >= 1.f) {
			frame[i] += 1.0f / 20.f;
		}
		frame[i] = min(1.0f, frame[i]);

		DirectX::XMFLOAT2 m_SpritesSize = {
		Ease(In, Quad, frame[i], 0.f, 1280.f),
		Ease(In, Quad, frame[i], 0.f, 1280.f)
		};
		sprites[i]->SetPosition({ width / 2 ,height / 2 });
		sprites[i]->SetColor({0.f, 0.f, 0.f, 1});
		sprites[i]->SetSize(m_SpritesSize);
	}
	return false;
}

void SceneChanger::ChangeGameOver() {

	if (easing_start) {
		ease_frame += 1.0f / 120.0f;
		DirectX::XMFLOAT2 size = { (float)base_size , (float)base_size };
		if (ease_frame <= 1.0f) {
			size.x = Ease(Out, Quint, ease_frame, 0, (float)base_size);
			size.y = Ease(Out, Quint, ease_frame, 0, (float)base_size);
		} else if (ease_frame >= 1.5f) {
			SceneManager::GetInstance()->ChangeScene("TITLE");

		}

		for (std::unique_ptr<IKESprite>& sprite : over_sprites) {
			sprite->SetAnchorPoint({ 0,0 });
			sprite->SetSize({ size.x,size.y });
		}
	}
}

void SceneChanger::FeedInit()
{
	feed = new Feed();
}

void SceneChanger::FeedDraw()
{
	//if (!feedF)return;
	IKESprite::PreDraw();
	feed->Draw();
	IKESprite::PostDraw();
}



void SceneChanger::FeedChange()
{
	if (feedF) {
		feed->FeedIn(Feed::FeedType::BLACK, 0.01f, feedF);
		if (feed->GetAlpha() >= 1.0f) {
			SceneManager::GetInstance()->ChangeScene("LOAD");
		}
	}
}
