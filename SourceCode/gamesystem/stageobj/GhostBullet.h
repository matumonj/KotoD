#pragma once
#include "InterBullet.h"

//Œ¾—ìƒNƒ‰ƒX
class GhostBullet :
	public InterBullet {
public:
	GhostBullet();

	bool Initialize() override;//‰Šú‰»

	/// <summary>
	/// ImGui•`‰æ
	/// </summary>
	void ImGui_Origin() override;

	void Draw_Origin() override;
private:
	//’e‚Ì“®‚­ˆ—
	void Action() override;

private:
	void VanishBullet();

private:
	float m_Frame = 0.0f;
	float m_AddFrame = {};
	float m_StartScale = {};
	float m_EndScale = {};
};
