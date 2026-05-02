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

#ifndef __ZOMBATARWIDGET_H__
#define __ZOMBATARWIDGET_H__

#include "../../ConstEnums.h"
#include "widget/Widget.h"
#include "widget/ButtonListener.h"
#include "../System/PlayerInfo.h"

class LawnApp;
class NewLawnButton;
namespace Sexy { class ButtonWidget; }

using namespace Sexy;

enum ZombatarCategory
{
	ZOMBATAR_CATEGORY_SKIN = 0,
	ZOMBATAR_CATEGORY_HAIR,
	ZOMBATAR_CATEGORY_FACIAL_HAIR,
	ZOMBATAR_CATEGORY_HATS,
	ZOMBATAR_CATEGORY_EYEWEAR,
	ZOMBATAR_CATEGORY_CLOTHES,
	ZOMBATAR_CATEGORY_ACCESSORIES,
	ZOMBATAR_CATEGORY_TIDBITS,
	ZOMBATAR_CATEGORY_BACKDROPS,
	NUM_ZOMBATAR_CATEGORIES
};

class ZombatarWidget : public Widget, public ButtonListener
{
private:
	enum
	{
		Zombatar_Back = 200,
		Zombatar_Finished,
		Zombatar_NewZombie,
		Zombatar_Delete,
		Zombatar_View,
		Zombatar_Save,
		Zombatar_PrevPage,
		Zombatar_NextPage,
		Zombatar_CategoryBase = 300,
		Zombatar_AccessoryBase = 400
	};

public:
	enum class State
	{
		MAIN,
		PREV_ANIM,
		NEXT_ANIM,
		COLOR_PICKER
	};

public:
	LawnApp*					mApp;
	NewLawnButton*				mBackButton;
	NewLawnButton*				mFinishedButton;
	NewLawnButton*				mNewZombieButton;
	NewLawnButton*				mDeleteButton;
	NewLawnButton*				mPrevButton;
	NewLawnButton*				mNextButton;
	NewLawnButton*				mCategoryButtons[NUM_ZOMBATAR_CATEGORIES];
	ButtonWidget*				mAccessoryButtons[9];
	State						mState;
	int							mCurrentCategory;
	int							mCurrentPage;
	int							mSlideCounter;
	int							mSlideStartX;
	int							mSlideTargetX;
	ZombatarHead				mCurrentHead;
	int							mCurrentHeadIndex;
	bool						mNeedToSave;
	bool						mShowingNewZombie;

public:
	ZombatarWidget(LawnApp* theApp);
	virtual ~ZombatarWidget();

	virtual void				Update();
	virtual void				Draw(Graphics* g);
	virtual void				AddedToManager(WidgetManager* theWidgetManager);
	virtual void				RemovedFromManager(WidgetManager* theWidgetManager);
	virtual void				OrderInManagerChanged();
	virtual void				ButtonPress(int theId);
	virtual void				ButtonDepress(int theId);
	virtual void				ButtonDownTick(int theId) {}
	virtual void				ButtonMouseEnter(int theId) {}
	virtual void				ButtonMouseLeave(int theId) {}
	virtual void				ButtonMouseMove(int theId, int theX, int theY) {}
	virtual void				MouseDown(int theX, int theY, int theClickCount);
	virtual void				MouseUp(int theX, int theY, int theClickCount);

	void						EnterEditor();
	void						LoadCurrentHead();
	void						SaveCurrentHead();

private:
	void						DrawPreview(Graphics* theG);
	void						DrawCategoryButtons(Graphics* theG);
	void						DrawAccessoryGrid(Graphics* theG);
	void						DrawColorPicker(Graphics* theG);
	void						DrawActionButtons(Graphics* theG);
	int							GetTotalPages() const;
	Color						GetCategoryColor(int theCategory, int theColorIndex) const;
	Image*						GetCategoryImage(int theCategory, int theItemIndex) const;
	Image*						GetCategoryMask(int theCategory, int theItemIndex) const;
};

#endif
