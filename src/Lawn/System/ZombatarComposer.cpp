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

#include "ZombatarComposer.h"
#include "../../LawnApp.h"
#include "../../Resources.h"
#include "../../SexyAppFramework/graphics/Graphics.h"
#include "../../SexyAppFramework/graphics/Image.h"
#include "../../SexyAppFramework/graphics/MemoryImage.h"
#include "../../SexyAppFramework/graphics/Color.h"
#include "../Widget/ZombatarWidget.h"

using namespace Sexy;

// Category resource tables — function-local statics for lazy initialisation
// after resources have been loaded.

static Image** GetHairImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_HAIR_1, IMAGE_ZOMBATAR_HAIR_2, IMAGE_ZOMBATAR_HAIR_3,
		IMAGE_ZOMBATAR_HAIR_4, IMAGE_ZOMBATAR_HAIR_5, IMAGE_ZOMBATAR_HAIR_6,
		IMAGE_ZOMBATAR_HAIR_7, IMAGE_ZOMBATAR_HAIR_8, IMAGE_ZOMBATAR_HAIR_9,
		IMAGE_ZOMBATAR_HAIR_10, IMAGE_ZOMBATAR_HAIR_11, IMAGE_ZOMBATAR_HAIR_12,
		IMAGE_ZOMBATAR_HAIR_13, IMAGE_ZOMBATAR_HAIR_14, IMAGE_ZOMBATAR_HAIR_15,
		IMAGE_ZOMBATAR_HAIR_16
	};
	return sImages;
}

static Image** GetHairMasks()
{
	static Image* sMasks[] = {
		IMAGE_ZOMBATAR_HAIR_1_MASK, IMAGE_ZOMBATAR_HAIR_2_MASK, nullptr,
		nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr,
		nullptr, IMAGE_ZOMBATAR_HAIR_11_MASK, IMAGE_ZOMBATAR_HAIR_12_MASK,
		IMAGE_ZOMBATAR_HAIR_13_MASK, IMAGE_ZOMBATAR_HAIR_14_MASK, IMAGE_ZOMBATAR_HAIR_15_MASK,
		nullptr
	};
	return sMasks;
}

static Image** GetFacialHairImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_FACIALHAIR_1, IMAGE_ZOMBATAR_FACIALHAIR_2, IMAGE_ZOMBATAR_FACIALHAIR_3,
		IMAGE_ZOMBATAR_FACIALHAIR_4, IMAGE_ZOMBATAR_FACIALHAIR_5, IMAGE_ZOMBATAR_FACIALHAIR_6,
		IMAGE_ZOMBATAR_FACIALHAIR_7, IMAGE_ZOMBATAR_FACIALHAIR_8, IMAGE_ZOMBATAR_FACIALHAIR_9,
		IMAGE_ZOMBATAR_FACIALHAIR_10, IMAGE_ZOMBATAR_FACIALHAIR_11, IMAGE_ZOMBATAR_FACIALHAIR_12,
		IMAGE_ZOMBATAR_FACIALHAIR_13, IMAGE_ZOMBATAR_FACIALHAIR_14, IMAGE_ZOMBATAR_FACIALHAIR_15,
		IMAGE_ZOMBATAR_FACIALHAIR_16, IMAGE_ZOMBATAR_FACIALHAIR_17, IMAGE_ZOMBATAR_FACIALHAIR_18,
		IMAGE_ZOMBATAR_FACIALHAIR_19, IMAGE_ZOMBATAR_FACIALHAIR_20, IMAGE_ZOMBATAR_FACIALHAIR_21,
		IMAGE_ZOMBATAR_FACIALHAIR_22, IMAGE_ZOMBATAR_FACIALHAIR_23, IMAGE_ZOMBATAR_FACIALHAIR_24
	};
	return sImages;
}

static Image** GetFacialHairMasks()
{
	static Image* sMasks[] = {
		IMAGE_ZOMBATAR_FACIALHAIR_1_MASK, nullptr, nullptr,
		IMAGE_ZOMBATAR_FACIALHAIR_4_MASK, nullptr, nullptr,
		nullptr, IMAGE_ZOMBATAR_FACIALHAIR_8_MASK, IMAGE_ZOMBATAR_FACIALHAIR_9_MASK,
		IMAGE_ZOMBATAR_FACIALHAIR_10_MASK, IMAGE_ZOMBATAR_FACIALHAIR_11_MASK, IMAGE_ZOMBATAR_FACIALHAIR_12_MASK,
		nullptr, IMAGE_ZOMBATAR_FACIALHAIR_14_MASK, IMAGE_ZOMBATAR_FACIALHAIR_15_MASK,
		IMAGE_ZOMBATAR_FACIALHAIR_16_MASK, nullptr, IMAGE_ZOMBATAR_FACIALHAIR_18_MASK,
		nullptr, nullptr, IMAGE_ZOMBATAR_FACIALHAIR_21_MASK,
		IMAGE_ZOMBATAR_FACIALHAIR_22_MASK, IMAGE_ZOMBATAR_FACIALHAIR_23_MASK, IMAGE_ZOMBATAR_FACIALHAIR_24_MASK
	};
	return sMasks;
}

static Image** GetHatsImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_HATS_1, IMAGE_ZOMBATAR_HATS_2, IMAGE_ZOMBATAR_HATS_3,
		IMAGE_ZOMBATAR_HATS_4, IMAGE_ZOMBATAR_HATS_5, IMAGE_ZOMBATAR_HATS_6,
		IMAGE_ZOMBATAR_HATS_7, IMAGE_ZOMBATAR_HATS_8, IMAGE_ZOMBATAR_HATS_9,
		IMAGE_ZOMBATAR_HATS_10, IMAGE_ZOMBATAR_HATS_11, IMAGE_ZOMBATAR_HATS_12,
		IMAGE_ZOMBATAR_HATS_13, IMAGE_ZOMBATAR_HATS_14
	};
	return sImages;
}

static Image** GetHatsMasks()
{
	static Image* sMasks[] = {
		IMAGE_ZOMBATAR_HATS_1_MASK, nullptr, IMAGE_ZOMBATAR_HATS_3_MASK,
		nullptr, nullptr, IMAGE_ZOMBATAR_HATS_6_MASK,
		IMAGE_ZOMBATAR_HATS_7_MASK, IMAGE_ZOMBATAR_HATS_8_MASK, IMAGE_ZOMBATAR_HATS_9_MASK,
		nullptr, IMAGE_ZOMBATAR_HATS_11_MASK, nullptr,
		nullptr, nullptr
	};
	return sMasks;
}

static Image** GetEyewearImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_EYEWEAR_1, IMAGE_ZOMBATAR_EYEWEAR_2, IMAGE_ZOMBATAR_EYEWEAR_3,
		IMAGE_ZOMBATAR_EYEWEAR_4, IMAGE_ZOMBATAR_EYEWEAR_5, IMAGE_ZOMBATAR_EYEWEAR_6,
		IMAGE_ZOMBATAR_EYEWEAR_7, IMAGE_ZOMBATAR_EYEWEAR_8, IMAGE_ZOMBATAR_EYEWEAR_9,
		IMAGE_ZOMBATAR_EYEWEAR_10, IMAGE_ZOMBATAR_EYEWEAR_11, IMAGE_ZOMBATAR_EYEWEAR_12,
		IMAGE_ZOMBATAR_EYEWEAR_13, IMAGE_ZOMBATAR_EYEWEAR_14, IMAGE_ZOMBATAR_EYEWEAR_15,
		IMAGE_ZOMBATAR_EYEWEAR_16
	};
	return sImages;
}

static Image** GetEyewearMasks()
{
	static Image* sMasks[] = {
		IMAGE_ZOMBATAR_EYEWEAR_1_MASK, IMAGE_ZOMBATAR_EYEWEAR_2_MASK, IMAGE_ZOMBATAR_EYEWEAR_3_MASK,
		IMAGE_ZOMBATAR_EYEWEAR_4_MASK, IMAGE_ZOMBATAR_EYEWEAR_5_MASK, IMAGE_ZOMBATAR_EYEWEAR_6_MASK,
		IMAGE_ZOMBATAR_EYEWEAR_7_MASK, IMAGE_ZOMBATAR_EYEWEAR_8_MASK, IMAGE_ZOMBATAR_EYEWEAR_9_MASK,
		IMAGE_ZOMBATAR_EYEWEAR_10_MASK, IMAGE_ZOMBATAR_EYEWEAR_11_MASK, IMAGE_ZOMBATAR_EYEWEAR_12_MASK,
		nullptr, nullptr, nullptr,
		nullptr
	};
	return sMasks;
}

static Image** GetClothesImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_CLOTHES_1, IMAGE_ZOMBATAR_CLOTHES_2, IMAGE_ZOMBATAR_CLOTHES_3,
		IMAGE_ZOMBATAR_CLOTHES_4, IMAGE_ZOMBATAR_CLOTHES_5, IMAGE_ZOMBATAR_CLOTHES_6,
		IMAGE_ZOMBATAR_CLOTHES_7, IMAGE_ZOMBATAR_CLOTHES_8, IMAGE_ZOMBATAR_CLOTHES_9,
		IMAGE_ZOMBATAR_CLOTHES_10, IMAGE_ZOMBATAR_CLOTHES_11, IMAGE_ZOMBATAR_CLOTHES_12
	};
	return sImages;
}

static Image** GetAccessoryImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_ACCESSORY_1, IMAGE_ZOMBATAR_ACCESSORY_2, IMAGE_ZOMBATAR_ACCESSORY_3,
		IMAGE_ZOMBATAR_ACCESSORY_4, IMAGE_ZOMBATAR_ACCESSORY_5, IMAGE_ZOMBATAR_ACCESSORY_6,
		IMAGE_ZOMBATAR_ACCESSORY_7, IMAGE_ZOMBATAR_ACCESSORY_8, IMAGE_ZOMBATAR_ACCESSORY_9,
		IMAGE_ZOMBATAR_ACCESSORY_10, IMAGE_ZOMBATAR_ACCESSORY_11, IMAGE_ZOMBATAR_ACCESSORY_12,
		IMAGE_ZOMBATAR_ACCESSORY_13, IMAGE_ZOMBATAR_ACCESSORY_14, IMAGE_ZOMBATAR_ACCESSORY_15,
		IMAGE_ZOMBATAR_ACCESSORY_16
	};
	return sImages;
}

static Image** GetTidbitsImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_TIDBITS_1, IMAGE_ZOMBATAR_TIDBITS_2, IMAGE_ZOMBATAR_TIDBITS_3,
		IMAGE_ZOMBATAR_TIDBITS_4, IMAGE_ZOMBATAR_TIDBITS_5, IMAGE_ZOMBATAR_TIDBITS_6,
		IMAGE_ZOMBATAR_TIDBITS_7, IMAGE_ZOMBATAR_TIDBITS_8, IMAGE_ZOMBATAR_TIDBITS_9,
		IMAGE_ZOMBATAR_TIDBITS_10, IMAGE_ZOMBATAR_TIDBITS_11, IMAGE_ZOMBATAR_TIDBITS_12,
		IMAGE_ZOMBATAR_TIDBITS_13, IMAGE_ZOMBATAR_TIDBITS_14
	};
	return sImages;
}

static Image** GetBackdropImages()
{
	static Image* sImages[] = {
		IMAGE_ZOMBATAR_BACKGROUND_MENU, IMAGE_ZOMBATAR_BACKGROUND_ROOF,
		IMAGE_ZOMBATAR_BACKGROUND_CRAZYDAVE, IMAGE_ZOMBATAR_BACKGROUND_MENU_DOS,
		IMAGE_ZOMBATAR_BACKGROUND_BLANK
	};
	return sImages;
}

// Color palettes matching original GOTY Zombatar
static Color gSkinColors[12] = {
	Color(0xF5, 0xD0, 0xA9), Color(0xE8, 0xC0, 0x98), Color(0xD4, 0xA8, 0x80),
	Color(0xC2, 0x90, 0x70), Color(0xB0, 0x80, 0x60), Color(0xA0, 0x70, 0x55),
	Color(0x8E, 0x60, 0x48), Color(0x7A, 0x50, 0x40), Color(0x68, 0x42, 0x35),
	Color(0x50, 0x32, 0x28), Color(0x3A, 0x24, 0x1C), Color(0x28, 0x18, 0x12)
};

static Color gHairColors[30] = {
	Color(0x00, 0x00, 0x00), Color(0x1A, 0x1A, 0x1A), Color(0x33, 0x33, 0x33),
	Color(0x4D, 0x4D, 0x4D), Color(0x66, 0x66, 0x66), Color(0x80, 0x80, 0x80),
	Color(0x99, 0x99, 0x99), Color(0xB3, 0xB3, 0xB3), Color(0xCC, 0xCC, 0xCC),
	Color(0xE6, 0xE6, 0xE6), Color(0xFF, 0xFF, 0xFF), Color(0x33, 0x11, 0x00),
	Color(0x55, 0x22, 0x00), Color(0x77, 0x33, 0x00), Color(0x99, 0x44, 0x00),
	Color(0xBB, 0x55, 0x00), Color(0xDD, 0x66, 0x00), Color(0xFF, 0x77, 0x00),
	Color(0xFF, 0x99, 0x33), Color(0xFF, 0xBB, 0x66), Color(0xFF, 0xDD, 0x99),
	Color(0x88, 0x00, 0x00), Color(0xAA, 0x11, 0x11), Color(0xCC, 0x22, 0x22),
	Color(0xDD, 0x44, 0x44), Color(0xEE, 0x66, 0x66), Color(0x44, 0x00, 0x88),
	Color(0x66, 0x22, 0xAA), Color(0x88, 0x44, 0xCC), Color(0xAA, 0x66, 0xEE)
};

static Color gHatEyewearColors[49] = {
	Color(0xFF, 0x00, 0x00), Color(0xCC, 0x00, 0x00), Color(0x99, 0x00, 0x00),
	Color(0x66, 0x00, 0x00), Color(0xFF, 0x66, 0x66), Color(0xFF, 0x99, 0x99),
	Color(0xFF, 0x80, 0x00), Color(0xCC, 0x66, 0x00), Color(0x99, 0x4D, 0x00),
	Color(0xFF, 0xB3, 0x66), Color(0xFF, 0xCC, 0x99), Color(0xFF, 0xFF, 0x00),
	Color(0xCC, 0xCC, 0x00), Color(0x99, 0x99, 0x00), Color(0x66, 0x66, 0x00),
	Color(0xFF, 0xFF, 0x66), Color(0xFF, 0xFF, 0x99), Color(0x00, 0xFF, 0x00),
	Color(0x00, 0xCC, 0x00), Color(0x00, 0x99, 0x00), Color(0x00, 0x66, 0x00),
	Color(0x66, 0xFF, 0x66), Color(0x99, 0xFF, 0x99), Color(0x00, 0xFF, 0xFF),
	Color(0x00, 0xCC, 0xCC), Color(0x00, 0x99, 0x99), Color(0x00, 0x66, 0x66),
	Color(0x66, 0xFF, 0xFF), Color(0x99, 0xFF, 0xFF), Color(0x00, 0x00, 0xFF),
	Color(0x00, 0x00, 0xCC), Color(0x00, 0x00, 0x99), Color(0x00, 0x00, 0x66),
	Color(0x66, 0x66, 0xFF), Color(0x99, 0x99, 0xFF), Color(0xFF, 0x00, 0xFF),
	Color(0xCC, 0x00, 0xCC), Color(0x99, 0x00, 0x99), Color(0x66, 0x00, 0x66),
	Color(0xFF, 0x66, 0xFF), Color(0xFF, 0x99, 0xFF), Color(0x80, 0x00, 0x80),
	Color(0x40, 0x00, 0x40), Color(0xFF, 0xFF, 0xFF), Color(0xCC, 0xCC, 0xCC),
	Color(0x99, 0x99, 0x99), Color(0x66, 0x66, 0x66), Color(0x33, 0x33, 0x33),
	Color(0x00, 0x00, 0x00)
};

void ZombatarComposer::Initialize()
{
	mApp = (LawnApp*)gSexyAppBase;
}

void ZombatarComposer::Dispose()
{
	for (auto& aPair : mHeadCache)
		delete aPair.second;
	mHeadCache.clear();
}

Image* ZombatarComposer::GetHeadImage(const ZombatarHead& theHead)
{
	uint64_t aHash = 0;
	const uint8_t* aData = reinterpret_cast<const uint8_t*>(&theHead);
	for (size_t i = 0; i < sizeof(ZombatarHead); i++)
		aHash = aHash * 31 + aData[i];

	auto aIt = mHeadCache.find(aHash);
	if (aIt != mHeadCache.end())
		return aIt->second;

	MemoryImage* aImage = ComposeHeadImage(theHead);
	mHeadCache[aHash] = aImage;
	return aImage;
}

MemoryImage* ZombatarComposer::ComposeHeadImage(const ZombatarHead& theHead)
{
	MemoryImage* aCanvas = new MemoryImage();
	aCanvas->Create(IMAGE_ZOMBATAR_ZOMBIE_BLANK->mWidth,
					IMAGE_ZOMBATAR_ZOMBIE_BLANK->mHeight);

	Graphics g(aCanvas);
	g.SetLinearBlend(true);

	// Base head
	g.DrawImage(IMAGE_ZOMBATAR_ZOMBIE_BLANK, 0, 0);

	// Skin color overlay
	if (theHead.mSkinColor < 12)
	{
		g.SetColorizeImages(true);
		g.SetColor(gSkinColors[theHead.mSkinColor]);
		g.DrawImage(IMAGE_ZOMBATAR_ZOMBIE_BLANK_SKIN, 0, 0);
		g.SetColorizeImages(false);
	}

	// Clothes
	if (theHead.mClothesType < 12)
		g.DrawImage(GetClothesImages()[theHead.mClothesType], 0, 0);

	// Facial hair
	if (theHead.mFacialHairType < 24)
	{
		g.DrawImage(GetFacialHairImages()[theHead.mFacialHairType], 0, 0);
		if (theHead.mFacialHairColor < 30)
		{
			g.SetColorizeImages(true);
			g.SetColor(gHairColors[theHead.mFacialHairColor]);
			g.DrawImage(GetFacialHairMasks()[theHead.mFacialHairType], 0, 0);
			g.SetColorizeImages(false);
		}
	}

	// Hair
	if (theHead.mHairType < 16)
	{
		g.DrawImage(GetHairImages()[theHead.mHairType], 0, 0);
		if (theHead.mHairColor < 30)
		{
			g.SetColorizeImages(true);
			g.SetColor(gHairColors[theHead.mHairColor]);
			g.DrawImage(GetHairMasks()[theHead.mHairType], 0, 0);
			g.SetColorizeImages(false);
		}
	}

	// Eyewear
	if (theHead.mEyewearType < 16)
	{
		g.DrawImage(GetEyewearImages()[theHead.mEyewearType], 0, 0);
		if (theHead.mEyewearColor < 49)
		{
			g.SetColorizeImages(true);
			g.SetColor(gHatEyewearColors[theHead.mEyewearColor]);
			g.DrawImage(GetEyewearMasks()[theHead.mEyewearType], 0, 0);
			g.SetColorizeImages(false);
		}
	}

	// Hats
	if (theHead.mHatType < 14)
	{
		g.DrawImage(GetHatsImages()[theHead.mHatType], 0, 0);
		if (theHead.mHatColor < 49)
		{
			g.SetColorizeImages(true);
			g.SetColor(gHatEyewearColors[theHead.mHatColor]);
			g.DrawImage(GetHatsMasks()[theHead.mHatType], 0, 0);
			g.SetColorizeImages(false);
		}
	}

	// Accessories
	if (theHead.mAccessoryType < 16)
		g.DrawImage(GetAccessoryImages()[theHead.mAccessoryType], 0, 0);

	// Tidbits
	if (theHead.mTidbitsType < 14)
		g.DrawImage(GetTidbitsImages()[theHead.mTidbitsType], 0, 0);

	return aCanvas;
}

