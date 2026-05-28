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

#include "widget/Widget.h"
#include "widget/ButtonListener.h"

using namespace Sexy;

class LawnApp;
class Zombie;
class GameButton;

enum ZombatarPage
{
	PAGE_SKIN,
	PAGE_HAIR,
	PAGE_FACIAL_HAIR,
	PAGE_TIDBITS,
	PAGE_EYEWEAR,
	PAGE_CLOTHES,
	PAGE_ACCESSORIES,
	PAGE_HATS,
	PAGE_BACKDROPS
};

enum ZombatarState
{
	STATE_AVATAR_LIST,
	STATE_AVATAR_TRANSITION,
	STATE_AVATAR_CREATION,
	STATE_CONFIRM_TRANSITION,
	STATE_CONFIRM
};

constexpr const int MAX_NUM_ZOMBATARS = 100;
constexpr const int NUM_SKIN_COLORS = 12;
constexpr const int NUM_MORE_COLORS = 18;
constexpr const int NUM_HAIR_ITEMS = 16;
constexpr const int NUM_FACIAL_HAIR_ITEMS = 24;
constexpr const int NUM_TIDBITS_ITEMS = 14;
constexpr const int NUM_EYEWEAR_ITEMS = 16;
constexpr const int NUM_CLOTHES_ITEMS = 12;
constexpr const int NUM_ACCESSORIES_ITEMS = 15;
constexpr const int NUM_HAT_ITEMS = 14;
constexpr const int NUM_BACKDROP_ITEMS = 5;

struct ZombatarConfig
{
	int mSkinColor;
	int mClothes;
	int mClothesColor;
	int mTidbits;
	int mTidbitsColor;
	int mAccessories;
	int mAccessoriesColor;
	int mFacialHair;
	int mFacialHairColor;
	int mHair;
	int mHairColor;
	int mEyewear;
	int mEyewearColor;
	int mHat;
	int mHatColor;
	int mBackdrop;
	int mBackdropColor;

	ZombatarConfig()
		: mSkinColor(0)
		, mClothes(-1)
		, mClothesColor(17)
		, mTidbits(-1)
		, mTidbitsColor(17)
		, mAccessories(-1)
		, mAccessoriesColor(17)
		, mFacialHair(-1)
		, mFacialHairColor(17)
		, mHair(-1)
		, mHairColor(17)
		, mEyewear(-1)
		, mEyewearColor(17)
		, mHat(-1)
		, mHatColor(17)
		, mBackdrop(4)
		, mBackdropColor(17)
	{
	}
};

struct PortraitItem
{
	Image** mLine;
	Image** mColor;
	bool mAllowColor;
	float mOffsetX, mOffsetY;
	float mColorOffsetX, mColorOffsetY;

	PortraitItem()
		: mLine(nullptr)
		, mColor(nullptr)
		, mAllowColor(false)
		, mOffsetX(0)
		, mOffsetY(0)
		, mColorOffsetX(0)
		, mColorOffsetY(0)
	{
	}

	PortraitItem(Image** theLine, Image** theColor, bool theAllowColor, float theOffsetX, float theOffsetY, float theColorOffsetX, float theColorOffsetY)
		: mLine(theLine)
		, mColor(theColor)
		, mAllowColor(theAllowColor)
		, mOffsetX(theOffsetX)
		, mOffsetY(theOffsetY)
		, mColorOffsetX(theColorOffsetX)
		, mColorOffsetY(theColorOffsetY)
	{
	}
};

class ZombatarWidget : public Widget, public ButtonListener
{
public:
	enum
	{
		ZombatarWidget_Back = 100,
		ZombatarWidget_Next = 101,
		ZombatarWidget_Prev = 102,
		ZombatarWidget_ViewAvatar = 103,
		ZombatarWidget_Finished = 104,
		ZombatarWidget_BackAvatar = 105,
		ZombatarWidget_CreateNew = 106,
		ZombatarWidget_DeleteAvatar = 107,
		ZombatarWidget_NextPage = 108,
		ZombatarWidget_PrevPage = 109,
		ZombatarWidget_Skin = 110,
		ZombatarWidget_Hair = 111,
		ZombatarWidget_FacialHair = 112,
		ZombatarWidget_Tidbits = 113,
		ZombatarWidget_Eyewear = 114,
		ZombatarWidget_Clothes = 115,
		ZombatarWidget_Accessories = 116,
		ZombatarWidget_Hats = 117,
		ZombatarWidget_Backdrops = 118,
	};

public:
	LawnApp* mApp;
	ZombatarState mState;
	ZombatarPage mCurrentPage;
	ZombatarConfig mCurrentConfig;
	Zombie* mPreviewZombie;

	GameButton* mBackButton;
	GameButton* mNextButton;
	GameButton* mPrevButton;
	GameButton* mViewButton;
	GameButton* mFinishedButton;
	GameButton* mAvatarBackButton;
	GameButton* mDeleteZombatarButton;
	GameButton* mNewZombatarButton;
	GameButton* mNextPageButton;
	GameButton* mPrevPageButton;

	GameButton* mToggledButton;
	GameButton* mSkinButton;
	GameButton* mHairButton;
	GameButton* mFacialHairButton;
	GameButton* mTidbitsButton;
	GameButton* mEyewearButton;
	GameButton* mClothesButton;
	GameButton* mAccessoriesButton;
	GameButton* mHatsButton;
	GameButton* mBackdropsButton;

	int mSelectedAvatarIndex;
	int mScrollOffset;
	bool mNeedsUpdate;
	int mSubPage;
	int mMaxSubPages;
	int mTransitionTimer;
	Rect mItemRects[18];
	Rect mColorRects[18];

public:
	ZombatarWidget(LawnApp* theApp);
	virtual ~ZombatarWidget();

	virtual void Update();
	virtual void Draw(Graphics* g);
	virtual void AddedToManager(WidgetManager* theWidgetManager);
	virtual void RemovedFromManager(WidgetManager* theWidgetManager);
	virtual void ButtonPress(int) {}
	virtual void ButtonDepress(int) {}
	virtual void ButtonDownTick(int) {}
	virtual void ButtonMouseEnter(int) {}
	virtual void ButtonMouseLeave(int) {}
	virtual void ButtonMouseMove(int, int, int) {}
	virtual void MouseDown(int x, int y, int theClickCount);
	virtual void MouseUp(int x, int y, int theClickCount);

	void ChangePage(ZombatarPage thePage);
	int GetPageColorIndex(ZombatarPage thePage);
	int GetPageItemIndex(ZombatarPage thePage);
	void UpdatePreview();
	void DrawItemGrid(Graphics* g);
	void DrawColorPalette(Graphics* g);
	void DrawPreview(Graphics* g);

	bool SaveAvatar();
	bool LoadAvatar(int theIndex);
	bool DeleteAvatar(int theIndex);
	int GetAvatarCount();
	void ResetConfig();
	void AcceptTOS();
	void ChangeState(ZombatarState theNewState);
	void SetupPreviewZombie();
	int GetItemCount(ZombatarPage thePage);
};

#endif
