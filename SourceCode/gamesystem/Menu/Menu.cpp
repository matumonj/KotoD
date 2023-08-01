#include "Menu.h"
#include "ImageManager.h"
#include <Input.h>
#include <Helper.h>
#include <Easing.h>
#include "SelectScene.h"
void (Menu::* Menu::stateTable[])() = {
	&Menu::NoneUpdate,
	&Menu::OpenUpdate,
	&Menu::SelectUpdate,
	&Menu::CheckUpdate,
	&Menu::CheckOpenCloseUpdate,
	&Menu::ConfirmUpdate,
	&Menu::CloseUpdate,

};

Menu::Menu() {


}

Menu::~Menu() {

}

void Menu::Initialize() {
	sprites_[BackScreen] = CreateSprite(ImageManager::MENU_HOME, { half_Width,half_Height }, 30.0f);
	sprites_[ConfirmButton] = CreateSprite(ImageManager::MENU_RULE, buttonPos[0], 30.0f);
	sprites_[SceneBackButton] = CreateSprite(ImageManager::MENU_SCENECHANGE, buttonPos[1], 30.0f);
	sprites_[SutoponBar] = CreateSprite(ImageManager::MENU_FRAME, buttonPos[0], 30.0f);
	sprites_[Confirm_FIRST] = CreateSprite(ImageManager::EXPLANATION_1, { half_Width,half_Height }, 15.0f);
	sprites_[Confirm_SECOND] = CreateSprite(ImageManager::EXPLANATION_2, { half_Width,half_Height }, 15.0f);
	sprites_[Confirm_THIRD] = CreateSprite(ImageManager::EXPLANATION_3, { half_Width,half_Height }, 15.0f);
	sprites_[Confirm_FOUR] = CreateSprite(ImageManager::EXPLANATION_4, { half_Width,half_Height }, 15.0f);

	sprites_[Check_HOME] = CreateSprite(ImageManager::CHECK_HOME, { half_Width,half_Height }, 15.0f);
	sprites_[Check_OK] = CreateSprite(ImageManager::CHECK_OK, checkPos[0], 15.0f);
	sprites_[Check_NO] = CreateSprite(ImageManager::CHECK_NO, checkPos[1], 15.0f);
	sprites_[Check_BAR] = CreateSprite(ImageManager::MENU_FRAME, checkPos[0], 15.0f);

	for (int i = 0; i < SpriteMax; i++) {
		sprites_[i].sprite->SetAnchorPoint({ 0.5f,0.5f });
		sprites_[i].sprite->SetColor({ 1.2f,1.2f,1.2f,1 });
	}
}

void Menu::Update() {

	(this->*stateTable[(size_t)_state])();

	for (int i = 0; i < SpriteMax; i++) {
		sprites_[i].sprite->SetSize(sprites_[i].size);
		sprites_[i].sprite->SetPosition(sprites_[i].pos);
	}
}

void Menu::Draw() {
	IKESprite::PreDraw();
	for (int i = 0; i < SpriteMax; i++) {
		if (!sprites_[i].isVisible) { continue; }
		sprites_[i].sprite->Draw();
	}
	IKESprite::PostDraw();
}

bool Menu::Pause() {
	return (_state != State::NONE);
}

void Menu::NoneUpdate() {
	if (Input::GetInstance()->TriggerButton(Input::START)) {
		_state = State::OPEN;
	}
}

void Menu::OpenUpdate() {
	for (int i = BackScreen; i <= SutoponBar; i++) {
		sprites_[(size_t)i].isVisible = true;
		sprites_[(size_t)i].start_size = { 0.0f,0.0f };
	}
	sprites_[(size_t)BackScreen].end_size = { 1280.0f,720.0f };
	sprites_[(size_t)ConfirmButton].end_size = { 300.0f,150.0f };
	sprites_[(size_t)ConfirmButton].end_pos = buttonPos[0];
	sprites_[(size_t)SceneBackButton].end_size = { 300.0f,150.0f };
	sprites_[(size_t)SceneBackButton].end_pos = buttonPos[1];
	sprites_[(size_t)SutoponBar].end_size = { 300.0f,150.0f };
	sprites_[(size_t)SutoponBar].end_pos = buttonPos[0];

	for (int i = BackScreen; i <= SutoponBar; i++) {
		if (!Helper::GetInstance()->FrameCheck(sprites_[i].easingFrame, 1 / sprites_[i].kFrameMax)) {
			if (i == 0) {
				sprites_[(size_t)i].size = {
					Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.x,sprites_[i].end_size.x),
					Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.y,sprites_[i].end_size.y)
				};
			} else {
				sprites_[(size_t)i].size = {
					Ease(Out,Elastic,sprites_[i].easingFrame,sprites_[i].start_size.x,sprites_[i].end_size.x),
					Ease(Out,Elastic,sprites_[i].easingFrame,sprites_[i].start_size.y,sprites_[i].end_size.y)
				};
				if (i >= ConfirmButton && i <= SutoponBar) {
					sprites_[(size_t)i].pos = {
						Ease(Out, Quad, sprites_[i].easingFrame,half_Width, sprites_[i].end_pos.x),
						Ease(Out, Quad, sprites_[i].easingFrame, half_Height,sprites_[i].end_pos.y)
					};
				}

			}
		}
	}
	if (sprites_[SutoponBar].easingFrame == 1.0f) {
		_state = State::SELECT;
		for (int i = 0; i < SpriteMax; i++) {
			sprites_[i].easingFrame = 0.0f;
		}
	}
}

void Menu::SelectUpdate() {
	if (TriggerMoveButton()) {
		moveBar = true;
		if (Input::GetInstance()->TiltPushStick(Input::L_RIGHT) ||
			Input::GetInstance()->TriggerButton(Input::RIGHT)) {
			barIndex++;
		} else {
			barIndex--;
		}
		if (barIndex > SCENEBACK) {
			barIndex = CONFIRM;
		}
		if (barIndex < CONFIRM) {
			barIndex = SCENEBACK;
		}
		sprites_[(size_t)SutoponBar].start_pos = sprites_[(size_t)SutoponBar].pos;
		sprites_[(size_t)SutoponBar].end_pos = buttonPos[barIndex];
		sprites_[(size_t)SutoponBar].easingFrame = 0.0f;
	}

	if (Input::GetInstance()->TriggerButton(Input::START)) {
		if (moveBar) { return; }
		sprites_[(size_t)BackScreen].start_size = { 1280.0f,720.0f };
		sprites_[(size_t)ConfirmButton].start_size = { 300.0f,150.0f };
		sprites_[(size_t)SceneBackButton].start_size = { 300.0f,150.0f };
		sprites_[(size_t)SutoponBar].start_size = { 300.0f,150.0f };
		for (int i = BackScreen; i <= SutoponBar; i++) {
			sprites_[i].kFrameMax = 15.0f;
			sprites_[i].easingFrame = 0.0f;

			if (i >= ConfirmButton && i <= SutoponBar) {
				sprites_[i].start_pos = sprites_[i].pos;
			}
		}
		_state = State::CLOSE;
	}
	if (Input::GetInstance()->TriggerButton(Input::B)) {
		if (moveBar) { return; }
		switch (barIndex) {
		case CONFIRM:
			isOpen = true;
			confirmIndex = CFIRST;
			confirmIndexOld = CFIRST;
			_state = State::CONFIRM;
			break;
		case SCENEBACK:
			isOpen = true;
			_state = State::CHECKOPENCLOSE;
			barIndex = FINAL_OK;
			break;
		default:
			break;
		}
		for (int i = 0; i < SpriteMax; i++) {
			sprites_[i].easingFrame = 0.0f;
		}
	}

	if (moveBar) {
		if (!Helper::GetInstance()->FrameCheck(sprites_[SutoponBar].easingFrame, 1 / sprites_[SutoponBar].kFrameMax)) {
			sprites_[(size_t)SutoponBar].pos = {
			Ease(Out, Quad, sprites_[SutoponBar].easingFrame, sprites_[SutoponBar].start_pos.x, sprites_[SutoponBar].end_pos.x),
			Ease(Out, Quad, sprites_[SutoponBar].easingFrame, sprites_[SutoponBar].start_pos.y, sprites_[SutoponBar].end_pos.y)
			};
		} else {
			moveBar = false;
			sprites_[(size_t)SutoponBar].easingFrame = 0.0f;
		}
	}
}

void Menu::CheckUpdate() {
	if (isSelectBack) { return; }
	if (TriggerMoveButton()) {
		moveBar = true;
		if (Input::GetInstance()->TiltPushStick(Input::L_RIGHT) ||
			Input::GetInstance()->TriggerButton(Input::RIGHT)) {
			barIndex++;
		} else {
			barIndex--;
		}
		if (barIndex > FINAL_NO) {
			barIndex = FINAL_OK;
		}
		if (barIndex < FINAL_OK) {
			barIndex = FINAL_NO;
		}
		sprites_[(size_t)Check_BAR].start_pos = sprites_[(size_t)Check_BAR].pos;
		sprites_[(size_t)Check_BAR].end_pos = checkPos[barIndex];
		sprites_[(size_t)Check_BAR].easingFrame = 0.0f;
	}
	if (Input::GetInstance()->TriggerButton(Input::B)) {
		if (moveBar) { return; }
		switch (barIndex) {
		case FINAL_OK:
			isSelectBack = true;
			SelectScene::GetIns()->ResetParama();
			break;
		case FINAL_NO:
			_state = State::CHECKOPENCLOSE;
			isOpen = false;
			break;
		default:
			break;
		}
		for (int i = 0; i < SpriteMax; i++) {
			sprites_[i].easingFrame = 0.0f;
		}
	}
	if (moveBar) {
		if (!Helper::GetInstance()->FrameCheck(sprites_[Check_BAR].easingFrame, 1 / sprites_[Check_BAR].kFrameMax)) {
			sprites_[(size_t)Check_BAR].pos = {
			Ease(Out, Quad, sprites_[Check_BAR].easingFrame, sprites_[Check_BAR].start_pos.x, sprites_[Check_BAR].end_pos.x),
			Ease(Out, Quad, sprites_[Check_BAR].easingFrame, sprites_[Check_BAR].start_pos.y, sprites_[Check_BAR].end_pos.y)
			};
		} else {
			moveBar = false;
			sprites_[(size_t)Check_BAR].easingFrame = 0.0f;
		}
	}
}

void Menu::CheckOpenCloseUpdate() {
	sprites_[(size_t)Check_HOME].isVisible = true;
	sprites_[(size_t)Check_OK].isVisible = true;
	sprites_[(size_t)Check_NO].isVisible = true;
	sprites_[(size_t)Check_BAR].isVisible = true;
	if (isOpen) {
		for (int i = Check_HOME; i <= Check_BAR; i++) {
			sprites_[(size_t)i].isVisible = true;
			sprites_[(size_t)i].start_size = { 0.0f,0.0f };
			if (i != Check_HOME) {
				sprites_[(size_t)i].end_size = { 300.0f,150.0f };
				sprites_[(size_t)i].start_pos = { half_Width ,half_Height };
			}
		}
		sprites_[(size_t)Check_HOME].end_size = { 880, 520 };;
		sprites_[(size_t)Check_OK].end_pos = checkPos[0];
		sprites_[(size_t)Check_NO].end_pos = checkPos[1];
		sprites_[(size_t)Check_BAR].end_pos = checkPos[0];

		for (int i = Check_HOME; i <= Check_BAR; i++) {
			if (!Helper::GetInstance()->FrameCheck(sprites_[i].easingFrame, 1 / sprites_[i].kFrameMax)) {
				if (i == Check_HOME) {
					sprites_[(size_t)i].size = {
						Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.x,sprites_[i].end_size.x),
						Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.y,sprites_[i].end_size.y)
					};
				} else {
					sprites_[(size_t)i].size = {
						Ease(Out,Elastic,sprites_[i].easingFrame,sprites_[i].start_size.x,sprites_[i].end_size.x),
						Ease(Out,Elastic,sprites_[i].easingFrame,sprites_[i].start_size.y,sprites_[i].end_size.y)
					};
					if (i >= Check_OK && i <= Check_BAR) {
						sprites_[(size_t)i].pos = {
							Ease(Out, Quad, sprites_[i].easingFrame, sprites_[i].start_pos.x, sprites_[i].end_pos.x),
							Ease(Out, Quad, sprites_[i].easingFrame, sprites_[i].start_pos.y,sprites_[i].end_pos.y)
						};
					}

				}
			}
		}

	} else {
		for (int i = Check_HOME; i <= Check_BAR; i++) {
			sprites_[(size_t)i].isVisible = true;
			sprites_[(size_t)i].end_size = { 0.0f,0.0f };
			if (i != Check_HOME) {
				sprites_[(size_t)i].start_size = { 300.0f,150.0f };
				sprites_[(size_t)i].end_pos = { half_Width ,half_Height };

			}
		}
		sprites_[(size_t)Check_HOME].start_size = { 880,520 };
		sprites_[(size_t)Check_OK].start_pos = checkPos[0];
		sprites_[(size_t)Check_NO].start_pos = checkPos[1];
		sprites_[(size_t)Check_BAR].start_pos = checkPos[1];
		for (int i = Check_HOME; i <= Check_BAR; i++) {
			if (!Helper::GetInstance()->FrameCheck(sprites_[i].easingFrame, 1 / sprites_[i].kFrameMax)) {
				if (i == Check_HOME) {
					sprites_[(size_t)i].size = {
						Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.x,sprites_[i].end_size.x),
						Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.y,sprites_[i].end_size.y)
					};
				} else {
					sprites_[(size_t)i].size = {
						Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.x,sprites_[i].end_size.x),
						Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.y,sprites_[i].end_size.y)
					};
					if (i >= Check_OK && i <= Check_BAR) {
						sprites_[(size_t)i].pos = {
							Ease(Out, Quad, sprites_[i].easingFrame, sprites_[i].start_pos.x, sprites_[i].end_pos.x),
							Ease(Out, Quad, sprites_[i].easingFrame, sprites_[i].start_pos.y,sprites_[i].end_pos.y)
						};
					}

				}
			}
		}
	}
	if (sprites_[Check_HOME].easingFrame == 1.0f) {
		if (isOpen) {
			_state = State::CHECK;
		} else {
			_state = State::SELECT;
		}
		for (int i = 0; i < SpriteMax; i++) {
			sprites_[i].easingFrame = 0.0f;
		}
	}
}

void Menu::ConfirmUpdate() {
	sprites_[(size_t)Confirm_FIRST].isVisible = false;
	sprites_[(size_t)Confirm_SECOND].isVisible = false;
	sprites_[(size_t)Confirm_THIRD].isVisible = false;
	sprites_[(size_t)Confirm_FOUR].isVisible = false;
	sprites_[(size_t)confirmIndex].isVisible = true;
	sprites_[(size_t)confirmIndexOld].isVisible = true;
	if (isOpen) {
		sprites_[(size_t)confirmIndex].start_size = { 0.0f, 0.0f };
		sprites_[(size_t)confirmIndex].end_size = { 1280.0f,720.0f };
		if (!Helper::GetInstance()->FrameCheck(sprites_[confirmIndex].easingFrame, 1 / sprites_[confirmIndex].kFrameMax)) {
			sprites_[confirmIndex].size = {
				Ease(InOut,Circ,sprites_[confirmIndex].easingFrame,sprites_[confirmIndex].start_size.x,sprites_[confirmIndex].end_size.x),
				Ease(InOut,Circ,sprites_[confirmIndex].easingFrame,sprites_[confirmIndex].start_size.y,sprites_[confirmIndex].end_size.y),
			};
			sprites_[confirmIndexOld].size = sprites_[confirmIndex].size;
		} else {
			for (int i = CFIRST; i <= CFOUR; i++) {
				sprites_[i].size = sprites_[confirmIndex].size;
			}
			isOpen = false;
			sprites_[confirmIndex].easingFrame = 0.0f;
		}
		return;
	}
	if (isFinish) {
		sprites_[(size_t)confirmIndex].start_size = { 1280.0f,720.0f };
		sprites_[(size_t)confirmIndex].end_size = { 0.0f, 0.0f };
		if (!Helper::GetInstance()->FrameCheck(sprites_[confirmIndex].easingFrame, 1 / sprites_[confirmIndex].kFrameMax)) {
			sprites_[confirmIndex].size = {
				Ease(InOut,Circ,sprites_[confirmIndex].easingFrame,sprites_[confirmIndex].start_size.x,sprites_[confirmIndex].end_size.x),
				Ease(InOut,Circ,sprites_[confirmIndex].easingFrame,sprites_[confirmIndex].start_size.y,sprites_[confirmIndex].end_size.y),
			};
			sprites_[confirmIndexOld].size = sprites_[confirmIndex].size;
		} else {
			for (int i = CFIRST; i <= CFOUR;i++) {
				sprites_[i].size = sprites_[confirmIndex].size;
			}
			isFinish = false;
			sprites_[confirmIndex].easingFrame = 0.0f;
			_state = State::SELECT;
		}
		return;
	}

	if (TriggerMoveButton()) {
		if (Input::GetInstance()->TiltPushStick(Input::L_RIGHT) ||
			Input::GetInstance()->TriggerButton(Input::RIGHT)) {
			if (confirmIndex == CFOUR) { return; }
			confirmIndex++;
		} else {
			if (confirmIndex == CFIRST) { return; }
			confirmIndex--;
		}
		sprites_[confirmIndex].easingFrame = 0.0f;
		sprites_[confirmIndexOld].easingFrame = 0.0f;
		moveBar = true;
	}
	if (moveBar) {
		if (confirmIndexOld < confirmIndex) {
			sprites_[confirmIndex].start_pos = { window_Width + half_Width,half_Height };
			sprites_[confirmIndex].end_pos = { half_Width,half_Height };
			if (!Helper::GetInstance()->FrameCheck(sprites_[confirmIndex].easingFrame, 1 / sprites_[confirmIndex].kFrameMax)) {
				sprites_[confirmIndex].pos = {
					Ease(InOut,Circ,sprites_[confirmIndex].easingFrame,sprites_[confirmIndex].start_pos.x,sprites_[confirmIndex].end_pos.x),
					half_Height
				};
			} else {
				moveBar = false;
				confirmIndexOld = confirmIndex;
			}
		} else {
			sprites_[confirmIndexOld].end_pos = { window_Width + half_Width,half_Height };
			sprites_[confirmIndexOld].start_pos = { half_Width,half_Height };

			if (!Helper::GetInstance()->FrameCheck(sprites_[confirmIndexOld].easingFrame, 1 / sprites_[confirmIndexOld].kFrameMax)) {
				sprites_[confirmIndexOld].pos = {
					Ease(InOut,Circ,sprites_[confirmIndexOld].easingFrame,sprites_[confirmIndexOld].start_pos.x,sprites_[confirmIndexOld].end_pos.x),
					half_Height
				};
			} else {
				moveBar = false;
				confirmIndexOld = confirmIndex;
			}

		}
	}



	if (Input::GetInstance()->TriggerButton(Input::DOWN)||
		Input::GetInstance()->TiltPushStick(Input::L_DOWN)) {
		if (moveBar) { return; }
		isFinish = true;
		sprites_[(size_t)confirmIndex].start_size = { 1280.0f,720.0f };
		sprites_[(size_t)confirmIndex].end_size = { 0.0f, 0.0f };
		sprites_[confirmIndex].easingFrame = 0.0f;
	}
}

void Menu::CloseUpdate() {
	for (int i = BackScreen; i <= SutoponBar; i++) {
		sprites_[(size_t)i].isVisible = true;
		sprites_[(size_t)i].end_size = { 0.0f,0.0f };
	}

	for (int i = BackScreen; i <= SutoponBar; i++) {
		if (!Helper::GetInstance()->FrameCheck(sprites_[i].easingFrame, 1 / sprites_[i].kFrameMax)) {
			sprites_[(size_t)i].size = {
				Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.x,sprites_[i].end_size.x),
				Ease(Out,Quad,sprites_[i].easingFrame,sprites_[i].start_size.y,sprites_[i].end_size.y)
			};
			if (i >= ConfirmButton && i <= SutoponBar) {
				sprites_[(size_t)i].pos = {
		Ease(Out, Quad, sprites_[i].easingFrame, sprites_[i].start_pos.x, half_Width),
		Ease(Out, Quad, sprites_[i].easingFrame, sprites_[i].start_pos.y, half_Height)
				};

			}
		}
	}
	if (sprites_[SutoponBar].easingFrame == 1.0f) {
		_state = State::NONE;
		for (int i = 0; i < SpriteMax; i++) {
			sprites_[i].easingFrame = 0.0f;
			sprites_[i].kFrameMax = 30.0f;

		}
	}
}

Menu::SpriteInfo Menu::CreateSprite(UINT num, XMFLOAT2 pos, float easingFrame) {
	SpriteInfo itr = {};
	itr.sprite = IKESprite::Create(num, pos);
	itr.size = itr.sprite->GetSize();
	itr.pos = pos;
	itr.start_size = itr.size;
	itr.end_size = itr.size;
	itr.start_pos = pos;
	itr.end_pos = pos;
	itr.isAction = false;
	itr.easingFrame = 0.0f;
	itr.kFrameMax = easingFrame;
	return itr;
}

bool Menu::TriggerMoveButton() {
	if (moveBar) { return false; }
	if (Input::GetInstance()->TiltPushStick(Input::L_RIGHT, 0.0f) ||
		Input::GetInstance()->TriggerButton(Input::RIGHT)) {
		return true;
	}
	if (Input::GetInstance()->TiltPushStick(Input::L_LEFT, 0.0f) ||
		Input::GetInstance()->TriggerButton(Input::LEFT)) {
		return true;
	}
	return false;
}
