#pragma once
#include "InterBullet.h"
//ƒ`ƒƒ[ƒW’†‚Ì‰Â‹‰»‚Å‚«‚é’e‚ÌƒNƒ‰ƒX
class ViewBullet :
	public InterBullet {
public:
	ViewBullet();

	bool Initialize() override;//‰Šú‰»

	/// <summary>
	/// ImGui•`‰æ
	/// </summary>
	void ImGui_Origin() override;

	void Draw_Origin() override;
private:
	//’e‚Ì“®‚­ˆ—
	void Action() override;

public:
	//•`‰æˆ—‚±‚ê‚¾‚¯•ª‚¯‚é
	void View_Draw();
public:
	//gettersetter
	
private:

};
