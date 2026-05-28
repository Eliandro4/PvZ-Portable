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

#ifndef __ZOMBATTARTOS_H__
#define __ZOMBATTARTOS_H__

#include "LawnDialog.h"
#include "GameButton.h"
#include "widget/SliderListener.h"
#include "widget/CheckboxListener.h"

class LawnApp;

namespace Sexy
{
	class Slider;
	class Checkbox;
}

class ZombatarTOS : public LawnDialog, public Sexy::SliderListener, public Sexy::CheckboxListener
{
protected:
	enum
	{
		ZombatarTOS_Slider = 600,
		ZombatarTOS_Accept = 601,
		ZombatarTOS_Back = 602,
		ZombatarTOS_TOSCheckbox = 603,
	};

public:
	Sexy::Slider* mTOSSlider;
	NewLawnButton* mBackButton;
	NewLawnButton* mAcceptButton;
	Sexy::Checkbox* mTOSCheckbox;

public:
	ZombatarTOS(LawnApp* theApp);
	~ZombatarTOS();

	virtual void Draw(Graphics* g);
	virtual void Update();
	virtual void AddedToManager(Sexy::WidgetManager* theWidgetManager);
	virtual void RemovedFromManager(Sexy::WidgetManager* theWidgetManager);
	virtual void Resize(int theX, int theY, int theWidth, int theHeight);
	virtual void ButtonPress(int theId);
	virtual void ButtonDepress(int theId);
	virtual void CheckboxChecked(int theId, bool checked);
	virtual void SliderVal(int theId, double theVal);
};

#endif
