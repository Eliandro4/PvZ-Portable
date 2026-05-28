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

#include "ZombatarWidget.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../Zombie.h"
#include "../System/PlayerInfo.h"
#include "GameButton.h"
#include "widget/WidgetManager.h"
#include "graphics/Graphics.h"
#include "../../Sexy.TodLib/TodCommon.h"
#include "../../Sexy.TodLib/TodStringFile.h"
#include "../../SexyAppFramework/Common.h"
#include "../../SexyAppFramework/graphics/Font.h"

static Color gSkinColors[NUM_SKIN_COLORS] = {
	Color(134, 147, 122), Color(79, 135, 94), Color(127, 135, 94), Color(120, 130, 50),
	Color(156, 163, 105), Color(96, 151, 11), Color(147, 184, 77), Color(82, 143, 54),
	Color(121, 168, 99), Color(65, 156, 74), Color(107, 178, 114), Color(104, 121, 90)
};

static Color gMoreColors[NUM_MORE_COLORS] = {
	Color(151, 33, 33), Color(199, 53, 53), Color(220, 112, 47), Color(251, 251, 172),
	Color(240, 210, 87), Color(165, 126, 65), Color(106, 72, 32), Color(72, 35, 5),
	Color(50, 56, 61), Color(0, 0, 0), Color(197, 239, 239), Color(63, 109, 242),
	Color(13, 202, 151), Color(158, 183, 19), Color(30, 210, 64), Color(225, 65, 230),
	Color(128, 47, 204), Color(255, 255, 255)
};

static PortraitItem gPortraitItems[116] = {
	// Clothes (0-11)
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_1,  nullptr, false, 49, 69, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_2,  nullptr, false, 37, 59, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_3,  nullptr, false, 48, 70, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_4,  nullptr, false, 38, 70, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_5,  nullptr, false, 52, 74, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_6,  nullptr, false, 54, 69, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_7,  nullptr, false, 39, 64, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_8,  nullptr, false, 50, 69, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_9,  nullptr, false, 51, 61, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_10, nullptr, false, 46, 68, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_11, nullptr, false, 46, 70, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_CLOTHES_12, nullptr, false, 41, 73, 0, 0 },
	// Hats (12-25)
	{ &Sexy::IMAGE_ZOMBATAR_HATS_1,  &Sexy::IMAGE_ZOMBATAR_HATS_1_MASK,  true,  -10, -35, -2, -1 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_2,  nullptr,                            false, 9, -28, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_3,  &Sexy::IMAGE_ZOMBATAR_HATS_3_MASK,  true,  15, -20, -15, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_4,  nullptr,                            false, -27, -30, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_5,  nullptr,                            false, 3, -24, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_6,  &Sexy::IMAGE_ZOMBATAR_HATS_6_MASK,  true,  -20, -37, -4, 2 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_7,  &Sexy::IMAGE_ZOMBATAR_HATS_7_MASK,  true,  15, -23, 1, -15 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_8,  &Sexy::IMAGE_ZOMBATAR_HATS_8_MASK,  true,  -35, -40, 2, 2 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_9,  &Sexy::IMAGE_ZOMBATAR_HATS_9_MASK,  true,  0, -40, 2, 2 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_10, nullptr,                            false, -25, 5, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_11, &Sexy::IMAGE_ZOMBATAR_HATS_11_MASK, true,  25, -18, -1, -14 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_12, nullptr,                            false, 5, -25, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_13, nullptr,                            false, -20, -40, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HATS_14, nullptr,                            false, -15, -35, 0, 0 },
	// Hair (26-41)
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_1,  &Sexy::IMAGE_ZOMBATAR_HAIR_1_MASK,  true,  -5, -40, -9, -2 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_2,  &Sexy::IMAGE_ZOMBATAR_HAIR_2_MASK,  true,  -15, -15, -2, -4 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_3,  nullptr,                             false, -15, -10, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_4,  nullptr,                             false, -8, -25, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_5,  nullptr,                             false, -2, -3, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_6,  nullptr,                             false, 1, -27, 3, 2 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_7,  nullptr,                             false, 13, -18, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_8,  nullptr,                             false, -10, -25, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_9,  nullptr,                             false, 90, 15, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_10, nullptr,                             false, -15, -8, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_11, &Sexy::IMAGE_ZOMBATAR_HAIR_11_MASK, true,  -5, -21, -3, -3 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_12, &Sexy::IMAGE_ZOMBATAR_HAIR_12_MASK, true,  18, -42, -2, -4 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_13, &Sexy::IMAGE_ZOMBATAR_HAIR_13_MASK, true,  -5, -27, -3, -2 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_14, &Sexy::IMAGE_ZOMBATAR_HAIR_14_MASK, true,  -29, -42, -1, -5 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_15, &Sexy::IMAGE_ZOMBATAR_HAIR_15_MASK, true,  7, -36, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_HAIR_16, nullptr,                             false, -12, -20, 0, 0 },
	// Eyewear (42-57)
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_1,  &Sexy::IMAGE_ZOMBATAR_EYEWEAR_1_MASK,  true,  -9, 33, 1, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_2,  &Sexy::IMAGE_ZOMBATAR_EYEWEAR_2_MASK,  true,  -6, 46, 1, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_3,  &Sexy::IMAGE_ZOMBATAR_EYEWEAR_3_MASK,  true,  -9, 29, 1, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_4,  &Sexy::IMAGE_ZOMBATAR_EYEWEAR_4_MASK,  true,  -9, 39, 1, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_5,  &Sexy::IMAGE_ZOMBATAR_EYEWEAR_5_MASK,  true,  -7, 36, 1, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_6,  &Sexy::IMAGE_ZOMBATAR_EYEWEAR_6_MASK,  true,  -8, 39, 0, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_7,  &Sexy::IMAGE_ZOMBATAR_EYEWEAR_7_MASK,  true,  13, 51, 1, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_8,  &Sexy::IMAGE_ZOMBATAR_EYEWEAR_8_MASK,  true,  -5, 31, 1, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_9,  &Sexy::IMAGE_ZOMBATAR_EYEWEAR_9_MASK,  true,  -1, 61, 1, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_10, &Sexy::IMAGE_ZOMBATAR_EYEWEAR_10_MASK, true,  -6, 36, 1, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_11, &Sexy::IMAGE_ZOMBATAR_EYEWEAR_11_MASK, true,  -6, 28, 1, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_12, &Sexy::IMAGE_ZOMBATAR_EYEWEAR_12_MASK, true,  0, 56, 1, 1 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_13, nullptr,                               false, -8, 41, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_14, nullptr,                               false, -3, 24, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_15, nullptr,                               false, 4, 25, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_EYEWEAR_16, nullptr,                               false, -3, 25, 0, 0 },
	// FacialHair (58-81)
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_1,  &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_1_MASK,  true,  -3, 67, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_2,  nullptr,                                  false, 13, 70, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_3,  nullptr,                                  false, 7, 70, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_4,  &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_4_MASK,  true,  0, 65, -3, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_5,  nullptr,                                  false, 31, 105, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_6,  nullptr,                                  false, 10, 72, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_7,  nullptr,                                  false, -25, 67, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_8,  &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_8_MASK,  true,  7, 65, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_9,  &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_9_MASK,  true,  3, 65, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_10, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_10_MASK, true,  6, 72, 0, -2 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_11, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_11_MASK, true,  6, 52, -1, -3 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_12, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_12_MASK, true,  -2, 66, -7, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_13, nullptr,                                  false, 7, 70, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_14, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_14_MASK, true,  -20, 63, -1, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_15, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_15_MASK, true,  25, 105, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_16, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_16_MASK, true,  25, 100, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_17, nullptr,                                  false, 5, 70, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_18, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_18_MASK, true,  20, 56, 0, -4 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_19, nullptr,                                  false, 8, 52, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_20, nullptr,                                  false, 76, 40, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_21, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_21_MASK, true,  80, 43, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_22, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_22_MASK, true,  -22, 51, -3, -5 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_23, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_23_MASK, true,  20, 105, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_24, &Sexy::IMAGE_ZOMBATAR_FACIALHAIR_24_MASK, true,  0, 68, -1, 0 },
	// Tidbits (82-95)
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_1,  nullptr, false, -9, 24, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_2,  nullptr, false, -9, 24, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_3,  nullptr, false, 9, 72, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_4,  nullptr, false, -6, 23, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_5,  nullptr, false, -6, 19, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_6,  nullptr, false, -9, 27, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_7,  nullptr, false, -9, 33, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_8,  nullptr, false, -4, 16, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_9,  nullptr, false, -16, 37, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_10, nullptr, false, -1, 32, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_11, nullptr, false, -1, 31, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_12, nullptr, false, 49, 52, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_13, nullptr, false, 51, 11, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_TIDBITS_14, nullptr, false, 76, 76, 0, 0 },
	// Accessories (96-110) - skip ACCESSORY_7
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_1,  nullptr, false, 65, 70, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_2,  nullptr, false, 70, 70, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_3,  nullptr, false, 48, 73, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_4,  nullptr, false, 93, 55, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_5,  nullptr, false, 93, 60, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_6,  nullptr, false, 93, 60, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_8,  nullptr, false, 66, 71, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_9,  nullptr, false, 80, 25, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_10, nullptr, false, 23, 78, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_11, nullptr, false, 5, 60, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_12, nullptr, false, 97, 52, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_13, nullptr, false, 40, 90, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_14, nullptr, false, 30, 105, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_15, nullptr, false, 95, 30, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_ACCESSORY_16, nullptr, false, -25, 0, 0, 0 },
	// Backdrops (111-115) - order: CRAZYDAVE, MENU, MENU_DOS, ROOF, BLANK
	{ &Sexy::IMAGE_ZOMBATAR_BACKGROUND_CRAZYDAVE,  nullptr, false, 0, 0, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_BACKGROUND_MENU,       nullptr, false, 0, 0, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_BACKGROUND_MENU_DOS,   nullptr, false, 0, 0, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_BACKGROUND_ROOF,       nullptr, false, 0, 0, 0, 0 },
	{ &Sexy::IMAGE_ZOMBATAR_BACKGROUND_BLANK,      nullptr, true,  0, 0, 0, 0 },
};

static int ItemTypeOffset(ZombatarPage page)
{
	switch (page)
	{
	case PAGE_CLOTHES:		return 0;
	case PAGE_HATS:			return 12;
	case PAGE_HAIR:			return 26;
	case PAGE_EYEWEAR:		return 42;
	case PAGE_FACIAL_HAIR:	return 58;
	case PAGE_TIDBITS:		return 82;
	case PAGE_ACCESSORIES:	return 96;
	case PAGE_BACKDROPS:	return 111;
	default:				return 0;
	}
}

static void GetPortraitItemScale(int theItemIndex, float* theScaleX, float* theScaleY)
{
	if (theItemIndex < 0 || theItemIndex >= 116)
	{
		*theScaleX = 0.24f;
		*theScaleY = 0.24f;
		return;
	}

	Image* aImg = gPortraitItems[theItemIndex].mLine ? *gPortraitItems[theItemIndex].mLine : nullptr;
	if (!aImg)
	{
		*theScaleX = 0.24f;
		*theScaleY = 0.24f;
		return;
	}

	float aScale;
	switch (theItemIndex)
	{
	case 0:  aScale = 0.14f; break;
	case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11:
		aScale = 0.15f; break;
	case 12: aScale = 0.17f; break;
	case 13: aScale = 0.19f; break;
	case 14: aScale = 0.22f; break;
	case 15: aScale = 0.20f; break;
	case 16: aScale = 0.19f; break;
	case 17: aScale = 0.18f; break;
	case 18: aScale = 0.20f; break;
	case 19: aScale = 0.25f; break;
	case 20: aScale = 0.26f; break;
	case 21: aScale = 0.16f; break;
	case 22: aScale = 0.18f; break;
	case 23: aScale = 0.23f; break;
	case 24: aScale = 0.19f; break;
	case 25: aScale = 0.25f; break;
	case 26: aScale = 0.21f; break;
	case 27: aScale = 0.21f; break;
	case 28: aScale = 0.24f; break;
	case 29: aScale = 0.23f; break;
	case 30: aScale = 0.22f; break;
	case 31: aScale = 0.22f; break;
	case 32: aScale = 0.22f; break;
	case 33: aScale = 0.23f; break;
	case 34:
		*theScaleX = 1.4f;
		*theScaleY = 0.9f;
		return;
	case 35: aScale = 0.24f; break;
	case 36: aScale = 0.21f; break;
	case 37: aScale = 0.21f; break;
	case 38: aScale = 0.24f; break;
	case 39: aScale = 0.24f; break;
	case 40: aScale = 0.23f; break;
	case 41: aScale = 0.25f; break;
	case 42: aScale = 0.22f; break;
	case 43: aScale = 0.22f; break;
	case 44: aScale = 0.24f; break;
	case 45: aScale = 0.21f; break;
	case 46: aScale = 0.22f; break;
	case 47: aScale = 0.21f; break;
	case 48: aScale = 0.21f; break;
	case 49: aScale = 0.20f; break;
	case 50: aScale = 0.20f; break;
	case 51: aScale = 0.20f; break;
	case 52: aScale = 0.21f; break;
	case 53: aScale = 0.20f; break;
	case 54: aScale = 0.23f; break;
	case 55: aScale = 0.23f; break;
	case 56: aScale = 0.22f; break;
	case 57: aScale = 0.23f; break;
	case 58: aScale = 0.24f; break;
	case 59: aScale = 0.25f; break;
	case 60: aScale = 0.24f; break;
	case 61: aScale = 0.24f; break;
	case 62: aScale = 0.24f; break;
	case 63: aScale = 0.24f; break;
	case 64: aScale = 0.25f; break;
	case 65: aScale = 0.24f; break;
	case 66: aScale = 0.24f; break;
	case 67: aScale = 0.24f; break;
	case 68: aScale = 0.24f; break;
	case 69: aScale = 0.24f; break;
	case 70: aScale = 0.25f; break;
	case 71: aScale = 0.24f; break;
	case 72: aScale = 0.24f; break;
	case 73: aScale = 0.24f; break;
	case 74: aScale = 0.24f; break;
	case 75: aScale = 0.25f; break;
	case 76: aScale = 0.24f; break;
	case 77: aScale = 0.25f; break;
	case 78: aScale = 0.24f; break;
	case 79: aScale = 0.24f; break;
	case 80: aScale = 0.24f; break;
	case 81: aScale = 0.24f; break;
	default:
	{
		int aMaxDim = (aImg->mWidth > aImg->mHeight) ? aImg->mWidth : aImg->mHeight;
		if (aMaxDim <= 0)
			aScale = 0.24f;
		else
			aScale = 46.0f / (float)aMaxDim;
		break;
	}
	}
	*theScaleX = aScale;
	*theScaleY = aScale;
}

static void GetPortraitItemOffset(int theItemIndex, int theCellX, int theCellY, int theCellW, int theCellH, float theScaleX, float theScaleY, int& theOutX, int& theOutY)
{
	Image* aImg = (theItemIndex >= 0 && theItemIndex < 116 && gPortraitItems[theItemIndex].mLine)
		? *gPortraitItems[theItemIndex].mLine : nullptr;
	if (!aImg)
	{
		theOutX = theCellX + theCellW / 2;
		theOutY = theCellY + theCellH / 2;
		return;
	}

	switch (theItemIndex)
	{
	case 0:
		theOutX = theCellX;
		theOutY = theCellY;
		return;
	case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11:
		theOutX = theCellX;
		theOutY = theCellY + (int)(theCellH * 0.1f);
		return;
	case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20:
	case 21: case 22: case 23: case 24: case 25:
		theOutX = theCellX + (int)(theCellW * 0.15f);
		theOutY = theCellY;
		return;
	case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34:
	case 35: case 36: case 37: case 38: case 39: case 40: case 41:
		theOutX = theCellX + (int)(theCellW * 0.1f);
		theOutY = theCellY;
		return;
	case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50:
	case 51: case 52: case 53: case 54: case 55: case 56: case 57:
		theOutX = theCellX + (int)(theCellW * 0.05f);
		theOutY = theCellY + (int)(theCellH * 0.25f);
		return;
	case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65: case 66:
	case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75:
	case 76: case 77: case 78: case 79: case 80: case 81:
		theOutX = theCellX;
		theOutY = theCellY + (int)(theCellH * 0.35f);
		return;
	case 82: case 83: case 84: case 85: case 86: case 87: case 88: case 89: case 90:
	case 91: case 92: case 93: case 94: case 95:
		theOutX = theCellX;
		theOutY = theCellY + (int)(theCellH * 0.2f);
		return;
	case 96: case 97: case 98: case 99: case 100: case 101: case 102: case 103: case 104:
	case 105: case 106: case 107: case 108: case 109: case 110:
		theOutX = theCellX + (int)(theCellW * 0.3f);
		theOutY = theCellY + (int)(theCellH * 0.4f);
		return;
	default:
	{
		int aScaledW = (int)(aImg->mWidth * theScaleX);
		int aScaledH = (int)(aImg->mHeight * theScaleY);
		theOutX = theCellX + (theCellW - aScaledW) / 2;
		theOutY = theCellY + (theCellH - aScaledH) / 2;
	}
	}
}

static int* GetSelectedItemPtr(ZombatarPage thePage, ZombatarConfig& theConfig)
{
	switch (thePage)
	{
	case PAGE_HAIR:			return &theConfig.mHair;
	case PAGE_FACIAL_HAIR:	return &theConfig.mFacialHair;
	case PAGE_TIDBITS:		return &theConfig.mTidbits;
	case PAGE_EYEWEAR:		return &theConfig.mEyewear;
	case PAGE_CLOTHES:		return &theConfig.mClothes;
	case PAGE_ACCESSORIES:	return &theConfig.mAccessories;
	case PAGE_HATS:			return &theConfig.mHat;
	case PAGE_BACKDROPS:	return &theConfig.mBackdrop;
	default:				return nullptr;
	}
}

static int* GetSelectedColorPtr(ZombatarPage thePage, ZombatarConfig& theConfig)
{
	switch (thePage)
	{
	case PAGE_HAIR:			return &theConfig.mHairColor;
	case PAGE_FACIAL_HAIR:	return &theConfig.mFacialHairColor;
	case PAGE_TIDBITS:		return &theConfig.mTidbitsColor;
	case PAGE_EYEWEAR:		return &theConfig.mEyewearColor;
	case PAGE_CLOTHES:		return &theConfig.mClothesColor;
	case PAGE_ACCESSORIES:	return &theConfig.mAccessoriesColor;
	case PAGE_HATS:			return &theConfig.mHatColor;
	case PAGE_BACKDROPS:	return &theConfig.mBackdropColor;
	default:				return nullptr;
	}
}

ZombatarWidget::ZombatarWidget(LawnApp* theApp)
{
	mApp = theApp;
	mState = STATE_AVATAR_CREATION;
	mCurrentPage = PAGE_SKIN;
	mPreviewZombie = nullptr;
	mSelectedAvatarIndex = -1;
	mScrollOffset = 0;
	mNeedsUpdate = true;
	mSubPage = 0;
	mMaxSubPages = 0;
	mTransitionTimer = 0;

	mLoadedResourceNames.push_back("DelayLoad_Zombatar");
	for (std::string& resource : mLoadedResourceNames)
		TodLoadResources(resource.c_str());

	mBackButton = new GameButton(ZombatarWidget_Back);
	mBackButton->mButtonImage = Sexy::IMAGE_BLANK;
	mBackButton->mOverImage = Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT;
	mBackButton->mDownImage = Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT;
	mBackButton->Resize(278, 528, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_MAINMENUBACK_HIGHLIGHT->mHeight);

	mNextButton = new GameButton(ZombatarWidget_Next);
	mNextButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON;
	mNextButton->mOverImage = Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT;
	mNextButton->mDownImage = Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT;
	mNextButton->Resize(467, 398, Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT->mHeight);

	mPrevButton = new GameButton(ZombatarWidget_Prev);
	mPrevButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_PREV_BUTTON;
	mPrevButton->mOverImage = Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT;
	mPrevButton->mDownImage = Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT;
	mPrevButton->Resize(120, 398, Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT->mHeight);

	mNewZombatarButton = new GameButton(ZombatarWidget_CreateNew);
	mNewZombatarButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON;
	mNewZombatarButton->mOverImage = Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT;
	mNewZombatarButton->mDownImage = Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT;
	mNewZombatarButton->Resize(195, 395, Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_NEWZOMBIE_BUTTON_HIGHLIGHT->mHeight);

	mViewButton = new GameButton(ZombatarWidget_ViewAvatar);
	mViewButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON;
	mViewButton->mOverImage = Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON_HIGHLIGHT;
	mViewButton->mDownImage = Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON_HIGHLIGHT;
	mViewButton->Resize(65, 472, Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_VIEW_BUTTON_HIGHLIGHT->mHeight);

	mFinishedButton = new GameButton(ZombatarWidget_Finished);
	mFinishedButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON;
	mFinishedButton->mOverImage = Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT;
	mFinishedButton->mDownImage = Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT;
	mFinishedButton->Resize(445, 472, Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_FINISHED_BUTTON_HIGHLIGHT->mHeight);

	mAvatarBackButton = new GameButton(ZombatarWidget_BackAvatar);
	mAvatarBackButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_BACK_BUTTON;
	mAvatarBackButton->mOverImage = Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT;
	mAvatarBackButton->mDownImage = Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT;
	mAvatarBackButton->Resize(385, 345, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_BACK_BUTTON_HIGHLIGHT->mHeight);

	mDeleteZombatarButton = new GameButton(ZombatarWidget_DeleteAvatar);
	mDeleteZombatarButton->mButtonImage = Sexy::IMAGE_BLANK;
	mDeleteZombatarButton->mOverImage = Sexy::IMAGE_BLANK;
	mDeleteZombatarButton->mDownImage = Sexy::IMAGE_BLANK;
	mDeleteZombatarButton->SetLabel("[ZOMBATAR_DELETE_BUTTON2]");
	mDeleteZombatarButton->SetFont(Sexy::FONT_DWARVENTODCRAFT12);
	mDeleteZombatarButton->mColors[0] = Color(255, 255, 255);
	mDeleteZombatarButton->mColors[1] = Color(0, 255, 40);
	mDeleteZombatarButton->Resize(337, 145, Sexy::FONT_DWARVENTODCRAFT12->StringWidth(mDeleteZombatarButton->mLabel), 20);

	mNextPageButton = new GameButton(ZombatarWidget_NextPage);
	mNextPageButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON;
	mNextPageButton->mOverImage = Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT;
	mNextPageButton->mDownImage = Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT;
	mNextPageButton->Resize(497, 372, Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_NEXT_BUTTON_HIGHLIGHT->mHeight);

	mPrevPageButton = new GameButton(ZombatarWidget_PrevPage);
	mPrevPageButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_PREV_BUTTON;
	mPrevPageButton->mOverImage = Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT;
	mPrevPageButton->mDownImage = Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT;
	mPrevPageButton->Resize(175, 372, Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_PREV_BUTTON_HIGHLIGHT->mHeight);

	mToggledButton = mSkinButton = new GameButton(ZombatarWidget_Skin);
	mSkinButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
	mSkinButton->mOverImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
	mSkinButton->mDownImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
	mSkinButton->Resize(58, 128, Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT->mHeight);

	mHairButton = new GameButton(ZombatarWidget_Hair);
	mHairButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON;
	mHairButton->mOverImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
	mHairButton->mDownImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
	mHairButton->Resize(58, 164, Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT->mHeight);

	mFacialHairButton = new GameButton(ZombatarWidget_FacialHair);
	mFacialHairButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON;
	mFacialHairButton->mOverImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
	mFacialHairButton->mDownImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
	mFacialHairButton->Resize(58, 200, Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT->mHeight);

	mTidbitsButton = new GameButton(ZombatarWidget_Tidbits);
	mTidbitsButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON;
	mTidbitsButton->mOverImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
	mTidbitsButton->mDownImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
	mTidbitsButton->Resize(58, 236, Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT->mHeight);

	mEyewearButton = new GameButton(ZombatarWidget_Eyewear);
	mEyewearButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON;
	mEyewearButton->mOverImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
	mEyewearButton->mDownImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
	mEyewearButton->Resize(58, 272, Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT->mHeight);

	mClothesButton = new GameButton(ZombatarWidget_Clothes);
	mClothesButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON;
	mClothesButton->mOverImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
	mClothesButton->mDownImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
	mClothesButton->Resize(58, 308, Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT->mHeight);

	mAccessoriesButton = new GameButton(ZombatarWidget_Accessories);
	mAccessoriesButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON;
	mAccessoriesButton->mOverImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
	mAccessoriesButton->mDownImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
	mAccessoriesButton->Resize(58, 344, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT->mHeight);

	mHatsButton = new GameButton(ZombatarWidget_Hats);
	mHatsButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON;
	mHatsButton->mOverImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
	mHatsButton->mDownImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
	mHatsButton->Resize(58, 380, Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT->mHeight);

	mBackdropsButton = new GameButton(ZombatarWidget_Backdrops);
	mBackdropsButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON;
	mBackdropsButton->mOverImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;
	mBackdropsButton->mDownImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;
	mBackdropsButton->Resize(58, 416, Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT->mWidth, Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT->mHeight);

	for (int i = 0; i < 18; i++)
	{
		mColorRects[i] = Rect(238 + (i % 9) * (Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth + 4),
							  367 + (i / 9) * (Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight + 4),
							  Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth,
							  Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight);
	}

	for (int i = 0; i < 18; i++)
	{
		mItemRects[i] = Rect(166 + (i % 6) * (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth - 4),
							  137 + (i / 6) * (Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight - 1),
							  Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth,
							  Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight);
	}

	ResetConfig();
	ChangePage(mCurrentPage);
	ChangeState(STATE_AVATAR_CREATION);
}

ZombatarWidget::~ZombatarWidget()
{
	delete mBackButton;
	delete mNextButton;
	delete mPrevButton;
	delete mNewZombatarButton;
	delete mViewButton;
	delete mFinishedButton;
	delete mAvatarBackButton;
	delete mDeleteZombatarButton;
	delete mNextPageButton;
	delete mPrevPageButton;
	delete mSkinButton;
	delete mHairButton;
	delete mFacialHairButton;
	delete mTidbitsButton;
	delete mEyewearButton;
	delete mClothesButton;
	delete mAccessoriesButton;
	delete mHatsButton;
	delete mBackdropsButton;
	if (mPreviewZombie)
	{
		mPreviewZombie->DieNoLoot();
		delete mPreviewZombie;
	}
}

int ZombatarWidget::GetItemCount(ZombatarPage thePage)
{
	switch (thePage)
	{
	case PAGE_HAIR:			return NUM_HAIR_ITEMS;
	case PAGE_FACIAL_HAIR:	return (mSubPage == 0) ? 17 : 7;
	case PAGE_TIDBITS:		return NUM_TIDBITS_ITEMS;
	case PAGE_EYEWEAR:		return NUM_EYEWEAR_ITEMS;
	case PAGE_CLOTHES:		return NUM_CLOTHES_ITEMS;
	case PAGE_ACCESSORIES:	return NUM_ACCESSORIES_ITEMS;
	case PAGE_HATS:			return NUM_HAT_ITEMS;
	case PAGE_BACKDROPS:	return NUM_BACKDROP_ITEMS;
	default:				return 0;
	}
}

int ZombatarWidget::GetPageColorIndex(ZombatarPage thePage)
{
	switch (thePage)
	{
	case PAGE_SKIN:			return mCurrentConfig.mSkinColor;
	case PAGE_HAIR:			return mCurrentConfig.mHairColor;
	case PAGE_FACIAL_HAIR:	return mCurrentConfig.mFacialHairColor;
	case PAGE_TIDBITS:		return mCurrentConfig.mTidbitsColor;
	case PAGE_EYEWEAR:		return mCurrentConfig.mEyewearColor;
	case PAGE_CLOTHES:		return mCurrentConfig.mClothesColor;
	case PAGE_ACCESSORIES:	return mCurrentConfig.mAccessoriesColor;
	case PAGE_HATS:			return mCurrentConfig.mHatColor;
	case PAGE_BACKDROPS:	return mCurrentConfig.mBackdropColor;
	}
	return -1;
}

int ZombatarWidget::GetPageItemIndex(ZombatarPage thePage)
{
	switch (thePage)
	{
	case PAGE_HAIR:			return mCurrentConfig.mHair;
	case PAGE_FACIAL_HAIR:	return mCurrentConfig.mFacialHair;
	case PAGE_TIDBITS:		return mCurrentConfig.mTidbits;
	case PAGE_EYEWEAR:		return mCurrentConfig.mEyewear;
	case PAGE_CLOTHES:		return mCurrentConfig.mClothes;
	case PAGE_ACCESSORIES:	return mCurrentConfig.mAccessories;
	case PAGE_HATS:			return mCurrentConfig.mHat;
	case PAGE_BACKDROPS:	return mCurrentConfig.mBackdrop;
	}
	return -1;
}

void ZombatarWidget::ResetConfig()
{
	mCurrentConfig = ZombatarConfig();
	mCurrentPage = PAGE_SKIN;
	mScrollOffset = 0;
	mNeedsUpdate = true;
}

void ZombatarWidget::UpdatePreview()
{
	mNeedsUpdate = true;
}

void ZombatarWidget::SetupPreviewZombie()
{
}

void ZombatarWidget::ChangePage(ZombatarPage thePage)
{
	mSubPage = 0;

	switch (mCurrentPage)
	{
	case PAGE_SKIN:
		mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
		mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
		mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON;
		break;
	case PAGE_HAIR:
		mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON;
		mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
		mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_OVER;
		break;
	case PAGE_FACIAL_HAIR:
		mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON;
		mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
		mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_OVER;
		break;
	case PAGE_TIDBITS:
		mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON;
		mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
		mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_OVER;
		break;
	case PAGE_EYEWEAR:
		mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON;
		mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
		mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_OVER;
		break;
	case PAGE_CLOTHES:
		mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON;
		mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
		mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_OVER;
		break;
	case PAGE_ACCESSORIES:
		mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON;
		mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
		mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_OVER;
		break;
	case PAGE_HATS:
		mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON;
		mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
		mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_OVER;
		break;
	case PAGE_BACKDROPS:
		mToggledButton->mButtonImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON;
		mToggledButton->mOverImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;
		mToggledButton->mDownImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_OVER;
		break;
	}
	mToggledButton->mDisabled = false;

	mCurrentPage = thePage;
	mScrollOffset = 0;
	mNeedsUpdate = true;

	if (thePage == PAGE_FACIAL_HAIR)
		mMaxSubPages = 1;
	else
		mMaxSubPages = 0;

	switch (mCurrentPage)
	{
	case PAGE_SKIN:			mToggledButton = mSkinButton; break;
	case PAGE_HAIR:			mToggledButton = mHairButton; break;
	case PAGE_FACIAL_HAIR:	mToggledButton = mFacialHairButton; break;
	case PAGE_TIDBITS:		mToggledButton = mTidbitsButton; break;
	case PAGE_EYEWEAR:		mToggledButton = mEyewearButton; break;
	case PAGE_CLOTHES:		mToggledButton = mClothesButton; break;
	case PAGE_ACCESSORIES:	mToggledButton = mAccessoriesButton; break;
	case PAGE_HATS:			mToggledButton = mHatsButton; break;
	case PAGE_BACKDROPS:	mToggledButton = mBackdropsButton; break;
	}

	Image* aHighlightImage = nullptr;
	switch (mCurrentPage)
	{
	case PAGE_SKIN:			aHighlightImage = Sexy::IMAGE_ZOMBATAR_SKIN_BUTTON_HIGHLIGHT; break;
	case PAGE_HAIR:			aHighlightImage = Sexy::IMAGE_ZOMBATAR_HAIR_BUTTON_HIGHLIGHT; break;
	case PAGE_FACIAL_HAIR:	aHighlightImage = Sexy::IMAGE_ZOMBATAR_FACIAL_HAIR_BUTTON_HIGHLIGHT; break;
	case PAGE_TIDBITS:		aHighlightImage = Sexy::IMAGE_ZOMBATAR_TIDBITS_BUTTON_HIGHLIGHT; break;
	case PAGE_EYEWEAR:		aHighlightImage = Sexy::IMAGE_ZOMBATAR_EYEWEAR_BUTTON_HIGHLIGHT; break;
	case PAGE_CLOTHES:		aHighlightImage = Sexy::IMAGE_ZOMBATAR_CLOTHES_BUTTON_HIGHLIGHT; break;
	case PAGE_ACCESSORIES:	aHighlightImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BUTTON_HIGHLIGHT; break;
	case PAGE_HATS:			aHighlightImage = Sexy::IMAGE_ZOMBATAR_HATS_BUTTON_HIGHLIGHT; break;
	case PAGE_BACKDROPS:	aHighlightImage = Sexy::IMAGE_ZOMBATAR_BACKDROPS_BUTTON_HIGHLIGHT; break;
	}

	mToggledButton->mButtonImage = aHighlightImage;
	mToggledButton->mOverImage = aHighlightImage;
	mToggledButton->mDownImage = aHighlightImage;
	mToggledButton->mDisabled = true;

	mNextPageButton->mDisabled = mState != STATE_AVATAR_CREATION || mSubPage == mMaxSubPages || mCurrentPage != PAGE_FACIAL_HAIR;
	mPrevPageButton->mDisabled = mState != STATE_AVATAR_CREATION || mSubPage == 0 || mCurrentPage != PAGE_FACIAL_HAIR;

	MarkDirty();
}

void ZombatarWidget::ChangeState(ZombatarState theNewState)
{
	if (theNewState == STATE_AVATAR_CREATION)
	{
		mFinishedButton->mX = 445;
		mFinishedButton->mY = 472;
	}
	else if (theNewState == STATE_CONFIRM)
	{
		mFinishedButton->mX = 155;
		mFinishedButton->mY = 345;
	}

	mState = theNewState;
	mTransitionTimer = 0;

	mViewButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mSkinButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mHairButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mFacialHairButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mTidbitsButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mEyewearButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mClothesButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mHatsButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mAccessoriesButton->mDisabled = mState != STATE_AVATAR_CREATION;
	mBackdropsButton->mDisabled = mState != STATE_AVATAR_CREATION;

	if (mState == STATE_AVATAR_CREATION)
	{
		ChangePage(mCurrentPage);
	}

	mAvatarBackButton->mDisabled = mState != STATE_CONFIRM;
	mFinishedButton->mDisabled = mState == STATE_AVATAR_LIST;
	mNextPageButton->mDisabled = mState != STATE_AVATAR_CREATION || mSubPage == mMaxSubPages || mCurrentPage != PAGE_FACIAL_HAIR;
	mPrevPageButton->mDisabled = mState != STATE_AVATAR_CREATION || mSubPage == 0 || mCurrentPage != PAGE_FACIAL_HAIR;
	mNewZombatarButton->mDisabled = mState != STATE_AVATAR_LIST;
	mNextButton->mDisabled = mState != STATE_AVATAR_LIST || (mApp->mPlayerInfo && mSelectedAvatarIndex >= GetAvatarCount() - 1);
	mPrevButton->mDisabled = mState != STATE_AVATAR_LIST || (mApp->mPlayerInfo && mSelectedAvatarIndex <= 0);
	mDeleteZombatarButton->mDisabled = mState != STATE_AVATAR_LIST;

	MarkDirty();
}

void ZombatarWidget::Update()
{
	Widget::Update();
	MarkDirty();

	mBackButton->Update();
	mNextButton->Update();
	mPrevButton->Update();
	mNewZombatarButton->Update();
	mViewButton->Update();
	mFinishedButton->Update();
	mAvatarBackButton->Update();
	mDeleteZombatarButton->Update();
	mNextPageButton->Update();
	mPrevPageButton->Update();
	mSkinButton->Update();
	mHairButton->Update();
	mFacialHairButton->Update();
	mTidbitsButton->Update();
	mEyewearButton->Update();
	mClothesButton->Update();
	mAccessoriesButton->Update();
	mHatsButton->Update();
	mBackdropsButton->Update();

	if (mPreviewZombie)
		mPreviewZombie->Update();

	int aCurrentItemIndex = GetPageItemIndex(mCurrentPage);
	bool anOverlapsItem = false;

	for (int i = 0; i < GetItemCount(mCurrentPage) + 1; i++)
	{
		if (mCurrentPage == PAGE_BACKDROPS && GetItemCount(mCurrentPage) == i)
			continue;
		if (mItemRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY) && i != aCurrentItemIndex)
		{
			anOverlapsItem = mState == STATE_AVATAR_CREATION;
			break;
		}
	}

	bool anOverlapsColor = false;
	if (mCurrentPage == PAGE_SKIN || (aCurrentItemIndex != -1 && gPortraitItems[ItemTypeOffset(mCurrentPage) + aCurrentItemIndex].mAllowColor))
	{
		int aMaxColor = (mCurrentPage == PAGE_SKIN) ? 12 : 18;
		int aCurrentItemColor = GetPageColorIndex(mCurrentPage);
		for (int i = 0; i < aMaxColor; i++)
		{
			if (mColorRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY) && i != aCurrentItemColor)
			{
				anOverlapsColor = mState == STATE_AVATAR_CREATION;
				break;
			}
		}
	}

	if (mHasFocus)
	{
		if (mViewButton->IsMouseOver() || mBackButton->IsMouseOver() || mFinishedButton->IsMouseOver() ||
			mSkinButton->IsMouseOver() || mHairButton->IsMouseOver() || mFacialHairButton->IsMouseOver() ||
			mTidbitsButton->IsMouseOver() || mEyewearButton->IsMouseOver() || mClothesButton->IsMouseOver() ||
			mAccessoriesButton->IsMouseOver() || mHatsButton->IsMouseOver() || mBackdropsButton->IsMouseOver() ||
			mAvatarBackButton->IsMouseOver() || mNewZombatarButton->IsMouseOver() || mNextButton->IsMouseOver() ||
			mPrevButton->IsMouseOver() || mDeleteZombatarButton->IsMouseOver() || mPrevPageButton->IsMouseOver() ||
			mNextPageButton->IsMouseOver() || anOverlapsColor || anOverlapsItem)
		{
			mApp->SetCursor(CURSOR_HAND);
		}
		else
		{
			mApp->SetCursor(CURSOR_POINTER);
		}
	}
}

void ZombatarWidget::DrawItemGrid(Graphics* g)
{
	int aCellW = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mWidth;
	int aCellH = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG->mHeight;
	int aGridX = 166;
	int aGridY = 137;
	int aColSpacing = aCellW - 4;
	int aRowSpacing = aCellH - 1;
	int aCols = 6;
	int aRows = 3;

	g->DrawImage(Sexy::IMAGE_ZOMBATAR_WIDGET_INNER_BG, aGridX - 5, aGridY - 5);

	if (mCurrentPage == PAGE_SKIN)
	{
		for (int i = 0; i < NUM_SKIN_COLORS; i++)
		{
			int aCol = i % aCols;
			int aRow = i / aCols;
			int aX = aGridX + aCol * aColSpacing;
			int aY = aGridY + aRow * aRowSpacing;

			g->DrawImage(Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG, aX, aY);
			g->SetColor(gSkinColors[i]);
			g->FillRect(aX + 8, aY + 8, aCellW - 16, aCellH - 16);

			if (mCurrentConfig.mSkinColor == i)
				g->DrawImage(Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG_HIGHLIGHT, aX, aY);
		}
		return;
	}

	int aItemCount = GetItemCount(mCurrentPage);
	int* aSelectedItem = GetSelectedItemPtr(mCurrentPage, mCurrentConfig);
	bool isBackdrops = (mCurrentPage == PAGE_BACKDROPS);
	int aBaseOffset = ItemTypeOffset(mCurrentPage);

	for (int i = 0; i < aCols * aRows; i++)
	{
		int aItemIndex = i + mScrollOffset;

		int aCol = i % aCols;
		int aRow = i / aCols;
		int aX = aGridX + aCol * aColSpacing;
		int aY = aGridY + aRow * aRowSpacing;

		if (aItemIndex < aItemCount)
		{
			Image* aBGImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG;
			int aConfigIndex = aItemIndex;
			if (mCurrentPage == PAGE_FACIAL_HAIR)
				aConfigIndex += mSubPage * 17;
			bool aIsSelected = (aSelectedItem && *aSelectedItem == aConfigIndex);
			bool aIsHovered = mItemRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY);

			if (aIsSelected)
			{
				aBGImage = Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG_HIGHLIGHT;
				g->SetColor(Color::White);
			}
			else
			{
				g->SetColor(aIsHovered ? Color::White : Color(155, 155, 175));
			}

			g->SetColorizeImages(true);
			g->DrawImage(aBGImage, mItemRects[i], Rect(0, 0, aCellW, aCellH));
			g->SetColorizeImages(false);

			int aGlobalIndex = aBaseOffset + aItemIndex;
			if (mCurrentPage == PAGE_FACIAL_HAIR)
				aGlobalIndex += mSubPage * 17;
			const PortraitItem& aItem = gPortraitItems[aGlobalIndex];

			if (mCurrentPage == PAGE_CLOTHES)
			{
				Rect aClipRect(aX + 8, aY + 8, aCellW - 20, aCellH - 20);
				g->ClipRect(aClipRect);
				g->SetColor(Color(gSkinColors[0].mRed, gSkinColors[0].mGreen, gSkinColors[0].mBlue, g->mColor.mAlpha));
				float aScaleX = (float)aCellW / Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN->mWidth;
				float aScaleY = (float)aCellH / Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN->mHeight;
				TodDrawImageScaledF(g, Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN, aX - 10, aY - 10, aScaleX, aScaleY);
				g->SetColor(Color(255, 255, 255, g->mColor.mAlpha));
				TodDrawImageScaledF(g, Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK, aX - 10, aY - 10, aScaleX, aScaleY);
				g->ClearClipRect();
			}

			if (aItem.mLine && *aItem.mLine)
			{
				float aScaleX, aScaleY;
				GetPortraitItemScale(aGlobalIndex, &aScaleX, &aScaleY);
				int aDrawX, aDrawY;
				GetPortraitItemOffset(aGlobalIndex, aX + 9, aY + 9, aCellW - 9, aCellH - 9, aScaleX, aScaleY, aDrawX, aDrawY);
				bool aDrewTinted = false;

				if (aItem.mColor && *aItem.mColor && aItem.mAllowColor)
				{
					int* aSelectedColor = GetSelectedColorPtr(mCurrentPage, mCurrentConfig);
					int aColorIdx = (aSelectedColor) ? *aSelectedColor : 17;
					if (aColorIdx >= 0 && aColorIdx < NUM_MORE_COLORS - 1)
					{
						g->SetColor(gMoreColors[aColorIdx]);
						g->SetColorizeImages(true);
						TodDrawImageScaledF(g, *aItem.mColor, (float)aDrawX, (float)aDrawY, aScaleX, aScaleY);
						g->SetColorizeImages(false);
					}
				}
				else if (aItem.mAllowColor && (!aItem.mColor || !*aItem.mColor))
				{
					int* aSelectedColor = GetSelectedColorPtr(mCurrentPage, mCurrentConfig);
					int aColorIdx = (aSelectedColor) ? *aSelectedColor : 17;
					if (aColorIdx >= 0 && aColorIdx < NUM_MORE_COLORS - 1)
					{
						g->SetColor(gMoreColors[aColorIdx]);
						g->SetColorizeImages(true);
						TodDrawImageScaledF(g, *aItem.mLine, (float)aDrawX, (float)aDrawY, aScaleX, aScaleY);
						g->SetColorizeImages(false);
						aDrewTinted = true;
					}
				}

				if (!aDrewTinted)
					TodDrawImageScaledF(g, *aItem.mLine, (float)aDrawX, (float)aDrawY, aScaleX, aScaleY);
			}
		}
		else if (!isBackdrops && aItemIndex == aItemCount)
		{
			bool aIsSelected = (aSelectedItem && *aSelectedItem == -1);
			bool aIsHovered = mItemRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY);

			if (aIsSelected)
				g->SetColor(Color::White);
			else
				g->SetColor(aIsHovered ? Color::White : Color(155, 155, 155));

			g->SetColorizeImages(true);
			g->DrawImage(Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG, mItemRects[i], Rect(0, 0, aCellW, aCellH));
			g->SetColorizeImages(false);

			if (aIsSelected)
				g->DrawImage(Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG_HIGHLIGHT, aX, aY);
			else
				g->DrawImage(Sexy::IMAGE_ZOMBATAR_ACCESSORY_BG_NONE, aX, aY);
		}
	}
}

void ZombatarWidget::DrawColorPalette(Graphics* g)
{
	int aSwatchW = Sexy::IMAGE_ZOMBATAR_COLORPICKER->mWidth;
	int aSwatchH = Sexy::IMAGE_ZOMBATAR_COLORPICKER->mHeight;
	int aPaletteX = 238;
	int aPaletteY = 367;
	int aColSpacing = aSwatchW + 4;
	int aRowSpacing = aSwatchH + 4;
	int aCols = 9;

	g->DrawImage(Sexy::IMAGE_ZOMBATAR_COLORS_BG, aPaletteX - 5, aPaletteY - 5);

	if (mCurrentPage == PAGE_SKIN)
	{
		for (int i = 0; i < NUM_SKIN_COLORS; i++)
		{
			int aCol = i % aCols;
			int aRow = i / aCols;
			int aX = aPaletteX + aCol * aColSpacing;
			int aY = aPaletteY + aRow * aRowSpacing;

			g->SetColorizeImages(true);
			if (mCurrentConfig.mSkinColor != i)
			{
				g->SetColor(gSkinColors[i]);
				g->mColor.mAlpha = 66;
				if (mColorRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY))
					g->mColor.mAlpha = 127;
			}
			else
			{
				g->SetColor(Color::White);
			}

			g->DrawImage(Sexy::IMAGE_ZOMBATAR_COLORPICKER, mColorRects[i], Rect(0, 0, aSwatchW, aSwatchH));
			g->SetColorizeImages(false);
		}
	}
	else
	{
		int* aSelectedColor = GetSelectedColorPtr(mCurrentPage, mCurrentConfig);
		int* aSelectedItem = GetSelectedItemPtr(mCurrentPage, mCurrentConfig);
		bool aItemHasColor = false;

		if (aSelectedItem && *aSelectedItem >= 0)
		{
			int aGlobalIndex = ItemTypeOffset(mCurrentPage) + *aSelectedItem;
			if (aGlobalIndex >= 0 && aGlobalIndex < 116)
				aItemHasColor = gPortraitItems[aGlobalIndex].mAllowColor;
		}

		if (!aItemHasColor)
			return;

		for (int i = 0; i < NUM_MORE_COLORS; i++)
		{
			int aCol = i % aCols;
			int aRow = i / aCols;
			int aX = aPaletteX + aCol * aColSpacing;
			int aY = aPaletteY + aRow * aRowSpacing;

			g->SetColorizeImages(true);
			if (aSelectedColor && *aSelectedColor != i)
			{
				g->SetColor(i == 17 ? Color::White : gMoreColors[i]);
				g->mColor.mAlpha = 66;
				if (mColorRects[i].Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY))
					g->mColor.mAlpha = 127;
			}
			else
			{
				g->SetColor(Color::White);
			}

			g->DrawImage(
				i == 17 ? Sexy::IMAGE_ZOMBATAR_COLORPICKER_NONE : Sexy::IMAGE_ZOMBATAR_COLORPICKER,
				mColorRects[i], Rect(0, 0, aSwatchW, aSwatchH));
			g->SetColorizeImages(false);
		}
	}
}

void ZombatarWidget::DrawPreview(Graphics* g)
{
	int aOriginX = 592;
	int aOriginY = 115;
	int aSkinX = aOriginX + 38;
	int aSkinY = aOriginY + 40;

	g->DrawImage(Sexy::IMAGE_ZOMBATAR_DISPLAY_WINDOW, aOriginX, aOriginY);

	if (mCurrentConfig.mBackdrop >= 0 && mCurrentConfig.mBackdrop < NUM_BACKDROP_ITEMS)
	{
		int aBackdropIdx = 111 + mCurrentConfig.mBackdrop;
		if (aBackdropIdx >= 0 && aBackdropIdx < 116)
		{
			Image* aBackdrop = gPortraitItems[aBackdropIdx].mLine ? *gPortraitItems[aBackdropIdx].mLine : nullptr;
			if (aBackdrop)
				g->DrawImage(aBackdrop, aOriginX, aOriginY);
		}
	}

	g->SetColor(gSkinColors[mCurrentConfig.mSkinColor]);
	g->SetColorizeImages(true);
	g->DrawImage(Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN, aSkinX, aSkinY);
	g->SetColorizeImages(false);

	g->DrawImage(Sexy::IMAGE_ZOMBATAR_ZOMBIE_BLANK, aSkinX, aSkinY);

	auto drawItem = [&](int theItemIndex, int theColorIndex) {
		if (theItemIndex < 0 || theItemIndex >= 116)
			return;
		const PortraitItem& aItem = gPortraitItems[theItemIndex];
		float aDrawX = (float)(aSkinX) + aItem.mOffsetX;
		float aDrawY = (float)(aSkinY) + aItem.mOffsetY;
		if (aItem.mColor && *aItem.mColor && theColorIndex >= 0 && theColorIndex < NUM_MORE_COLORS - 1)
		{
			g->SetColor(gMoreColors[theColorIndex]);
			g->SetColorizeImages(true);
			g->DrawImage(*aItem.mColor, (int)(aDrawX + aItem.mColorOffsetX), (int)(aDrawY + aItem.mColorOffsetY));
			g->SetColorizeImages(false);
		}
		else if (aItem.mAllowColor && (!aItem.mColor || !*aItem.mColor) && theColorIndex >= 0 && theColorIndex < NUM_MORE_COLORS - 1 && aItem.mLine && *aItem.mLine)
		{
			g->SetColor(gMoreColors[theColorIndex]);
			g->SetColorizeImages(true);
			g->DrawImage(*aItem.mLine, (int)aDrawX, (int)aDrawY);
			g->SetColorizeImages(false);
			return;
		}
		if (aItem.mLine && *aItem.mLine)
			g->DrawImage(*aItem.mLine, (int)aDrawX, (int)aDrawY);
	};

	if (mCurrentConfig.mClothes >= 0 && mCurrentConfig.mClothes < NUM_CLOTHES_ITEMS)
		drawItem(ItemTypeOffset(PAGE_CLOTHES) + mCurrentConfig.mClothes, mCurrentConfig.mClothesColor);

	if (mCurrentConfig.mAccessories >= 0 && mCurrentConfig.mAccessories < NUM_ACCESSORIES_ITEMS)
		drawItem(ItemTypeOffset(PAGE_ACCESSORIES) + mCurrentConfig.mAccessories, mCurrentConfig.mAccessoriesColor);

	if (mCurrentConfig.mTidbits >= 0 && mCurrentConfig.mTidbits < NUM_TIDBITS_ITEMS)
		drawItem(ItemTypeOffset(PAGE_TIDBITS) + mCurrentConfig.mTidbits, mCurrentConfig.mTidbitsColor);

	if (mCurrentConfig.mFacialHair >= 0 && mCurrentConfig.mFacialHair < NUM_FACIAL_HAIR_ITEMS)
		drawItem(ItemTypeOffset(PAGE_FACIAL_HAIR) + mCurrentConfig.mFacialHair, mCurrentConfig.mFacialHairColor);

	if (mCurrentConfig.mHair >= 0 && mCurrentConfig.mHair < NUM_HAIR_ITEMS)
		drawItem(ItemTypeOffset(PAGE_HAIR) + mCurrentConfig.mHair, mCurrentConfig.mHairColor);

	if (mCurrentConfig.mEyewear >= 0 && mCurrentConfig.mEyewear < NUM_EYEWEAR_ITEMS)
		drawItem(ItemTypeOffset(PAGE_EYEWEAR) + mCurrentConfig.mEyewear, mCurrentConfig.mEyewearColor);

	if (mCurrentConfig.mHat >= 0 && mCurrentConfig.mHat < NUM_HAT_ITEMS)
		drawItem(ItemTypeOffset(PAGE_HATS) + mCurrentConfig.mHat, mCurrentConfig.mHatColor);
}

void ZombatarWidget::Draw(Graphics* g)
{
	g->SetLinearBlend(true);

	g->DrawImage(Sexy::IMAGE_ZOMBATAR_MAIN_BG, 0, 0);
	mBackButton->Draw(g);

	if (mState == STATE_AVATAR_LIST)
	{
		g->DrawImage(Sexy::IMAGE_ZOMBATAR_BACKGROUND_MENU, 0, 0);
		g->DrawImage(Sexy::IMAGE_ZOMBATAR_LOGO, 200, 30);

		TodDrawString(g, "[ZOMBATAR_START_TEXT]", 400, 100, Sexy::FONT_DWARVENTODCRAFT15, Color::White, DS_ALIGN_CENTER);

		int aCount = GetAvatarCount();
		if (aCount == 0)
		{
			TodDrawString(g, "[ZOMBATAR_COLOR_ITEM_NOT_CHOSEN]", 400, 250, Sexy::FONT_BRIANNETOD12, Color(200, 200, 200), DS_ALIGN_CENTER);
		}

		if (mSelectedAvatarIndex >= 0)
		{
			Rect aClipRect(58, 113, 497, 365);
			g->ClipRect(aClipRect);

			LoadAvatar(mSelectedAvatarIndex);
			DrawPreview(g);

			g->ClearClipRect();

			int aPageNum = mSelectedAvatarIndex + 1;
			std::string aPageStr = StrFormat("[ZOMBATAR_PAGE] %d / %d", aPageNum, aCount);
			TodDrawString(g, aPageStr, 300, 400, Sexy::FONT_BRIANNETOD12, Color::White, DS_ALIGN_CENTER);
		}

		mNewZombatarButton->Draw(g);
		if (!mNextButton->mDisabled)
			mNextButton->Draw(g);
		if (!mPrevButton->mDisabled)
			mPrevButton->Draw(g);
		mDeleteZombatarButton->Draw(g);
		return;
	}

	g->DrawImage(Sexy::IMAGE_ZOMBATAR_WIDGET_BG, 0, 0);

	DrawItemGrid(g);
	DrawColorPalette(g);
	DrawPreview(g);

	if (mCurrentPage == PAGE_SKIN)
	{
		TodDrawStringWrapped(g, "[ZOMBATAR_START_TEXT]", Rect(145, 185, 420, 120), Sexy::FONT_DWARVENTODCRAFT15,
							 Color(208, 190, 44), DS_ALIGN_CENTER);
	}
	else
	{
		int* aSelectedColor = GetSelectedColorPtr(mCurrentPage, mCurrentConfig);
		int* aSelectedItem = GetSelectedItemPtr(mCurrentPage, mCurrentConfig);
		bool aItemHasColor = false;

		if (aSelectedItem && *aSelectedItem >= 0)
		{
			int aGlobalIndex = ItemTypeOffset(mCurrentPage) + *aSelectedItem;
			if (aGlobalIndex >= 0 && aGlobalIndex < 116)
				aItemHasColor = gPortraitItems[aGlobalIndex].mAllowColor;
		}

		if (!aItemHasColor)
		{
			TodDrawString(g, "[ZOMBATAR_COLOR_ITEM_NOT_CHOSEN]", 349, 394, Sexy::FONT_BRIANNETOD12, Color(255, 255, 255), DS_ALIGN_CENTER);
		}
		else if (aSelectedColor && *aSelectedColor == 17)
		{
			TodDrawString(g, "[ZOMBATAR_COLOR_NOT_APPLICABLE]", 349, 394, Sexy::FONT_BRIANNETOD12, Color(255, 255, 255), DS_ALIGN_CENTER);
		}
	}

	if (mMaxSubPages > 0)
	{
		std::string aPageStr = StrFormat("[ZOMBATAR_PAGE] %d / %d", mSubPage + 1, mMaxSubPages + 1);
		TodDrawString(g, aPageStr, 348, 441, Sexy::FONT_BRIANNETOD12, Color(0, 0, 0), DS_ALIGN_CENTER);

		g->SetColorizeImages(true);
		g->SetColor(Color::White);
		g->PushState();
		if (mNextPageButton->mDisabled)
			g->mColor.mAlpha = 44;
		mNextPageButton->Draw(g);
		g->PopState();
		g->PushState();
		if (mPrevPageButton->mDisabled)
			g->mColor.mAlpha = 44;
		mPrevPageButton->Draw(g);
		g->PopState();
		g->SetColorizeImages(false);
	}

	mSkinButton->Draw(g);
	mHairButton->Draw(g);
	mFacialHairButton->Draw(g);
	mTidbitsButton->Draw(g);
	mEyewearButton->Draw(g);
	mClothesButton->Draw(g);
	mAccessoriesButton->Draw(g);
	mHatsButton->Draw(g);
	mBackdropsButton->Draw(g);

	if (mState == STATE_AVATAR_CREATION)
	{
		mViewButton->Draw(g);
	}

	if (mState == STATE_CONFIRM)
	{
		mAvatarBackButton->Draw(g);
		TodDrawString(g, "[ZOMBATAR_FINISHED_WARNING_HEADER]", 305, 185, Sexy::FONT_HOUSEOFTERROR28, Color(254, 227, 0), DS_ALIGN_CENTER);
		TodDrawString(g, "[ZOMBATAR_FINISHED_BUTTON_TEXT]", 195, 335, Sexy::FONT_BRIANNETOD12, Color(255, 255, 255), DS_ALIGN_CENTER);
		TodDrawString(g, "[ZOMBATAR_BACK_BUTTON_TEXT]", 435, 335, Sexy::FONT_BRIANNETOD12, Color(255, 255, 255), DS_ALIGN_CENTER);
		TodDrawStringWrapped(g, "[ZOMBATAR_FINISHED_WARNING_TEXT]", Rect(70, 225, 480, 80), Sexy::FONT_CONTINUUMBOLD14, Color(255, 255, 255), DS_ALIGN_CENTER);
	}

	mFinishedButton->Draw(g);
}

void ZombatarWidget::MouseDown(int x, int y, int theClickCount)
{
}

void ZombatarWidget::MouseUp(int x, int y, int theClickCount)
{
	if (mBackButton->IsMouseOver())
	{
		if (mState == STATE_AVATAR_CREATION)
		{
			ChangeState(STATE_AVATAR_LIST);
		}
		else if (mState == STATE_AVATAR_LIST)
		{
			if (mWidgetManager)
				mWidgetManager->RemoveWidget(this);
		}
		else if (mState == STATE_CONFIRM)
		{
			ChangeState(STATE_AVATAR_CREATION);
		}
	}
	else if (mNextButton->IsMouseOver())
	{
		if (mState == STATE_AVATAR_LIST)
		{
			if (mSelectedAvatarIndex < GetAvatarCount() - 1)
				mSelectedAvatarIndex++;
			mNextButton->mDisabled = mSelectedAvatarIndex >= GetAvatarCount() - 1;
			mPrevButton->mDisabled = mSelectedAvatarIndex <= 0;
		}
	}
	else if (mPrevButton->IsMouseOver())
	{
		if (mState == STATE_AVATAR_LIST)
		{
			if (mSelectedAvatarIndex > 0)
				mSelectedAvatarIndex--;
			mNextButton->mDisabled = mSelectedAvatarIndex >= GetAvatarCount() - 1;
			mPrevButton->mDisabled = mSelectedAvatarIndex <= 0;
		}
	}
	else if (mNextPageButton->IsMouseOver())
	{
		if (mSubPage < mMaxSubPages)
			mSubPage++;
		mNextPageButton->mDisabled = mState != STATE_AVATAR_CREATION || mSubPage == mMaxSubPages || mCurrentPage != PAGE_FACIAL_HAIR;
		mPrevPageButton->mDisabled = mState != STATE_AVATAR_CREATION || mSubPage == 0 || mCurrentPage != PAGE_FACIAL_HAIR;
	}
	else if (mPrevPageButton->IsMouseOver())
	{
		if (mSubPage > 0)
			mSubPage--;
		mNextPageButton->mDisabled = mState != STATE_AVATAR_CREATION || mSubPage == mMaxSubPages || mCurrentPage != PAGE_FACIAL_HAIR;
		mPrevPageButton->mDisabled = mState != STATE_AVATAR_CREATION || mSubPage == 0 || mCurrentPage != PAGE_FACIAL_HAIR;
	}
	else if (mDeleteZombatarButton->IsMouseOver())
	{
		if (mSelectedAvatarIndex >= 0)
		{
			DeleteAvatar(mSelectedAvatarIndex);
			if (mSelectedAvatarIndex >= GetAvatarCount())
				mSelectedAvatarIndex = GetAvatarCount() - 1;
		}
	}
	else if (mViewButton->IsMouseOver())
	{
		ChangeState(STATE_AVATAR_LIST);
	}
	else if (mAvatarBackButton->IsMouseOver())
	{
		ChangeState(STATE_AVATAR_CREATION);
	}
	else if (mNewZombatarButton->IsMouseOver())
	{
		ResetConfig();
		ChangeState(STATE_AVATAR_CREATION);
	}
	else if (mFinishedButton->IsMouseOver())
	{
		if (mState == STATE_AVATAR_CREATION)
		{
			ChangeState(STATE_CONFIRM);
		}
		else if (mState == STATE_CONFIRM)
		{
			SaveAvatar();
			ChangeState(STATE_AVATAR_LIST);
		}
	}
	else if (mSkinButton->IsMouseOver())
	{
		ChangePage(PAGE_SKIN);
	}
	else if (mHairButton->IsMouseOver())
	{
		ChangePage(PAGE_HAIR);
	}
	else if (mFacialHairButton->IsMouseOver())
	{
		ChangePage(PAGE_FACIAL_HAIR);
	}
	else if (mTidbitsButton->IsMouseOver())
	{
		ChangePage(PAGE_TIDBITS);
	}
	else if (mEyewearButton->IsMouseOver())
	{
		ChangePage(PAGE_EYEWEAR);
	}
	else if (mClothesButton->IsMouseOver())
	{
		ChangePage(PAGE_CLOTHES);
	}
	else if (mAccessoriesButton->IsMouseOver())
	{
		ChangePage(PAGE_ACCESSORIES);
	}
	else if (mHatsButton->IsMouseOver())
	{
		ChangePage(PAGE_HATS);
	}
	else if (mBackdropsButton->IsMouseOver())
	{
		ChangePage(PAGE_BACKDROPS);
	}

	if (mState == STATE_AVATAR_CREATION)
	{
		int aItemCount = (mCurrentPage == PAGE_SKIN) ? NUM_SKIN_COLORS : GetItemCount(mCurrentPage);
		bool isBackdrops = (mCurrentPage == PAGE_BACKDROPS);

		for (int i = 0; i < 6 * 3; i++)
		{
			int aItemIndex = i + mScrollOffset;

			if (mItemRects[i].Contains(x, y))
			{
				if (mCurrentPage == PAGE_SKIN)
				{
					if (aItemIndex < aItemCount)
					{
						mCurrentConfig.mSkinColor = aItemIndex;
						UpdatePreview();
					}
				}
				else
				{
					if (aItemIndex < aItemCount || (!isBackdrops && aItemIndex == aItemCount))
					{
						int* aPtr = GetSelectedItemPtr(mCurrentPage, mCurrentConfig);
						if (aPtr)
						{
							int aConfigIndex = aItemIndex;
							if (mCurrentPage == PAGE_FACIAL_HAIR)
								aConfigIndex += mSubPage * 17;

							if (aItemIndex == aItemCount)
								*aPtr = -1;
							else
								*aPtr = (*aPtr == aConfigIndex) ? -1 : aConfigIndex;
							UpdatePreview();
						}
					}
				}
				break;
			}
		}

		int aCurrentItemIndex = GetPageItemIndex(mCurrentPage);
		if (mCurrentPage == PAGE_SKIN || (aCurrentItemIndex != -1 && gPortraitItems[ItemTypeOffset(mCurrentPage) + aCurrentItemIndex].mAllowColor))
		{
			int aMaxColor = (mCurrentPage == PAGE_SKIN) ? NUM_SKIN_COLORS : NUM_MORE_COLORS;
			for (int i = 0; i < aMaxColor; i++)
			{
				if (mColorRects[i].Contains(x, y))
				{
					if (mCurrentPage == PAGE_SKIN)
					{
						mCurrentConfig.mSkinColor = i;
					}
					else
					{
						int* aColorPtr = GetSelectedColorPtr(mCurrentPage, mCurrentConfig);
						if (aColorPtr)
							*aColorPtr = i;
					}
					UpdatePreview();
					break;
				}
			}
		}
	}

	MarkDirty();
}

void ZombatarWidget::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
}

void ZombatarWidget::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
}

void ZombatarWidget::AcceptTOS()
{
	if (mApp->mPlayerInfo)
	{
		mApp->mPlayerInfo->mZombatarAccepted = true;
		mApp->WriteCurrentUserConfig();
	}

	ChangeState(STATE_AVATAR_LIST);
}

bool ZombatarWidget::SaveAvatar()
{
	if (!mApp->mPlayerInfo)
		return false;

	if (mApp->mPlayerInfo->mZombatarHeadCount >= MAX_NUM_ZOMBATARS)
		return false;

	auto writeInt32 = [](std::vector<unsigned char>& data, int32_t value) {
		data.push_back(static_cast<unsigned char>(value & 0xFF));
		data.push_back(static_cast<unsigned char>((value >> 8) & 0xFF));
		data.push_back(static_cast<unsigned char>((value >> 16) & 0xFF));
		data.push_back(static_cast<unsigned char>((value >> 24) & 0xFF));
	};

	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mSkinColor);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mClothes);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mClothesColor);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mTidbits);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mTidbitsColor);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mAccessories);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mAccessoriesColor);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mFacialHair);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mFacialHairColor);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mHair);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mHairColor);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mEyewear);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mEyewearColor);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mHat);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mHatColor);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mBackdrop);
	writeInt32(mApp->mPlayerInfo->mZombatarData, mCurrentConfig.mBackdropColor);

	mApp->mPlayerInfo->mZombatarHeadCount++;
	mApp->mPlayerInfo->mZombatarCreatedBefore = 1;
	mApp->WriteCurrentUserConfig();

	return true;
}

bool ZombatarWidget::LoadAvatar(int theIndex)
{
	if (!mApp->mPlayerInfo)
		return false;

	if (theIndex < 0 || theIndex >= static_cast<int>(mApp->mPlayerInfo->mZombatarHeadCount))
		return false;

	int aOffset = theIndex * 68;
	if (aOffset + 68 > static_cast<int>(mApp->mPlayerInfo->mZombatarData.size()))
		return false;

	auto readInt32 = [](const unsigned char* data) -> int32_t {
		return static_cast<int32_t>(data[0]) |
			(static_cast<int32_t>(data[1]) << 8) |
			(static_cast<int32_t>(data[2]) << 16) |
			(static_cast<int32_t>(data[3]) << 24);
	};

	const unsigned char* p = mApp->mPlayerInfo->mZombatarData.data() + aOffset;
	mCurrentConfig.mSkinColor = readInt32(p + 0);
	mCurrentConfig.mClothes = readInt32(p + 4);
	mCurrentConfig.mClothesColor = readInt32(p + 8);
	mCurrentConfig.mTidbits = readInt32(p + 12);
	mCurrentConfig.mTidbitsColor = readInt32(p + 16);
	mCurrentConfig.mAccessories = readInt32(p + 20);
	mCurrentConfig.mAccessoriesColor = readInt32(p + 24);
	mCurrentConfig.mFacialHair = readInt32(p + 28);
	mCurrentConfig.mFacialHairColor = readInt32(p + 32);
	mCurrentConfig.mHair = readInt32(p + 36);
	mCurrentConfig.mHairColor = readInt32(p + 40);
	mCurrentConfig.mEyewear = readInt32(p + 44);
	mCurrentConfig.mEyewearColor = readInt32(p + 48);
	mCurrentConfig.mHat = readInt32(p + 52);
	mCurrentConfig.mHatColor = readInt32(p + 56);
	mCurrentConfig.mBackdrop = readInt32(p + 60);
	mCurrentConfig.mBackdropColor = readInt32(p + 64);

	UpdatePreview();
	return true;
}

bool ZombatarWidget::DeleteAvatar(int theIndex)
{
	if (!mApp->mPlayerInfo)
		return false;

	if (theIndex < 0 || theIndex >= static_cast<int>(mApp->mPlayerInfo->mZombatarHeadCount))
		return false;

	int aOffset = theIndex * 68;
	mApp->mPlayerInfo->mZombatarData.erase(
		mApp->mPlayerInfo->mZombatarData.begin() + aOffset,
		mApp->mPlayerInfo->mZombatarData.begin() + aOffset + 68
	);

	mApp->mPlayerInfo->mZombatarHeadCount--;
	if (mApp->mPlayerInfo->mZombatarHeadCount == 0)
	{
		mApp->mPlayerInfo->mZombatarCreatedBefore = 0;
	}

	mApp->WriteCurrentUserConfig();
	return true;
}

int ZombatarWidget::GetAvatarCount()
{
	if (!mApp->mPlayerInfo)
		return 0;

	return static_cast<int>(mApp->mPlayerInfo->mZombatarHeadCount);
}
