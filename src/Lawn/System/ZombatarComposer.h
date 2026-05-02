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

#ifndef __ZOMBATARCOMPOSER_H__
#define __ZOMBATARCOMPOSER_H__

#include "../../ConstEnums.h"
#include <unordered_map>

namespace Sexy
{
	class Graphics;
	class MemoryImage;
	class Image;
	class Color;
}

class LawnApp;
struct ZombatarHead;

class ZombatarComposer
{
public:
	LawnApp*					mApp;
	std::unordered_map<uint64_t, Sexy::MemoryImage*>	mHeadCache;

public:
	void						Initialize();
	void						Dispose();

	Sexy::Image*				GetHeadImage(const ZombatarHead& theHead);
	Sexy::MemoryImage*			ComposeHeadImage(const ZombatarHead& theHead);
};

#endif
