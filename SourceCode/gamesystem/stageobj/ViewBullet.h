#pragma once
#include "InterBullet.h"
//�`���[�W���̉����ł���e�̃N���X
class ViewBullet :
	public InterBullet {
public:
	ViewBullet();

	bool Initialize() override;//������

	/// <summary>
	/// ImGui�`��
	/// </summary>
	void ImGui_Origin() override;

	void Draw_Origin() override;
private:
	//�e�̓�������
	void Action() override;

public:
	//�`�揈�����ꂾ��������
	void View_Draw();
public:
	//gettersetter
	
private:

};
