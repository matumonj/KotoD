#include "CameraWork.h"
#include "VariableCommon.h"
#include <Easing.h>
#include "Player.h"
#include "ClearText.h"
#include "Helper.h"
CameraWork::CameraWork(XMFLOAT3 eye, XMFLOAT3 target) {
	m_eyePos = eye;
	m_targetPos = target;

	Shake* shake_ = new Shake();
	shake.reset(shake_);

	Feed* feed_ = new Feed();
	feed.reset(feed_);
}
void CameraWork::SplineSet() {
	if (SceneName == "FIRSTSTAGE" || SceneName == "SECONDSTAGE") {
#pragma region First
		{
			if (pointsList.size() == 0) {
				pointsList.emplace_back(XMFLOAT3{ 150,5,0 });
				pointsList.emplace_back(XMFLOAT3{ 130,5,120 });

				pointsList.emplace_back(XMFLOAT3{ 0,5,120 });

				pointsList.emplace_back(XMFLOAT3{ -150,5,100 });


				pointsList.emplace_back(XMFLOAT3{ 0,2,180 });
				pointsList.emplace_back(XMFLOAT3{ 0,10,90 });
				pointsList.emplace_back(XMFLOAT3{ 0,30,0 });

			}
			spline = new Spline();
			spline->Init(pointsList, static_cast<int>(pointsList.size()));
		}
#pragma endregion
	}
	else if (SceneName == "FIVESTAGE" || SceneName == "SIXSTAGE") {
#pragma region First
		{
			if (pointsList.size() == 0) {
				pointsList.emplace_back(XMFLOAT3{ 50,80,50 });
				pointsList.emplace_back(XMFLOAT3{ 0,70,90 });
				pointsList.emplace_back(XMFLOAT3{ -50,80,60 });
				pointsList.emplace_back(XMFLOAT3{ 0,20,10 });
				pointsList.emplace_back(XMFLOAT3{ 50, 70,10 });
				pointsList.emplace_back(XMFLOAT3{ 30,50,60 });
				pointsList.emplace_back(XMFLOAT3{ 0,30,60 });
				pointsList.emplace_back(XMFLOAT3{ -20,20,40 });
				pointsList.emplace_back(XMFLOAT3{ 0,30,10 });
				pointsList.emplace_back(XMFLOAT3{ 0,30,10 });
				//pointsList.emplace_back(XMFLOAT3{ 30,10,-50 });


				//pointsList.emplace_back(XMFLOAT3{ 0,2,180 });
				//pointsList.emplace_back(XMFLOAT3{ 0,10,90 });
				//pointsList.emplace_back(XMFLOAT3{ 0,30,0 });
			}
			spline = new Spline();
			spline->Init(pointsList, static_cast<int>(pointsList.size()));
		}
#pragma endregion
	}
	else if (SceneName == "SEVENSTAGE") {
#pragma region First
		{
			if (pointsList.size() == 0) {
				pointsList.emplace_back(XMFLOAT3{ 0,50,0 });
				pointsList.emplace_back(XMFLOAT3{ 30,40,30 });
				pointsList.emplace_back(XMFLOAT3{ 0,30,60 });
				pointsList.emplace_back(XMFLOAT3{ -30,25,30 });
				pointsList.emplace_back(XMFLOAT3{ 0, 20,10 });
				pointsList.emplace_back(XMFLOAT3{ 30,40,30 });
				pointsList.emplace_back(XMFLOAT3{ 0,30,60 });
				pointsList.emplace_back(XMFLOAT3{ -30,25,30 });
				pointsList.emplace_back(XMFLOAT3{ 0, 20,-20 });
				pointsList.emplace_back(XMFLOAT3{ 0, 20,-20 });
			}
			spline = new Spline();
			spline->Init(pointsList, static_cast<int>(pointsList.size()));
		}
	}
}
/*CharaStateのState並び順に合わせる*/
void (CameraWork::* CameraWork::stateTable[])() = {
	&CameraWork::DefaultCam,//通常
	&CameraWork::SpecialUpdate,//ロード
	&CameraWork::BossAppear,//ボス登場
	&CameraWork::SetBossDead_Before,//ボスのやられたとき
	&CameraWork::SetBossDead_AfterFirst,//1ボスのやられたとき（フェード後）
	&CameraWork::SetBossDead_AfterSecond,//2ボスのやられたとき（フェード後）
	&CameraWork::SetBossDead_AfterThird,//3ボスのやられた時(フェード後)
	&CameraWork::SetBossDead_AfterFourth,//4ボスのやられたとき（フェード後）
	&CameraWork::SetBossDead_AfterFive,//5ボスのやられたとき（フェード後）
	&CameraWork::SetBossDead_AfterSix,//6ボスのやられたとき（フェード後）
	&CameraWork::SetBossDead_AfterSeven,//7ボスのやられたとき（フェード後）
	&CameraWork::StrongCamera,//ボス覚醒カメラ
	&CameraWork::TitleCamera,//タイトルカメラ
};
//XV
void CameraWork::Update(DebugCamera* camera) {
	//状態移行(charastateに合わせる)

	(this->*stateTable[m_CameraState])();
	camera->SetEye(m_eyePos);
	camera->SetTarget(m_targetPos);
	camera->Update();
}
void CameraWork::DefUpda(DebugCamera* camera) {
	//状態移行(charastateに合わせる)
	camera->SetEye(m_eyePos);
	camera->SetTarget(m_targetPos);
	camera->Update();
}
//通常のカメラ
void CameraWork::DefaultCam() {
	if (FeedF) {
		feed->FeedIn(Feed::FeedType::WHITE, 1.0f/30.0f, FeedF);
		if (feed->GetFeedEnd()) {
			AppearEndF = true;
			m_CameraSkip = false;
		}
	}
	m_eyePos.x = Player::GetInstance()->GetPosition().x;
	m_eyePos.y = Player::GetInstance()->GetPosition().y + 50.0f;

	m_eyePos.z = Player::GetInstance()->GetPosition().z - 20.0f;
	m_targetPos.x = Player::GetInstance()->GetPosition().x;
	m_targetPos.y = 3.0f;
	m_targetPos.z = Player::GetInstance()->GetPosition().z;

}
//ボス登場
void CameraWork::BossAppear() {
	//シーンによってカメラの関数が違う
	if (SceneName == "FIRSTSTAGE") {
		FirstBossAppear();
	} else if (SceneName == "SECONDSTAGE") {
		SecondBossAppear();
	} else if (SceneName == "THIRDSTAGE") {
		ThirdBossAppear();
	} else if (SceneName == "FOURTHSTAGE") {
		FourthBossAppear();
	} else if (SceneName == "FIVESTAGE") {
		FiveBossAppear();
	} else if (SceneName == "SIXSTAGE") {
		SixBossAppear();
	} else if (SceneName == "SEVENSTAGE") {
		SevenBossAppear();
	} else {
		assert(0);
	}
	if (Input::GetInstance()->TriggerButton(Input::A)) {
		m_CameraSkip = true;
	}
	if (m_CameraSkip) {
		if (!FeedF) {
			FeedF = true;
		}
	}
	if (FeedF) {
		feed->FeedIn(Feed::FeedType::WHITE, 1.0f/30.0f, FeedF);
		if (feed->GetFeedEnd()) {
			AppearEndF = true;
			m_CameraSkip = false;
		}
	}

}
//ボス撃破
void CameraWork::SetBossDead_Before() {
	DeathTimer++;
	if (DeathTimer == 1) {

		RadEffect = 20;
		m_eyePos.x = boss->GetPosition().x - 10.0f;
		m_eyePos.z = boss->GetPosition().z - 20.f;
		m_eyePos.y = 20.f;
	}

	if (DeathTimer == 100) {
		m_eyePos.x = boss->GetPosition().x + 10.0f;
		m_eyePos.z = boss->GetPosition().z - 20.f;
		m_eyePos.y = 20.f;
	}

	if (DeathTimer == 200) {
		m_eyePos.x = boss->GetPosition().x;
		m_eyePos.z = boss->GetPosition().z - 20.f;
		m_eyePos.y = 20.f;
	}

	m_eyePos.z += 0.08f;

	if (!FeedF) {
		FeedF = true;
	}

	if (FeedF) {
		//DeathTimer = 0;
		feed->FeedIn(Feed::FeedType::WHITE, 0.004f, FeedF);
		if (feed->GetFeedEnd()) {
			FeedEndF = true;
			ParticleEmitter::GetInstance()->AllDelete();
			DeathTimer = 0;
		}
	}


	m_targetPos.x = boss->GetPosition().x;
	m_targetPos.y = boss->GetPosition().y;
	m_targetPos.z = boss->GetPosition().z;
}
//フェード後の撃破アクション(1ボス)
void CameraWork::SetBossDead_AfterFirst() {
	RadEffect = 0.f;
	if (FeedF) {
		feed->FeedIn(Feed::FeedType::WHITE, 1.0f / 30.0f, FeedF);
	}
	if (!m_LookPlayer) {
		m_eyePos.x = 0.0f;
		m_eyePos.y = Player::GetInstance()->GetPosition().y + 20.0f;
		m_eyePos.z = Player::GetInstance()->GetPosition().z + 10.0f;


		m_targetPos = { boss->GetPosition().x,boss->GetPosition().y,boss->GetPosition().z };

		DeathTimer++;
		if (DeathTimer == 160) {
			m_LookPlayer = true;
			m_AfterEye = { Player::GetInstance()->GetPosition().x - 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_AfterTarget = { Player::GetInstance()->GetPosition().x + 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_Frame = {};
		}
	}
	else {
		if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
			if (DeathTimer == 161) {
				ClearText::GetInstance()->SetAlive(true);
			}
			DeathTimer++;

			if (DeathTimer == 210) {
				m_EndDeath = true;
			}
		}

		SetEaseCamera();
	}
}
//フェード後の撃破アクション(2ボス)
void CameraWork::SetBossDead_AfterSecond() {
	RadEffect = 0;
	if (!m_LookPlayer) {
		m_eyePos = {
		boss->GetPosition().x,
		Player::GetInstance()->GetPosition().y + 5.f ,
		boss->GetPosition().z - 25.f
		};
		m_targetPos.x = boss->GetPosition().x;
		m_targetPos.z = boss->GetPosition().z;

		if (boss->GetPosition().y <= 0.f) {
			m_LookPlayer = true;
			m_AfterEye = { Player::GetInstance()->GetPosition().x - 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_AfterTarget = { Player::GetInstance()->GetPosition().x + 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_Frame = {};
		}
	}
	else {
		if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
			DeathTimer++;
			if (DeathTimer == 1) {
				ClearText::GetInstance()->SetAlive(true);
			}
			if (DeathTimer == 50) {
				m_EndDeath = true;
			}
		}

		SetEaseCamera();
	}
	FeedF = false;
}
//フェード後の撃破アクション(3ボス)
void CameraWork::SetBossDead_AfterThird() {
	RadEffect = 0.f;
	if (FeedF) {
		feed->FeedIn(Feed::FeedType::WHITE, 1.0f/30.0f, FeedF);
	}

	if (!m_LookPlayer) {
		m_eyePos.x = 0.0f;
		m_eyePos.y = Player::GetInstance()->GetPosition().y + 10.0f;
		m_eyePos.z = Player::GetInstance()->GetPosition().z + 10.0f;


		m_targetPos = { boss->GetPosition().x,boss->GetPosition().y,boss->GetPosition().z };

		DeathTimer++;
		if (DeathTimer == 250) {
			m_LookPlayer = true;
			m_AfterEye = { Player::GetInstance()->GetPosition().x - 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_AfterTarget = { Player::GetInstance()->GetPosition().x + 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_Frame = {};
		}
	}
	else {
		if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
			DeathTimer++;
			if (DeathTimer == 251) {
				ClearText::GetInstance()->SetAlive(true);
			}
			if (DeathTimer == 300) {
				m_EndDeath = true;
			}
		}

		SetEaseCamera();
	}
}
//フェード後の撃破アクション(4ボス)
void CameraWork::SetBossDead_AfterFourth() {
	RadEffect = 0.f;
	if (FeedF) {
		feed->FeedIn(Feed::FeedType::WHITE, 1.0f/30.0f, FeedF);
	}

	if (!m_LookPlayer) {
		m_eyePos.x = 0.0f;
		m_eyePos.y = Player::GetInstance()->GetPosition().y + 3.0f;
		m_eyePos.z = -30.0f;


		m_targetPos = { boss->GetPosition().x,boss->GetPosition().y,boss->GetPosition().z };

		DeathTimer++;

		if (DeathTimer == 170) {
			m_LookPlayer = true;
			m_AfterEye = { Player::GetInstance()->GetPosition().x - 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_AfterTarget = { Player::GetInstance()->GetPosition().x + 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_Frame = {};
		}
	}
	else {
		if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
			DeathTimer++;
			if (DeathTimer == 171) {
				ClearText::GetInstance()->SetAlive(true);
			}
			if (DeathTimer == 220) {
				m_EndDeath = true;
			}
		}

		SetEaseCamera();
	}
}
//フェード後の撃破アクション(5ボス)
void CameraWork::SetBossDead_AfterFive() {
	RadEffect = 0.f;
	if (FeedF) {
		feed->FeedIn(Feed::FeedType::WHITE, 1.0f/30.0f, FeedF);
	}

	if (!m_LookPlayer) {
		DeathTimer++;
		m_eyePos.x = 0.0f;
		m_eyePos.y = Player::GetInstance()->GetPosition().y + 3.0f;
		m_eyePos.z = -20.0f;


		m_targetPos = { boss->GetPosition().x,boss->GetPosition().y,boss->GetPosition().z };
		if (DeathTimer == 200) {
			m_LookPlayer = true;
			m_AfterEye = { Player::GetInstance()->GetPosition().x - 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_AfterTarget = { Player::GetInstance()->GetPosition().x + 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_Frame = {};
		}
	}
	else {
		if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
			if (DeathTimer == 201) {
				ClearText::GetInstance()->SetAlive(true);
			}
			DeathTimer++;

			if (DeathTimer == 250) {
				m_EndDeath = true;
			}
		}

		SetEaseCamera();
	}

}
//フェード後の撃破アクション(6ボス)
void CameraWork::SetBossDead_AfterSix() {
	RadEffect = 0.f;
	if (FeedF) {
		feed->FeedIn(Feed::FeedType::WHITE, 1.0f/30.0f, FeedF);
	}
	if (!m_LookPlayer) {
		m_eyePos.x = 0.0f;
		m_eyePos.y = Player::GetInstance()->GetPosition().y + 20.0f;
		m_eyePos.z = Player::GetInstance()->GetPosition().z + 10.0f;


		m_targetPos = { boss->GetPosition().x,boss->GetPosition().y,boss->GetPosition().z };

		DeathTimer++;
		if (DeathTimer == 350) {
			m_LookPlayer = true;
			m_AfterEye = { Player::GetInstance()->GetPosition().x - 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_AfterTarget = { Player::GetInstance()->GetPosition().x + 15.0f,8.0f,Player::GetInstance()->GetPosition().z };
			m_Frame = {};
		}
	}
	else {
		if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
			if (DeathTimer == 351) {
				ClearText::GetInstance()->SetAlive(true);
			}
			DeathTimer++;

			if (DeathTimer == 400) {
				m_EndDeath = true;
			}
		}

		SetEaseCamera();
	}
}
//フェード後の撃破アクション(7ボス)
void CameraWork::SetBossDead_AfterSeven() {
	const float l_AddFrame = 0.01f;
	RadEffect = 0.f;
	if (FeedF) {
		feed->FeedIn(Feed::FeedType::WHITE, 1.0f/30.0f, FeedF);
	}
	
	//カメラ位置固定
	if (_DeathCamera == DEATH_SET) {
		m_eyePos = { -10.0f,5.0f,-5.0f };
		m_targetPos = { 0.0f,5.0f,20.0f };
		if (m_EndTimer == 150) {
			_DeathCamera = DEATH_BOSS_UP;
			m_Frame = {};
			m_AfterEye = { -3.0f,5.0f,10.0f };
			m_AfterTarget = { 0.0f,5.0f,30.0f };
		}
	}
	else if (_DeathCamera == DEATH_BOSS_UP) {			//ボスの前に持ってくる
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}

		if (m_EndTimer == 420) {				//プレイヤーを見る
			m_AfterEye = { 0.0f,5.0f,10.0f };
			m_AfterTarget = { 0.0f,5.0f,-30.0f };
			_DeathCamera = DEATH_PLAYER;
			m_Frame = {};
		}
		SetEaseCamera();
	}
	else if(_DeathCamera == DEATH_PLAYER) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}
		if (m_EndTimer == 730) {
			m_Frame = {};
			m_AfterEye = { -15.0f,5.0f,12.0f };
			m_AfterTarget = { 0.0f,5.0f,12.0f };
			_DeathCamera = DEATH_LOOK_BOSS;
		}
		SetEaseCamera();
	}
	else if(_DeathCamera == DEATH_LOOK_BOSS) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}
		SetEaseCamera();
		if (m_EndTimer == 1800) {
			m_Frame = {};
			_DeathCamera = DEATH_END;
			m_AfterEye = { -300.0f,5.0f,12.0f };
			m_AfterTarget = { 0.0f,5.0f,12.0f };
		}
	}
	else {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}

		SetEaseCamera();
		if (m_Frame > 0.02f) {
			m_EndDeath = true;
		}
	}
}
//エディタのカメラ
void CameraWork::EditorCamera() {
	m_eyePos.y = 35.f;
	m_targetPos.z = m_eyePos.z + 30.0f;
	m_targetPos.x = m_eyePos.x;
}
//ImGui
void CameraWork::ImGuiDraw() {
	ImGui::Begin("Camera");
	ImGui::Text("Timer:%d", DeathTimer);
	ImGui::End();
}
void CameraWork::SpecialUpdate() {

}
void CameraWork::feedDraw() {
	if (FeedF || Feed_Spline)
		feed->Draw();
}
//最初のボスのカメラ
void CameraWork::FirstBossAppear() {
	
	XMVECTOR move = { 0.f,0.f, 0.1f, 0.0f };
	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(boss->GetRotation().y + 60));
	move = XMVector3TransformNormal(move, matRot);
		float l_AddFrame = 0.0f;
	if (m_AppearType == APPEAR_START) {
		m_CameraTimer++;
		m_AfterSpeed = m_CameraSpeed;
		m_targetPos.x = 0.0f;
		m_eyePos = {
			Player::GetInstance()->GetPosition().x,
			0.0f,
			Player::GetInstance()->GetPosition().z + 23.0f,
		};

		if (m_CameraTimer == 10) {
			m_AfterSpeed = 30.0f;
			m_AppearType = APPEAR_FIVE;
		}

		SetCircleCameraTarget();
	}
	
	//ボスの後ろにいる
	else if (m_AppearType == APPEAR_FIVE) {
		m_targetPos = boss->GetPosition();
		m_eyePos = { boss->GetPosition().x - 10.0f,boss->GetPosition().y,boss->GetPosition().z + 20.0f };
		m_CameraTimer++;

		if (m_CameraTimer == 30) {
			m_AfterEye = { boss->GetPosition().x - 5.0f, boss->GetPosition().y, boss->GetPosition().z - 40.0f };
			m_AppearType = APPEAR_SIX;
			m_Frame = {};
			m_CameraTimer = {};
		}
	}
	//プレイヤーの右後ろに来る
	else if (m_AppearType == APPEAR_SIX) {
		l_AddFrame = 0.01f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 0.0f;
			m_AppearType = APPEAR_SEVEN;
		}

		m_eyePos = {
			Ease(In,Cubic,m_Frame,m_eyePos.x,m_AfterEye.x),
			Ease(In,Cubic,m_Frame,m_eyePos.y,m_AfterEye.y),
			Ease(In,Cubic,m_Frame,m_eyePos.z,m_AfterEye.z),
		};
	}
	//カメラを停止させる
	else if (m_AppearType == APPEAR_SEVEN) {
		//カメラが寄るフラグになったら次のシーン移行
		if (m_Approach) {
			m_AfterEye = { boss->GetPosition().x,boss->GetPosition().y,boss->GetPosition().z - 20.0f };
			m_AfterTarget = { boss->GetPosition().x,boss->GetPosition().y,boss->GetPosition().z };
			m_Frame = {};
			m_CameraTimer = {};
			m_AppearType = APPEAR_EIGHT;
			m_Approach = false;
		}
	}
	//カメラをボスの前に
	else if (m_AppearType == APPEAR_EIGHT) {
		l_AddFrame = 0.05f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			//一定時間でカメラを戻す
			if (Helper::GetInstance()->CheckMin(m_CameraTimer, 100, 1)) {
				m_AfterEye = { Player::GetInstance()->GetPosition().x,45.0f,Player::GetInstance()->GetPosition().z - 20.0f };
				m_AfterTarget = { Player::GetInstance()->GetPosition().x,5.0f,Player::GetInstance()->GetPosition().z };
				m_Frame = {};
				m_CameraTimer = {};
				m_AppearType = APPEAR_END;
				boss->SetFinishApp(true);
			}
		}
		m_eyePos = {
		Ease(In,Cubic,m_Frame,m_eyePos.x,m_AfterEye.x),
		Ease(In,Cubic,m_Frame,m_eyePos.y,m_AfterEye.y),
		Ease(In,Cubic,m_Frame,m_eyePos.z,m_AfterEye.z),
		};

		m_targetPos = {
		Ease(In,Cubic,m_Frame,m_targetPos.x,m_AfterTarget.x),
		Ease(In,Cubic,m_Frame,m_targetPos.y,m_AfterTarget.y),
		Ease(In,Cubic,m_Frame,m_targetPos.z,m_AfterTarget.z),
		};
	}
	//バトル前のカメラに戻る
	else if (m_AppearType == APPEAR_END) {
		l_AddFrame = 0.05f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}

		m_eyePos = {
		Ease(In,Cubic,m_Frame,m_eyePos.x,m_AfterEye.x),
		Ease(In,Cubic,m_Frame,m_eyePos.y,m_AfterEye.y),
		Ease(In,Cubic,m_Frame,m_eyePos.z,m_AfterEye.z),
		};

		m_targetPos = {
		Ease(In,Cubic,m_Frame,m_targetPos.x,m_AfterTarget.x),
		Ease(In,Cubic,m_Frame,m_targetPos.y,m_AfterTarget.y),
		Ease(In,Cubic,m_Frame,m_targetPos.z,m_AfterTarget.z),
		};
	}
}
void CameraWork::FirstBossDead_AfterFeed() {

}
//2個目のボスのカメラ
void CameraWork::SecondBossAppear() {

	XMVECTOR move = { 0.f,0.f, 0.1f, 0.0f };
	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(boss->GetRotation().y + 60));
	move = XMVector3TransformNormal(move, matRot);

	if (spline->GetIndex() >= pointsList.size() - 2) {
		RadEffect -= 0.2f;
	} else if (spline->GetIndex() >= pointsList.size()) {
		RadEffect += 0.2f;
		SplineSpeed = 400.f;
	} else {
		SplineSpeed = 300.f;
	}
	if (!Finish) {

		spline->Upda(m_eyePos, SplineSpeed);
	}
	Helper::GetInstance()->Clamp(RadEffect, 0.f, 15.f);

	if (spline->GetIndex() >= pointsList.size() - 1) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
			AppearEndF = true;
			m_CameraState = CAMERA_NORMAL;
			m_Frame = 1.0f;
		}
		m_AfterEye = { Player::GetInstance()->GetPosition().x,45.0f,Player::GetInstance()->GetPosition().z - 20.0f };
		m_AfterTarget = Player::GetInstance()->GetPosition();
		m_targetPos = {
Ease(In,Cubic,m_Frame,boss->GetPosition().x,m_AfterTarget.x),
Ease(In,Cubic,m_Frame,boss->GetPosition().y,m_AfterTarget.y),
Ease(In,Cubic,m_Frame,boss->GetPosition().z,m_AfterTarget.z),
		};

		m_eyePos = {
Ease(In,Cubic,m_Frame,m_eyePos.x,m_AfterEye.x),
Ease(In,Cubic,m_Frame,m_eyePos.y,m_AfterEye.y),
Ease(In,Cubic,m_Frame,m_eyePos.z,m_AfterEye.z),
		};

		Finish = true;
	} else {
		m_targetPos = { boss->GetPosition() };
	}
}
//3つ目のボス
void CameraWork::ThirdBossAppear() {
	float l_AddFrame = 0.0f;
	if (m_AppearType == APPEAR_START) {
		m_CameraTimer++;
		m_AfterSpeed = m_CameraSpeed;
		m_targetPos.x = 0.0f;
		m_eyePos = {
			Player::GetInstance()->GetPosition().x,
			0.0f,
			15.0f,
		};

		if (m_CameraTimer == 10) {
			m_AfterSpeed = 30.0f;
			m_AppearType = APPEAR_SECOND;
		}

		SetCircleCameraTarget();
	}
	//右を見る
	else if (m_AppearType == APPEAR_SECOND) {
		l_AddFrame = 0.01f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_AfterSpeed = 150.0f;
			m_AppearType = APPEAR_THIRD;
			m_Frame = 0.0f;
		}

		m_CameraSpeed = Ease(In, Cubic, m_Frame, m_CameraSpeed, m_AfterSpeed);

		SetCircleCameraTarget();
	}
	//左を見る
	else if (m_AppearType == APPEAR_THIRD) {
		l_AddFrame = 0.01f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_AfterTarget = { boss->GetPosition().x,20.0f,boss->GetPosition().z };
			m_AppearType = APPEAR_FOURTH;
			m_Frame = 0.0f;
			m_CameraTimer = 0;
		}

		m_CameraSpeed = Ease(In, Cubic, m_Frame, m_CameraSpeed, m_AfterSpeed);

		SetCircleCameraTarget();
	}
	//上を見る
	else if (m_AppearType == APPEAR_FOURTH) {
		l_AddFrame = 0.01f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_AppearType = APPEAR_FIVE;
			m_Frame = 0.0f;
			m_CameraTimer = {};
		}

		m_CameraSpeed = Ease(In, Cubic, m_Frame, m_CameraSpeed, m_AfterSpeed);
		m_CameraScale = Ease(In, Cubic, m_Frame, m_CameraScale, m_AfterScale);
		m_targetPos = { Ease(In, Cubic, m_Frame, m_targetPos.x, m_AfterTarget.x),
			Ease(In, Cubic, m_Frame, m_targetPos.y, m_AfterTarget.y),
			Ease(In, Cubic, m_Frame, m_targetPos.z, m_AfterTarget.z),
		};
	}
	//ボスの後ろにいる
	else if (m_AppearType == APPEAR_FIVE) {
		m_targetPos = boss->GetPosition();
		m_eyePos = { boss->GetPosition().x - 10.0f,boss->GetPosition().y,boss->GetPosition().z + 20.0f };
		m_CameraTimer++;

		if (m_CameraTimer == 30) {
			m_AfterEye = { boss->GetPosition().x - 5.0f, boss->GetPosition().y, boss->GetPosition().z - 40.0f };
			m_AppearType = APPEAR_SIX;
			m_Frame = {};
			m_CameraTimer = {};
		}
	}
	//プレイヤーの右後ろに来る
	else if (m_AppearType == APPEAR_SIX) {
		l_AddFrame = 0.01f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 0.0f;
			m_AppearType = APPEAR_SEVEN;
		}

		m_eyePos = {
			Ease(In,Cubic,m_Frame,m_eyePos.x,m_AfterEye.x),
			Ease(In,Cubic,m_Frame,m_eyePos.y,m_AfterEye.y),
			Ease(In,Cubic,m_Frame,m_eyePos.z,m_AfterEye.z),
		};
	}
	//カメラを停止させる
	else if (m_AppearType == APPEAR_SEVEN) {
		//カメラが寄るフラグになったら次のシーン移行
		if (m_Approach) {
			m_AfterEye = { boss->GetPosition().x,boss->GetPosition().y,boss->GetPosition().z - 20.0f };
			m_AfterTarget = { boss->GetPosition().x,boss->GetPosition().y,boss->GetPosition().z };
			m_Frame = {};
			m_CameraTimer = {};
			m_AppearType = APPEAR_EIGHT;
			m_Approach = false;
		}
	}
	//カメラをボスの前に
	else if (m_AppearType == APPEAR_EIGHT) {
		l_AddFrame = 0.05f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			//一定時間でカメラを戻す
			if (Helper::GetInstance()->CheckMin(m_CameraTimer, 100, 1)) {
				m_AfterEye = { Player::GetInstance()->GetPosition().x,45.0f,Player::GetInstance()->GetPosition().z - 20.0f };
				m_AfterTarget = { Player::GetInstance()->GetPosition().x,5.0f,Player::GetInstance()->GetPosition().z };
				m_Frame = {};
				m_CameraTimer = {};
				m_AppearType = APPEAR_END;
				boss->SetFinishApp(true);
				m_CameraSkip = true;
			}
		}
		m_eyePos = {
		Ease(In,Cubic,m_Frame,m_eyePos.x,m_AfterEye.x),
		Ease(In,Cubic,m_Frame,m_eyePos.y,m_AfterEye.y),
		Ease(In,Cubic,m_Frame,m_eyePos.z,m_AfterEye.z),
		};

		m_targetPos = {
		Ease(In,Cubic,m_Frame,m_targetPos.x,m_AfterTarget.x),
		Ease(In,Cubic,m_Frame,m_targetPos.y,m_AfterTarget.y),
		Ease(In,Cubic,m_Frame,m_targetPos.z,m_AfterTarget.z),
		};
	}
	//バトル前のカメラに戻る
	else if (m_AppearType == APPEAR_END) {
		l_AddFrame = 0.05f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}

		m_eyePos = {
		Ease(In,Cubic,m_Frame,m_eyePos.x,m_AfterEye.x),
		Ease(In,Cubic,m_Frame,m_eyePos.y,m_AfterEye.y),
		Ease(In,Cubic,m_Frame,m_eyePos.z,m_AfterEye.z),
		};

		m_targetPos = {
		Ease(In,Cubic,m_Frame,m_targetPos.x,m_AfterTarget.x),
		Ease(In,Cubic,m_Frame,m_targetPos.y,m_AfterTarget.y),
		Ease(In,Cubic,m_Frame,m_targetPos.z,m_AfterTarget.z),
		};
	}
}
//4つ目のボスの登場
void CameraWork::FourthBossAppear() {
	m_Frame += 1 / m_FrameMax;
	Helper::GetInstance()->Clamp(m_Frame, 0.0f, 1.0f);
	switch (m_AppearType) {
	case APPEAR_START:
		m_FrameMax = 90.0f;
		m_eyePos = {
		Player::GetInstance()->GetPosition().x,
		3.0f,
		Player::GetInstance()->GetPosition().z,
		};
		m_BeforeEye = m_eyePos;
		m_AfterEye = m_eyePos;
		break;
	case APPEAR_SECOND:
		m_FrameMax = 90.0f;
		m_AfterTarget = {
		30,
		5,
		-30
		};
		break;
	case APPEAR_THIRD:
		m_FrameMax = 90.0f;
		m_AfterTarget = {
		0,
		5,
		0
		};
		break;
	case APPEAR_FOURTH:
		m_FrameMax = 90.0f;
		m_AfterTarget = {
		-30,
		5,
		-30
		};
		break;
	case APPEAR_FIVE:
		m_FrameMax = 90.0f;
		m_AfterTarget = {
		0,
		5,
		0
		};
		break;
	case APPEAR_SIX:
		break;
	case APPEAR_SEVEN:
		break;
	case APPEAR_EIGHT:
		break;
	case APPEAR_END:
		break;
	default:
		break;
	}
	if (m_Frame == 1.0f) {
		m_AppearType++;
		m_BeforeEye = m_eyePos;
		m_BeforeTarget = m_targetPos;
		m_Frame = 0.0f;
	}

	m_eyePos = {
	Ease(In,Cubic,m_Frame,m_BeforeEye.x,m_AfterEye.x),
	Ease(In,Cubic,m_Frame,m_BeforeEye.y,m_AfterEye.y),
	Ease(In,Cubic,m_Frame,m_BeforeEye.z,m_AfterEye.z),
	};

	m_targetPos = {
	Ease(In,Cubic,m_Frame,m_BeforeTarget.x,m_AfterTarget.x),
	Ease(In,Cubic,m_Frame,m_BeforeTarget.y,m_AfterTarget.y),
	Ease(In,Cubic,m_Frame,m_BeforeTarget.z,m_AfterTarget.z),
	};


}
//5個目のボス
void CameraWork::FiveBossAppear() {
	float l_AddFrame = 0.01f;
	if (m_AppearType == APPEAR_START) {
		if (m_LastTimer == 1) {
			m_eyePos = { -3.0f,5.0f,20.0f };
			m_targetPos = { 0.0f,5.0f,0.0f };
		}

		if (m_LastTimer == 150) {
			m_Frame = {};
			m_AppearType = APPEAR_SECOND;
			m_AfterTarget = { 0.0f,5.0f,boss->GetPosition().z };
			m_AfterEye = { 0.0f,6.0f,5.0f };
		}
	}
	else if (m_AppearType == APPEAR_SECOND) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}

		if (m_LastTimer == 620) {
			m_Frame = {};
			m_AfterTarget = { 0.0f,5.0f,12.0f };
			m_AfterEye = { -25.0f,5.0f,12.0f };
			m_AppearType = APPEAR_THIRD;
		}
		SetEaseCamera();
	}
	else if (m_AppearType == APPEAR_THIRD) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}
		if (m_LastTimer == 750) {
			m_Frame = 0.0f;
			m_AfterTarget = { 0.0f,m_targetPos.y,-4.0f };
			m_AppearType = APPEAR_FOURTH;
		}
		SetEaseCamera();
	}
	else if (m_AppearType == APPEAR_FOURTH) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}
		if (m_LastTimer == 900) {
			m_Frame = 0.0f;
			m_AfterEye = m_eyePos;
			m_AfterTarget = { 0.0f,m_targetPos.y,28.0f };
			m_AppearType = APPEAR_FIVE;
		}

		SetEaseCamera();
	}
	else if (m_AppearType == APPEAR_FIVE) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}
		if (m_LastTimer == 1050) {
			m_Frame = 0.0f;
			m_AfterEye = { -300.0f,5.0f,12.0f };
			m_AfterTarget = { 0.0f,5.0f,12.0f };
			m_AppearType = APPEAR_SIX;
		}

		SetEaseCamera();
	}
	else if (m_AppearType == APPEAR_SIX) {
		l_AddFrame = 0.005f;
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}
		SetEaseCamera();

		if (m_Frame > 0.005f) {
			m_CameraSkip = true;
		}
	}
}
//6個目のボス
void CameraWork::SixBossAppear() {
	if (spline->GetIndex() >= pointsList.size() - 2) {
		RadEffect -= 0.2f;
	} else if (spline->GetIndex() >= pointsList.size()) {
		RadEffect += 0.2f;
		SplineSpeed = 180.0f;
	} else {
		SplineSpeed = 180.f;
	}
	if (!Finish) {
		spline->Upda(m_eyePos, SplineSpeed);
	}
	Helper::GetInstance()->Clamp(RadEffect, 0.f, 15.f);

	if (spline->GetIndex() >= pointsList.size() - 1) {
		m_CameraSkip = true;
		if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
			AppearEndF = true;
			m_CameraState = CAMERA_NORMAL;
			m_Frame = 1.0f;
		}
		m_AfterEye = { Player::GetInstance()->GetPosition().x,45.0f,Player::GetInstance()->GetPosition().z - 20.0f };
		m_AfterTarget = Player::GetInstance()->GetPosition();
		m_targetPos = {
Ease(In,Cubic,m_Frame,boss->GetPosition().x,m_AfterTarget.x),
Ease(In,Cubic,m_Frame,boss->GetPosition().y,m_AfterTarget.y),
Ease(In,Cubic,m_Frame,boss->GetPosition().z,m_AfterTarget.z),
		};

		m_eyePos = {
Ease(In,Cubic,m_Frame,m_eyePos.x,m_AfterEye.x),
Ease(In,Cubic,m_Frame,m_eyePos.y,m_AfterEye.y),
Ease(In,Cubic,m_Frame,m_eyePos.z,m_AfterEye.z),
		};

		Finish = true;
	} else {
		m_targetPos = { boss->GetPosition() };
	}
}
//7個目のボス
void CameraWork::SevenBossAppear() {
	const float l_AddFrame = 0.01f;
	if (_LastState == LAST_SET) {
		if (m_LastTimer == 1) {
			m_eyePos = { 0.0f,5.0f,20.0f };
			m_targetPos = { 0.0f,5.0f,0.0f };
		}

		if (m_LastTimer == 150) {
			m_Frame = {};
			_LastState = LAST_BOSS;
			m_AfterTarget = { boss->GetPosition().x,5.0f,boss->GetPosition().z };
			m_AfterEye = { -10.0f,3.0f,-5.0f };
		}
	}
	else if (_LastState == LAST_BOSS) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
			if (m_LastTimer == 300) {
				m_AfterTarget = { 0.0f,3.0f,10.0f };
				m_AfterEye = { -5.0f,3.0f,15.0f };
				m_Frame = {};
				_LastState = LAST_PLAYER;
			}
			else if (m_LastTimer == 900) {
				m_AfterTarget = { boss->GetPosition().x,5.0f,boss->GetPosition().z };
				m_AfterEye = { .0f,6.0f,5.0f };
				m_Frame = {};
				_LastState = LAST_UPBOSS;
			}
		}
		SetEaseCamera();
	}
	else if (_LastState == LAST_PLAYER) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
			if (m_LastTimer == 600) {
				m_Frame = {};
				_LastState = LAST_BOSS;
				m_AfterTarget = { boss->GetPosition().x,5.0f,boss->GetPosition().z };
				m_AfterEye = { -10.0f,3.0f,-5.0f };
			}
			else if (m_LastTimer == 2100) {
				m_Frame = {};
				_LastState = LAST_BATTLE;
				m_AfterTarget = { 0.0f,5.0f,20.0f };
				m_AfterEye = { -15.0f,5.0f,20.0f };
			}
		}
		SetEaseCamera();
	}
	else if (_LastState == LAST_UPBOSS) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
			if (m_LastTimer == 1050) {
				m_AfterTarget = { boss->GetPosition().x,5.0f,boss->GetPosition().z };
				m_AfterEye = { 0.0f,6.0f,15.0f };
				m_Frame = {};
				_LastState = LAST_ZOOMBOSS;
			}
		}
		SetEaseCamera();
	}
	else if (_LastState == LAST_ZOOMBOSS) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
			if (m_LastTimer == 1250) {
				m_AfterTarget = { boss->GetPosition().x,5.0f,boss->GetPosition().z };
				m_AfterEye = { .0f,6.0f,0.0f };
				m_Frame = {};
				_LastState = LAST_FARBOSS;
			}
			else if (m_LastTimer == 1800) {
				m_AfterTarget = { 0.0f,3.0f,10.0f };
				m_AfterEye = { -5.0f,3.0f,15.0f };
				m_Frame = {};
				_LastState = LAST_PLAYER;
			}
		}
		SetEaseCamera();
	}
	else if (_LastState == LAST_FARBOSS) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
			if (m_LastTimer == 1380) {
				m_AfterTarget = { boss->GetPosition().x,5.0f,boss->GetPosition().z };
				m_AfterEye = { 0.0f,6.0f,15.0f };
				m_Frame = {};
				_LastState = LAST_ZOOMBOSS;
			}
		}
		SetEaseCamera();
	}
	else if (_LastState == LAST_BATTLE) {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
			if (m_LastTimer == 2700) {
				m_Frame = {};
				_LastState = LAST_BATTLE2;
				m_AfterTarget = { 0.0f,5.0f,20.0f };
				m_AfterEye = { -10.0f,5.0f,20.0f };
			}
		}
		SetEaseCamera();
	}
	else {
		if (Helper::GetInstance()->FrameCheck(m_Frame, l_AddFrame)) {
			m_Frame = 1.0f;
		}
		SetEaseCamera();
	}
}
//円運動の際のカメラ位置更新
void CameraWork::SetCircleCameraTarget() {
	//円運動の計算
	m_CameraRadius = m_CameraSpeed * m_PI / 180.0f;
	m_CameraCircleX = cosf(m_CameraRadius) * m_CameraScale;
	m_CameraCircleZ = sinf(m_CameraRadius) * m_CameraScale;
	m_targetPos.x = m_CameraCircleX;
	m_targetPos.z = m_CameraCircleZ;
}
//円運動の際のカメラ位置更新
void CameraWork::SetCircleCameraEye(const XMFLOAT3& target, const XMFLOAT3& basepos) {
	//円運動の計算
	m_CameraRadius = m_CameraSpeed * m_PI / 180.0f;
	m_CameraCircleX = cosf(m_CameraRadius) * m_CameraScale;
	m_CameraCircleZ = sinf(m_CameraRadius) * m_CameraScale;
	m_eyePos.x = m_CameraCircleX + basepos.x;
	m_eyePos.z = m_CameraCircleZ + basepos.z;
	m_targetPos = target;
}
//覚醒時のカメラ
void CameraWork::StrongCamera() {
	float l_AddFrame = 0.01f;
	const int l_Timer = 200;
	//フェード後は円運動をする
	if (_StrongState == STRONG_ONE) {
		m_Frame = {};
		m_CameraScale = 20.0f;
		m_CameraSpeed = 0.0f;
		if (FeedF) {
			feed->FeedIn(Feed::FeedType::WHITE, 0.025f, FeedF);
			if (feed->GetAlpha() >= 1.0f) {
				_StrongState = STRONG_SECOND;
				m_ChangeStrong = true;
			}
		}
	}
	else if (_StrongState == STRONG_SECOND) {
		if (feed->GetAlpha() == 0.0f) {
			m_StrongTimer++;
		}

		if (m_StrongTimer == 10) {
			FeedEndF = false;
		}
		m_targetPos = boss->GetPosition();
		SplineSpeed = 180.0f;
		spline->Upda(m_eyePos, SplineSpeed);
		if (!Finish) {

			spline->Upda(m_eyePos, SplineSpeed);
		}

		if (spline->GetIndex() >= pointsList.size() - 1) {

			if (Helper::GetInstance()->FrameCheck(m_Frame, 0.01f)) {
				m_Frame = 1.0f;
				FeedF = true;
				m_Finish = true;
			}
			m_AfterEye = { 0.0f,5.0f, 15.0f};
			m_AfterTarget = boss->GetPosition();
			RadEffect = Ease(In, Cubic, m_Frame, RadEffect, 20.0f);
			SetEaseCamera();
		}

		if (FeedF) {
			feed->FeedIn(Feed::FeedType::WHITE, 0.025f, FeedF);
			if (feed->GetAlpha() >= 1.0f) {
				if (m_Finish) {
					m_Frame = {};
					m_StrongTimer = {};
					m_NearBoss = false;
					RadEffect = 0;
					_StrongState = STRONG_THIRD;
					m_EndStrong = true;
					m_ChangeStrong = false;
				}
			}
		}
	}
}
//タイトルカメラ
void CameraWork::TitleCamera() {
	m_eyePos = { 0.0f,3.0f,-20.0f };
	m_targetPos = { 0.0f,0.0f,10.0f };
}
void CameraWork::SetEaseCamera() {
	m_eyePos = {
Ease(In,Cubic,m_Frame,m_eyePos.x,m_AfterEye.x),
Ease(In,Cubic,m_Frame,m_eyePos.y,m_AfterEye.y),
Ease(In,Cubic,m_Frame,m_eyePos.z,m_AfterEye.z),
	};
	m_targetPos = {
Ease(In,Cubic,m_Frame,m_targetPos.x,m_AfterTarget.x),
Ease(In,Cubic,m_Frame,m_targetPos.y,m_AfterTarget.y),
Ease(In,Cubic,m_Frame,m_targetPos.z,m_AfterTarget.z),
	};
}