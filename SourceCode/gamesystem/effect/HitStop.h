#pragma once
//�q�b�g�X�g�b�v�悤�Ȃ̂Ń��\�[�X�͗v��Ȃ�
class HitStop {
public:
	static HitStop* GetInstance();
	void LoadCSV();
	//�X�V�����̂�
	void Update();
	void ImGuiDraw();
public:
	//getter setter
	bool GetHitStop() { return  m_HitStop; }
	void SetHitStop(bool HitStop) { this->m_HitStop = HitStop; }
private:
	bool m_HitStop = false;//�q�b�g�X�g�b�v
	int m_StopTimer = 0;//�q�b�g�X�g�b�v�̎���
	int m_TargetTimer = 0;//�w��̎���
};