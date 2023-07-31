#include "SelectScene.h"
#include "Helper.h"
#include "ImageManager.h"
#include "SceneManager.h"
#include "Easing.h"
#include "ParticleEmitter.h"

SelectScene* SelectScene::GetIns() {
	static SelectScene ins;
	return &ins;
}

void SelectScene::ResetParama() {
	ButtonNav_Challenge_Cancel[0]->SetPosition({ 400,650 });
	ButtonNav_Challenge_Cancel[1]->SetPosition({ 200,650 });
	ButtonNav_Pos[0] = { 820,560 };
	ButtonNav_Pos[1] = { 450,560 };
	for (auto i = 0; i < 2; i++) {
		ButtonNav_Challenge_Cancel[i]->SetAnchorPoint({ 0.5f,0.5f });
		ButtonNav_Challenge_CancelScl[i] = { 200,150 };
		ButtonNav_Challenge_CancelColAlpha[i] = 1.f;
	}

	KotokoYPos = -10.f;
	SutoYPos = 0.f;
	//
	constexpr float PosRad = 25.f;
	for (auto i = 0; i < ObjNum; i++) {
		TipsPosY[i] = -360.f;
		StageObjRotAngle[i] = OldObjAngle[i];// static_cast<float>(i)* (360.f / static_cast<float>(ObjNum)) + 180.f;
		//位置の初期化
		StageObjPos[i].x = Pedestal->GetPosition().x + sinf(StageObjRotAngle[i] * (PI / PI_180)) * PosRad;
		StageObjPos[i].z = Pedestal->GetPosition().z + cosf(StageObjRotAngle[i] * (PI / PI_180)) * PosRad;
		StageObjPos[i].y = Pedestal->GetPosition().y + 8.f;
		//BossIcon.
		StageObjRot[i].y = 90;
		StageObjs[i]->SetPosition(StageObjPos[i]);
		StageObjs[i]->SetScale({ 1,1,1 });
	}
	StageObjs[FIRST]->SetScale({ 5,5,5 });
	StageObjs[THIRD]->SetScale({ 4,4,4 });
	StageObjs[FOUR]->SetScale({ 3.f,3.f,3.f });
	StageObjs[FOUR]->SetRotation({ 0.0f,0.0f,0.0f });
	StageObjs[FIVE]->SetScale({ 0.2f,0.2f,0.2f });
	StageObjs[TITLE]->SetScale({ 8.2f,5.2f,8.2f });
	TrigerSelect = NOINP;
	CloseF = false;

	closeScl = 6500.f;
	closeRad = 1500.f;


	for (auto i = 0; i < ObjNum; i++) {
		TipsAct[i] = false;
		TipsPosY[i] = -360.f;
	}

	sin = false;
	_stages = Stage::MAX;

	JudgChal = false;

	SceneSave::GetInstance()->AllClear();
	if (SceneSave::GetInstance()->GetClearFlag(kFirstStage)) {
		SelectScene::GetIns()->SetSelectState(SELECT_SECOND);
	}
}

void SelectScene::Init() {
	Pedestal.reset(new IKEObject3d());
	Pedestal->Initialize();
	Pedestal->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Pedestal));
	Pedestal->SetScale({ 300,20,300 });
	for (auto i = 0; i < ObjNum; i++) {
		StageObjs[i].reset(new IKEObject3d());
		StageObjs[i]->Initialize();
	}
	kotokoObj.reset(new IKEObject3d());
	kotokoObj->Initialize();
	kotokoObj->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::TITKOTOKO));
	KotokoYPos = -7.f;
	SutoYPos = 2.f;

	sutoponObj.reset(new IKEObject3d());
	sutoponObj->Initialize();
	sutoponObj->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Sutopon));

	TitleTex.reset(IKETexture::Create(ImageManager::TITLETIPS, { 0,0,0 }, { 0.5f,0.5f,0.5f }, { 1,1,1,1 }));
	TitleTex->TextureCreate();

	StageObjs[FIRST]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::MILKCAP_NORMAL));
	StageObjs[SECOND]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Tyuta));
	StageObjs[THIRD]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::KIDO_OBJ));
	StageObjs[FOUR]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::Camera));
	StageObjs[FIVE]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::DJ));
	StageObjs[SIX]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::PEDKOTOKO));
	StageObjs[SEVEN]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::LASTBOSS));
	StageObjs[TITLE]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::HOME));

	BackSprite = IKESprite::Create(ImageManager::PLAY, { 0.0f,0.0f });
	BackSprite->SetAddOffset(-0.0005f);
	SelQuestSprite = IKESprite::Create(ImageManager::SELQUEsT, { 0.0f,0.0f });
	SelQuestSub[0] = IKESprite::Create(ImageManager::SELQUEStsuB, { 0.0f,0.0f });
	SelQuestSub[1] = IKESprite::Create(ImageManager::SELQUEStsuB, { 0.0f,0.0f });

	SelQuestalpha = 1.f;
	SelSubAlpha[0] = 1.f;
	SelSubAlpha[1] = 1.f;
	//SelQuestSprite->SetAddOffset(-0.0005f);

	ButtonNav_RBLB[0] = IKESprite::Create(ImageManager::RBBUTTON, { 0,0 });
	ButtonNav_RBLB[1] = IKESprite::Create(ImageManager::LBBUTTON, { 0,0 });

	ButtonNav_Challenge_Cancel[0] = IKESprite::Create(ImageManager::Challenge, { 0,0 });
	ButtonNav_Challenge_Cancel[1] = IKESprite::Create(ImageManager::Cancel, { 0,0 });


	ButtonNav_RBLB[0]->SetAnchorPoint({ 0.5f,0.5f });
	ButtonNav_RBLB[1]->SetAnchorPoint({ 0.5f,0.5f });
	ButtonNav_RBLB[0]->SetPosition({ 800,650 });
	ButtonNav_RBLB[1]->SetPosition({ 580,650 });
	ButtonNav_RBLB[0]->SetSize({ 250,250 });
	ButtonNav_RBLB[1]->SetSize({ 250,250 });

	ButtonNav_Challenge_Cancel[0]->SetPosition({ 400,650 });
	ButtonNav_Challenge_Cancel[1]->SetPosition({ 200,650 });
	for (auto i = 0; i < 2; i++) {
		ButtonNav_Challenge_Cancel[i]->SetAnchorPoint({ 0.5f,0.5f });
		ButtonNav_Challenge_CancelScl[i] = { 200,150 };
		ButtonNav_Challenge_CancelColAlpha[i] = 1.f;
	}

	StageObj[FIRST] = IKESprite::Create(ImageManager::tip1, { 0,0 });
	StageObj[SECOND] = IKESprite::Create(ImageManager::tip2, { 0,0 });
	StageObj[THIRD] = IKESprite::Create(ImageManager::tip3, { 0,0 });
	StageObj[FOUR] = IKESprite::Create(ImageManager::tip4, { 0,0 });
	StageObj[FIVE] = IKESprite::Create(ImageManager::tip5, { 0,0 });
	StageObj[SIX] = IKESprite::Create(ImageManager::tip6, { 0,0 });
	StageObj[SEVEN] = IKESprite::Create(ImageManager::tip7, { 0,0 });
	StageObj[TITLE] = IKESprite::Create(ImageManager::tip8, { 0,0 });
	//ポストエフェクト用
	BossIcon[FIRST] = IKESprite::Create(ImageManager::CLOSEMILK, { 0,0 });
	BossIcon[SECOND] = IKESprite::Create(ImageManager::CLOSESYTOPON, { 0,0 });
	BossIcon[THIRD] = IKESprite::Create(ImageManager::CLOSEKIDO, { 0,0 });
	BossIcon[FOUR] = IKESprite::Create(ImageManager::CLOSECAMERA, { 0,0 });
	BossIcon[FIVE] = IKESprite::Create(ImageManager::CLOSEDJ, { 0,0 });
	BossIcon[SIX] = IKESprite::Create(ImageManager::CLOSEDARK, { 0,0 });
	BossIcon[SEVEN] = IKESprite::Create(ImageManager::CLOSEBRO, { 0,0 });
	BossIcon[TITLE] = IKESprite::Create(ImageManager::CLOSEHOME, { 0,0 });

	for (int i = 0; i < MAX; i++) {
		BossIcon[i]->SetAnchorPoint({ 0.5f,0.5f });
		BossIcon[i]->SetSize({ 0,0 });
		BossIcon[i]->SetPosition({ 1280 / 2,720 / 2 });
	}
	ButtonNav_Pos[0] = { 820,560 };
	ButtonNav_Pos[1] = { 450,560 };
	constexpr float PosRad = 30.f;
	for (auto i = 0; i < ObjNum; i++) {
		TipsPosY[i] = -360.f;
		StageObjRotAngle[i] = static_cast<float>(i) * (360.f / static_cast<float>(ObjNum)) + 180.f;
		//位置の初期化
		StageObjPos[i].x = Pedestal->GetPosition().x + sinf(StageObjRotAngle[i] * (PI / PI_180)) * PosRad;
		StageObjPos[i].z = Pedestal->GetPosition().z + cosf(StageObjRotAngle[i] * (PI / PI_180)) * PosRad;
		StageObjPos[i].y = Pedestal->GetPosition().y + 4.f;
		//BossIcon.
		StageObjRot[i].y = 90;
		StageObjs[i]->SetPosition(StageObjPos[i]);
		StageObjs[i]->SetScale({ 0.0f,0.0f,0.0f });
		m_Scale[i] = { 0.0f,0.0f,0.0f };
		AfterScale[i] = { 1.0f,1.0f,1.0f };
	}
	AfterScale[SECOND] = { 0.50f,0.50f,0.50f };
	m_Scale[FIRST] = { 5.0f,5.0f,5.0f };
	AfterScale[FIRST] = { 5.0f,5.0f,5.0f };
	m_Birth[FIRST] = true;
	m_BirthFinish[FIRST] = true;
	AfterScale[THIRD] = { 3.0f,3.0f,3.0f };
	AfterScale[FOUR] = { 2.70f,2.70f,2.70f };
	StageObjs[FOUR]->SetRotation({ 0.0f,90.0f,0.0f });
	StageObjRot[FOUR].z = 190;
	StageObjRot[FOUR].x = 190;
	AfterScale[FIVE] = { 0.2f,0.2f,0.2f };



	//一回開放したら大きさを合わせる
	for (auto i = 0; i < ObjNum; i++) {
		//	if (m_Birth[i]) {
		m_Scale[i] = AfterScale[i];
		//}
		TipsAct[i] = false;
	}
	m_Color[FIRST] = { 1,1,1 };
	m_Color[TITLE] = { 1,1,1 };

	MaxSpeed = 180.f;
	MaxScl = 15500.f;
}

void SelectScene::Jump()
{	//落下の緩急
	constexpr float Distortion = 0.5f;
	//ジャンプ高さ
	constexpr float Height = 4.f;

	float GroundY = -9.f;
	if (JumpK) {
		JFrameK += 1.f / 30.f;
		KotokoYPos = GroundY + (1.0f - pow(1.0f - sinf(PI * JFrameK), Distortion)) * Height;
	} else
	{
		JFrameK = 0.f;
	}
	if (JFrameK >= 1.f) {
		JumpK = false;
	}

	if (JumpS) {
		JFrameS += 1.f / 30.f;
		SutoYPos = 0.f + (1.0f - pow(1.0f - sinf(PI * JFrameS), Distortion)) * Height;
	} else
	{
		JFrameS = 0.f;
	}
	if (JFrameS >= 1.f) {
		JumpS = false;
	}
}

void SelectScene::Upda() {
	constexpr float PosRad = 28.f;

	//土台
	Pedestal->SetScale({ 15.f,15.f,15.f });
	Pedestal->Update();

	kotokoObj->SetPosition({ -20,KotokoYPos,-38 });
	kotokoObj->SetRotation({ 0,140,0 });
	kotokoObj->SetScale({ 3.7f,3.5f,3.7f });
	kotokoObj->Update();

	sutoponObj->SetRotation({ 20,5,-10 });
	sutoponObj->SetPosition({ 18,SutoYPos,-42 });
	sutoponObj->SetScale({ 3.1f,3.1f,3.1f });
	sutoponObj->Update();

	for (auto i = 0; i < ObjNum; i++) {
		if (TipsAct[i])
		{
			//	JumpK = true;
				//JumpS = true;
		}
	}
	CloseIconView(CloseF);
	Helper::GetInstance()->Clamp(closeScl, 0.f, MaxScl);
	Helper::GetInstance()->Clamp(closeRad, 0.f, 1500.f);
	if (CLastEaseTime <= 0.01f || CLastEaseTime >= 1.f)
		RotPedestal();

	//Selectは常時出す
	m_Birth[TITLE] = true;
	for (int i = 0; i < MAX; i++) {
		if (IconColor[i] < 1.f) { continue; }
		if (Input::GetInstance()->TriggerButton(Input::B)) {
			JumpK = true;
			JumpS = true;
			TipsAct[i] = true;
		}
	}

	ChangeEffect("FIRSTSTAGE", Stage::FIRST, FIRST);
	ChangeEffect("SECONDSTAGE", Stage::SECOND, SECOND);
	ChangeEffect("FOURTHSTAGE", Stage::FOUR, FOUR);
	ChangeEffect("THIRDSTAGE", Stage::THIRD, THIRD);
	ChangeEffect("FIVESTAGE", Stage::SIX, SIX);
	ChangeEffect("SIXSTAGE", Stage::FIVE, FIVE);
	ChangeEffect("SEVENSTAGE", Stage::SEVEN, SEVEN);
	ChangeEffect("TITLE", Stage::TITLE, TITLE);

	for (int i = 0; i < MAX; i++) {
		if (closeScl >= MaxScl - 2000.f) {
			BossIcon[i]->SetAnchorPoint({ 0.5f,0.5f });
			BossIcon[i]->SetSize({ 0,0 });
			BossIcon[i]->SetPosition({ 1280 / 2,720 / 2 });
			_stages = Stage::MAX;
			sin = false;
		}

		if (_stages != Stage::MAX) {
			TipsPosY[i] -= 40.f;
			BossIcon[i]->SetAnchorPoint({ 0.5f,0.5f });
			BossIcon[i]->SetSize({ closeScl,closeScl });
			BossIcon[i]->SetPosition({ 1280 / 2,720 / 2 });
		}
	}

	Jump();
	m_NowSelePos = {
		Pedestal->GetPosition().x + sinf(180.f * (PI / PI_180)) * PosRad,
		4.f,
		Pedestal->GetPosition().z + cosf(180 * (PI / PI_180)) * PosRad };

	for (auto i = 0; i < ObjNum; i++) {
		if (Collision::GetLength(m_NowSelePos, StageObjPos[i]) < 5) {
			StageObjRot[i].y++;
			IconColor[i] += 0.05f;
		} else {
			StageObjRot[i].y = 90;
			IconColor[i] -= 0.05f;
		}

		Helper::GetInstance()->Clamp(IconColor[i], 0.3f, 1.f);
	}
	StageObjPos[TITLE].y = 0.f;
	ViewTips();
	for (auto i = 0; i < ObjNum; i++) {
		StageObjPos[i].x = Pedestal->GetPosition().x + sinf(StageObjRotAngle[i] * (PI / PI_180)) * PosRad;
		StageObjPos[i].z = Pedestal->GetPosition().z + cosf(StageObjRotAngle[i] * (PI / PI_180)) * PosRad;

		StageObj[i]->SetSize({ 800.f,500.f });
		StageObj[i]->SetPosition({ 640,TipsPosY[i] });
		StageObj[i]->SetAnchorPoint({ 0.5f,0.5f });
		StageObjs[i]->SetColor({ m_Color[i].x, m_Color[i].y, m_Color[i].z,1.f });

		StageObjs[i]->SetRotation({ StageObjRot[i] });

		//StageObjs[FOUR]->SetRotation({90,StageObjRot[FOUR].y,0 });
		StageObjs[i]->SetPosition(StageObjPos[i]);
		StageObjs[i]->SetScale({ m_Scale[i].x * 1.5f,m_Scale[i].y * 1.5f,m_Scale[i].z * 1.5f });
		StageObjs[i]->Update();
	}
	StageObjs[TITLE]->SetColor({ 1,1,1,1 });

	TitleTex->SetScale({ 2,2,3 });
	TitleTex->SetPosition(StageObjPos[TITLE]);
	TitleTex->SetColor({ 1,1,1,1 });
	TitleTex->Update();

	//if (!ChangeLastF) {
	bool temp[ObjNum] = {};
	for (auto i = 0; i < TipsAct.size(); i++)
		temp[i] = TipsAct[i];
	if (Helper::GetInstance()->All_OfF(temp, ObjNum)) {
		if (TrigerSelect == NOINP) {
			if (Input::GetInstance()->TriggerButton(Input::RIGHT) || Input::GetInstance()->TriggerButton(Input::RB)) {
				if (!JumpS)JumpS = true;
				SelIndex++;
				TrigerSelect = RB;
			}

			if (Input::GetInstance()->TriggerButton(Input::LEFT) || Input::GetInstance()->TriggerButton(Input::LB)) {
				if (!JumpK)JumpK = true;
				SelIndex--;
				TrigerSelect = LB;

			}
		}
	}
	//}

	ChangeStageRot();
	m_Scale[TITLE] = { 1.01f,1.01f,1.01f };
	if (m_SelectState != SELECT_LAST)
		CLastEaseTime = 0.f;
	//セレクトのステート管理
	StateManager();

}

void SelectScene::Draw_Obj(DirectXCommon* dxcomn) {
	IKESprite::PreDraw();
	BackSprite->Draw();
	IKESprite::PostDraw();
	size_t t = ObjNum;

	IKEObject3d::PreDraw();
	Pedestal->Draw();
	for (auto i = 0; i < ObjNum; i++) {
		StageObjs[i]->Draw();
	}
	kotokoObj->Draw();
	sutoponObj->Draw();
	IKEObject3d::PostDraw();

	IKETexture::PreDraw2(dxcomn, 1);
	//TitleTex->Draw();
	IKETexture::PostDraw();
}

void SelectScene::Draw_Sprite() {

	for (auto i = 0; i < ObjNum; i++) {
		if (!BossIcon[i]) { continue; }
		if (_stages != i)continue;
		BossIcon[i]->Draw();
	}

}

void SelectScene::ImGuiDraw() {
	//ImGui::Begin("Select");
	//ImGui::Text("Scene%d", (int)_stages);
	//ImGui::Text("%d", m_SelectState);
	//ImGui::End();

	//SceneSave::GetInstance()->ImGuiDraw();
}
void SelectScene::Draw_SpriteBack() {
	if (closeScl <= 0.f) { return; }

	bool temp[ObjNum] = {};
	for (auto i = 0; i < TipsAct.size(); i++) {
		temp[i] = !TipsAct[i];
		if (TipsAct[i])
		{
			SelQuestalpha -= 0.1f;
			SelSubAlpha[0] -= 0.1f;
			SelSubAlpha[1] -= 0.1f;
		}
		if (!TipsAct[0] && !TipsAct[1] && !TipsAct[2] &&
			!TipsAct[3] && !TipsAct[4] && !TipsAct[5] &&
			!TipsAct[6] && !TipsAct[7])
		{
			SelQuestalpha += 0.1f;
			SelSubAlpha[0] += 0.1f;
			SelSubAlpha[1] += 0.1f;
		}
	}

	Helper::GetInstance()->Clamp(SelQuestalpha, 0.f, 1.f);
	Helper::GetInstance()->Clamp(SelSubAlpha[0], 0.f, 1.f);
	Helper::GetInstance()->Clamp(SelSubAlpha[1], 0.f, 1.f);
	if (SelQuestSprite != nullptr) {
		SelQuestSprite->SetSize({ 400,200 });
		SelQuestSprite->SetPosition({ 400,0 });
		SelQuestSprite->SetColor({ 1,1,1,SelQuestalpha });

		SelQuestSprite->Draw();

		SelQuestSub[0]->SetPosition({ 1030,150 });
		SelQuestSub[0]->SetSize({ 180,180 });
		SelQuestSub[0]->SetColor({ 1,1,1,SelSubAlpha[0] });
		SelQuestSub[1]->SetSize({ 100,100 });
		SelQuestSub[1]->SetPosition({ 1190,300 });
		SelQuestSub[1]->SetColor({ 1,1,1,SelSubAlpha[1] });
		//SelQuestSub[0]->Draw();
		//SelQuestSub[1]->Draw();
	}
	for (auto i = 0; i < ObjNum; i++) {
		if (!TipsAct[0] && !TipsAct[1] && !TipsAct[2] &&
			!TipsAct[3] && !TipsAct[4] && !TipsAct[5] &&
			!TipsAct[6] && !TipsAct[7]) {
			ButtonNav_RBLB[0]->Draw();
			ButtonNav_RBLB[1]->Draw();
		}
	}
	for (auto i = 0; i < ObjNum; i++) {
		StageObj[i]->Draw();
	}
	for (int i = 0; i < MAX; i++) {
		if (TipsPosY[i] < 360.f) { continue; }
		if (JudgChal) {
			size_t cancel = 0, notCancel = 1;
			ButtonNav_Challenge_CancelScl[cancel].x += 5.0f;
			ButtonNav_Challenge_CancelScl[cancel].y += 5.0f;
			ButtonNav_Challenge_CancelColAlpha[cancel] -= 0.02f;
			ButtonNav_Challenge_CancelColAlpha[notCancel] -= 0.04f;
			if (ButtonNav_Challenge_CancelColAlpha[cancel] < 0.0f) {
				JudgChal = false;
				for (auto i = 0; i < 2; i++) {
					ButtonNav_Challenge_CancelScl[i] = { 200,150 };
					ButtonNav_Challenge_CancelColAlpha[i] = 1.f;
				}
			}
		}
		for (int i = 0; i < 2; i++) {
			ButtonNav_Challenge_Cancel[i]->SetPosition(ButtonNav_Pos[i]);
			ButtonNav_Challenge_Cancel[i]->SetSize(ButtonNav_Challenge_CancelScl[i]);
			ButtonNav_Challenge_Cancel[i]->SetColor({ 1,1,1,ButtonNav_Challenge_CancelColAlpha[i] });

			ButtonNav_Challenge_Cancel[i]->Draw();
		}
	}

}


void SelectScene::RotPedestal() {
	float vel = static_cast<float>(PI_360 / ObjNum);
	if (TrigerSelect == RB) {
		vel *= 1.0f;
	} else if (TrigerSelect == LB) {
		vel *= -1.0f;
	}
	if (TrigerSelect == NOINP) {
		IconRotAngle_EaseT = 0.f;
		for (auto i = 0; i < ObjNum; i++)
			NowRotAngle[i] = StageObjRotAngle[i];
	} else {
		if (Helper::GetInstance()->FrameCheck(IconRotAngle_EaseT, 1 / 15.0f)) {
			TrigerSelect = NOINP;
		}
		for (auto i = 0; i < ObjNum; i++) {
			StageObjRotAngle[i] = Ease(In, Linear, IconRotAngle_EaseT, StageObjRotAngle[i], NowRotAngle[i] + vel);
		}
	}
}
void SelectScene::CloseIconView(bool closeF) {
	//定数わっしょい　良い方法模索中。。。
	constexpr float texScl = 6500.f;
	constexpr float MinScl = 2000.f;
	constexpr float SubRad = 0.52f;

	if (closeF && !sin) {
		closeScl -= SclingSpeed;
		if (closeScl <= MinScl) {
			SclingSpeed = 115.f;
			closeRad -= SclingSpeed * SubRad;
		} else
			SclingSpeed = 160.f;
	}
	if (sin) {
		if (closeScl >= MinScl)
			SclingSpeed = MaxSpeed;
		else
			SclingSpeed = 100.f;

		closeScl += SclingSpeed;
		closeRad += SclingSpeed * SubRad;
	}
}
void SelectScene::ChangeEffect(std::string name, Stage stage, UINT iconnum) {
	if (_stages != stage) { return; }
	if (sin) { return; }
	CloseF = true;
	if (closeScl <= 0.f) {
		sin = true;
		Player::GetInstance()->MoveStop(false);
		Player::GetInstance()->SetCanShot(true);
		//ResetParama();
		for (auto i = 0; i < ObjNum; i++)
			OldObjAngle[i] = StageObjRotAngle[i];
		Audio::GetInstance()->StopWave(AUDIO_LOAD);
		SceneManager::GetInstance()->ChangeScene(name);
		CloseF = false;
	}
}

void SelectScene::TipsPosUpda(Stage stage) {
	if (_stages == stage) { return; }
	if (!TipsAct[stage]) { return; }
	if (TipsPosY[stage] < 360.f) { return; }
	if (Input::GetInstance()->TriggerButton(Input::B)) {
		JudgChal = true;
		_stages = stage;
	} else if (Input::GetInstance()->TriggerButton(Input::X)) {
		TipsAct[stage] = false;
	}
}

void SelectScene::ViewTips() {
	constexpr float AddVal = 40.f;
	for (auto i = 0; i < ObjNum; i++) {
		TipsPosUpda((Stage)i);
		if (TipsAct[i])
			TipsPosY[i] += AddVal;
		else
			TipsPosY[i] -= AddVal;

		Helper::GetInstance()->Clamp(TipsPosY[i], -360.f, 360.f);
	}
}

void SelectScene::StateManager() {
	//debugよう
	m_Scale[TITLE] = { 0.7025f,0.71f,0.75f };

	//m_Wide = true;
	//m_SelectState = SELECT_SECOND;
	//m_Wide = true;
	//
	//クリア状況に応じてOBJの大きさだったりが違う
	if (m_SelectState == SELECT_FIRST) {		//ここは牛乳のみ

	} else if (m_SelectState == SELECT_SECOND) {//牛乳をクリアしてラスボス以外出現する
		bool temp[ObjNum] = {};
		for (auto i = 0; i < TipsAct.size(); i++)
			temp[i] = TipsAct[i];
		if (Helper::GetInstance()->All_OfF(temp, ObjNum)) {			//画面を閉じた後
			m_BirthTimer++;
			for (auto i = 1; i < ObjNum - 2; i++) {			//ラスボス以外
				m_Birth[i] = true;			//ボスが出現した
			}
			if (m_BirthTimer == 150) {			//　一定フレームに達するとボスが大きくなる
				m_Wide = true;
				m_BirthTimer = {};
			}
			if (m_Wide) {			//ボスを大きくする
				for (auto i = 1; i < ObjNum - 2; i++) {
					if (m_Color[i - 1].x > 0.5f)
						Helper::GetInstance()->FrameCheck(ObjColEase[i], 0.05f);

					m_Color[i] = { Ease(In,Cubic,ObjColEase[i],0,1),
						Ease(In,Cubic,ObjColEase[i],0,1),
						Ease(In,Cubic,ObjColEase[i],0,1),
					};
					m_BirthFinish[i] = true;
				}
			}
		}

		if (SceneSave::GetInstance()->AllClear()) {			//ラスボス以外倒したら(いまはちえよしまで)ラスボスが出現する
			//ChangeLastF = true;
			m_SelectState = SELECT_LAST;
			m_Wide = false;
			m_BirthTimer = {};
		}
	} else {
		m_Birth[SEVEN] = true;
		m_BirthFinish[SEVEN] = true;		//ラスボスゾーンの出現
		bool temp[ObjNum] = {};
		for (auto i = 0; i < TipsAct.size(); i++)
			temp[i] = TipsAct[i];
		if (Helper::GetInstance()->All_OfF(temp, ObjNum)) {
			m_BirthTimer++;
			//m_Birth[SEVEN] = true;			//ラスボスの出現

			if (m_BirthTimer == 150) {
				m_Wide = true;
				m_BirthTimer = {};
			}

			if (m_Wide) {//ラスボスのOBJを大きくする

				m_BirthFinish[SEVEN] = true;
			} else {
				BirthParticle();
			}
		}
	}
	if (m_SelectState == SELECT_LAST)
	{

	} else {
	}
	BirthParticle();
}

//パーティクル
void SelectScene::BirthParticle() {
	int l_Life[ObjNum];

	for (auto i = 0; i < ObjNum - 2; i++) {
		l_Life[i] = 50;
		if (SceneSave::GetInstance()->GetClearFlag((SeceneCategory)(i + 1))) {
			ParticleEmitter::GetInstance()->SelectEffect(l_Life[i], { StageObjPos[i].x,StageObjPos[i].y - 3.f,StageObjPos[i].z }, 1.0f, 0.0f, { 0.8f,0.5f,0.4f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
		}
	}
	l_Life[TITLE] = 50;
	if (!m_Birth[TITLE] && !m_BirthFinish[TITLE]) {
		//ParticleEmitter::GetInstance()->SelectEffect(l_Life[TITLE], { StageObjPos[TITLE].x,StageObjPos[TITLE].y - 3.f,StageObjPos[TITLE].z }, 1.0f, 0.0f, { 0.8f,0.5f,0.4f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	}
	if (m_SelectState != SELECT_LAST) {
		l_Life[SEVEN] = 50;
		if (SceneSave::GetInstance()->GetClearFlag(SeceneCategory::kSevenStage)) {
			ParticleEmitter::GetInstance()->SelectEffect(l_Life[SEVEN], { StageObjPos[SEVEN].x,StageObjPos[SEVEN].y - 3.f,StageObjPos[SEVEN].z }, 1.0f, 0.0f, { 0.8f,0.5f,0.4f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
		}
	}
}

void SelectScene::ChangeStageRot()
{
	if (m_SelectState != SELECT_LAST)return;
	if (CLastEaseTime >= 1.f)return;
	Helper::GetInstance()->FrameCheck(ObjColEase[SEVEN], 0.04f);
	m_Color[SEVEN] = { Ease(In,Cubic,ObjColEase[SEVEN],0,1),
		Ease(In,Cubic,ObjColEase[SEVEN],0,1),
			Ease(In,Cubic,ObjColEase[SEVEN],0,1),
	};

	Helper::GetInstance()->FrameCheck(CLastEaseTime, 0.04f);


	for (auto i = 0; i < ObjNum - 2; i++)
	{
		StageObjRotAngle[i] = Ease(In, Quad, CLastEaseTime, StageObjRotAngle[i], 180 - (SEVEN - i) * 360.f / ObjNum);
	}
	StageObjRotAngle[SEVEN] = Ease(In, Quad, CLastEaseTime, StageObjRotAngle[SEVEN], 180);
	StageObjRotAngle[TITLE] = Ease(In, Quad, CLastEaseTime, StageObjRotAngle[TITLE], 180 - (SEVEN - TITLE) * 360.f / ObjNum);

}
