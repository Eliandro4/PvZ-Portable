/*
 * Copyright (C) 2026 Zhou Qiankang <wszqkzqk@qq.com>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * This file is part of PvZ-Portable.
 *
 * PvZ-Portable is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PvZ-Portable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with PvZ-Portable. If not, see <https://www.gnu.org/licenses/>.
 */

#include "ZombatarTOS.h"
#include "GameSelector.h"
#include "../System/PlayerInfo.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "widget/Slider.h"
#include "widget/Checkbox.h"
#include "widget/WidgetManager.h"

using namespace Sexy;

ZombatarTOS::ZombatarTOS(LawnApp* theApp)
	: LawnDialog(theApp, DIALOG_ZOMBATAR_TOS, true, "[ZOMBATAR_TOS_HEADER]", "", "", BUTTONS_NONE)
{
	mApp = theApp;

	mTOSSlider = new Slider(IMAGE_ZOMBATAR_TOS_SLIDER, IMAGE_ZOMBATAR_TOS_SLIDER_THUMB, ZombatarTOS_Slider, this);
	mTOSSlider->mHorizontal = false;
	mTOSSlider->SetValue(0);

	mBackButton = MakeNewButton(ZombatarTOS_Back, this, "", nullptr,
		IMAGE_ZOMBATAR_BACK_BUTTON, IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT, IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT);

	mAcceptButton = MakeNewButton(ZombatarTOS_Accept, this, "", nullptr,
		IMAGE_ZOMBATAR_ACCEPT_BUTTON, IMAGE_ZOMBATAR_ACCEPT_BUTTON_HIGHLIGHT, IMAGE_ZOMBATAR_ACCEPT_BUTTON_HIGHLIGHT);
	mAcceptButton->mDisabled = true;

	mTOSCheckbox = MakeNewCheckbox(ZombatarTOS_TOSCheckbox, this, false);

	CalcSize(110, 200);
}

ZombatarTOS::~ZombatarTOS()
{
	if (mTOSSlider)
		delete mTOSSlider;
	if (mBackButton)
		delete mBackButton;
	if (mAcceptButton)
		delete mAcceptButton;
	if (mTOSCheckbox)
		delete mTOSCheckbox;
}

void ZombatarTOS::AddedToManager(WidgetManager* theWidgetManager)
{
	Dialog::AddedToManager(theWidgetManager);
	AddWidget(mTOSSlider);
	AddWidget(mBackButton);
	AddWidget(mAcceptButton);
	AddWidget(mTOSCheckbox);
}

void ZombatarTOS::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Dialog::RemovedFromManager(theWidgetManager);
	RemoveWidget(mTOSSlider);
	RemoveWidget(mBackButton);
	RemoveWidget(mAcceptButton);
	RemoveWidget(mTOSCheckbox);
}

void ZombatarTOS::Resize(int theX, int theY, int theWidth, int theHeight)
{
	LawnDialog::Resize(theX, theY, theWidth, theHeight);
	mTOSSlider->Resize(Rect(GetWidth() - 15, GetTop() + 5, 40, 135));
	mBackButton->Resize(GetLeft(), 340, IMAGE_ZOMBATAR_BACK_BUTTON->mWidth, IMAGE_ZOMBATAR_BACK_BUTTON->mHeight);
	mAcceptButton->Resize(GetWidth() - 60, 340, IMAGE_ZOMBATAR_ACCEPT_BUTTON->mWidth, IMAGE_ZOMBATAR_ACCEPT_BUTTON->mHeight);
	mTOSCheckbox->Resize(GetWidth() - 110, 335, 46, 45);
}

void ZombatarTOS::Draw(Graphics* g)
{
	LawnDialog::Draw(g);
	g->ClipRect(Rect(GetLeft(), GetTop(), GetWidth(), mHeight - 260));
	TodDrawStringWrapped(g, "[ZOMBATAR_TOS]",
		Rect(GetLeft() + 10, GetTop() - static_cast<int>(mTOSSlider->mVal * 510), GetWidth() - 90, mHeight),
		FONT_PICO129, Color(255, 255, 255), DS_ALIGN_LEFT);
}

void ZombatarTOS::Update()
{
	LawnDialog::Update();
}

void ZombatarTOS::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void ZombatarTOS::ButtonDepress(int theId)
{
	switch (theId)
	{
	case ZombatarTOS_Back:
		mApp->KillDialog(mId);
		break;
	case ZombatarTOS_Accept:
		mApp->KillDialog(mId);
		mApp->mGameSelector->mZombatarWidget->ChangeState(STATE_AVATAR_LIST);
		mApp->mGameSelector->mZombatarWidget->Move(0, 0);
		mApp->mGameSelector->mWidgetManager->AddWidget(mApp->mGameSelector->mZombatarWidget);
		mApp->mGameSelector->mWidgetManager->SetFocus(mApp->mGameSelector->mZombatarWidget);
		if (mApp->mPlayerInfo)
		{
			mApp->mPlayerInfo->mZombatarAccepted = 1;
			mApp->WriteCurrentUserConfig();
		}
		break;
	}
}

void ZombatarTOS::CheckboxChecked(int theId, bool checked)
{
	if (theId == ZombatarTOS_TOSCheckbox)
	{
		mAcceptButton->mDisabled = !checked;
	}
}

void ZombatarTOS::SliderVal(int theId, double theVal)
{
	(void)theId;
	(void)theVal;
	MarkDirty();
}
