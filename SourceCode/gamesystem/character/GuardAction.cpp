#include "GuardAction.h"

#include "CsvLoader.h"
#include "Easing.h"
#include "Helper.h"
#include "ImageManager.h"

void GuardAction::Init()
{
	
	for (auto i = 0; i < GuardSize; i++) {
		guardtex[i].reset(new IKEObject3d());
		guardtex[i]->Initialize();
		guardAlpha[i] = 0.f;
	}
	guardtex[0]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::WIDTH1));
	guardtex[1]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::WIDTH1));
	guardtex[2]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::WIDTH1));
	guardtex[3]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::WIDTH1));
	guardtex[4]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::WIDTH1));
	guardtex[5]->SetModel(ModelManager::GetInstance()->GetModel(ModelManager::WIDTH1));
	//ガード続く時間
	GuardTimes = static_cast<float>(std::any_cast<double>(LoadCSV::LoadCsvParam("Resources/csv/chara/boss/five/Fiveboss.csv", "GuardTime")));

}

void GuardAction::Upda()
{


	XMVECTOR positionA = {
		boss->GetPosition().x,
		boss->GetPosition().y,
		boss->GetPosition().z
	};
	XMVECTOR positionB[GuardSize];
	//プレイヤーと敵のベクトルの長さ(差)を求める
	XMVECTOR SubVector[GuardSize];
	//調整用
	float RotY[GuardSize];

	GuardAreacreate();

	GuardEaseT += 0.01f;
	for (auto i = 0; i < GuardSize; i++) {
		if (guardtex[i] == nullptr)continue;
		positionB[i] = { guardPos[i].x, guardPos[i].y, guardPos[i].z };
		SubVector[i] = XMVectorSubtract(positionB[i], positionA); // positionA - positionB;
		RotY[i] = atan2f(SubVector[i].m128_f32[0], SubVector[i].m128_f32[2]);

		if (guardtexEaseT[4] >= 1.f) {
			guardRot[i].y += 1.f;
			
		}// Ease(In, Quad, GuardEaseT, 0.f, 180.f);
		guardtex[i]->SetPosition({boss->GetPosition()});
		guardtex[i]->SetRotation(guardRot[i]);
		guardtex[i]->SetScale({ guardScl[i]});

		if (guardUvY[i] > 1.f)guardUvY[i] = 0.f;
		guardUvY[i] += 0.001f;
		guardtex[i]->SetUvScrollY(guardUvY[i]);
		guardtex[i]->SetColor({ 1,1,1,guardAlpha[i] });
		guardtex[i]->Update();
	}
	if(!GuardStart)
	{
		for (auto i = 0; i < GuardSize; i++) {
			guardAlpha[i] -= 0.05f;
		}
		
	}
}

void GuardAction::Draw(DirectXCommon* dxCommon)
{
	IKEObject3d::PreDraw();
	for (auto i = 0; i < GuardSize; i++) {
		if (guardAlpha[i] <= 0.f)continue;
		guardtex[i]->Draw();
	}
	IKEObject3d::PostDraw();
}

void GuardAction::SpriteDraw()
{

}

void GuardAction::DeathUpdate(int Timer) {

}

void GuardAction::GuardAreacreate()
{
	if (GuardStart) {
		GuardTime++;
		Helper::GetInstance()->FrameCheck(guardtexEaseT[5], 0.02f);
		for (auto i = 5; i >=0; i--) {
			if(guardtexEaseT[i+1] > 0.8f)
				Helper::GetInstance()->FrameCheck(guardtexEaseT[i], 0.02f);
			}

		for(auto i=0;i<GuardSize;i++)
		{
			if (guardtexEaseT[i] > 0.2f)
				guardAlpha[i] += 0.05f;
			
			guardScl[i] = {
				Ease(In,Quad,guardtexEaseT[i],0.f,8.5f),
				Ease(In,Quad,guardtexEaseT[i],0.f,8.f),
				Ease(In,Quad,guardtexEaseT[i],0.f,8.5f)

			};
			Helper::GetInstance()->Clamp(guardAlpha[i], 0.f, 0.5f);
		}

		if(GuardTime>GuardTimes)
		{
			GuardStart = false;
		}
	}
}
