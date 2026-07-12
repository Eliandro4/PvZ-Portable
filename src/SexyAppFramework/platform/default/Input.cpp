/*
 * Portions of this file are based on the PopCap Games Framework
 * Copyright (C) 2005-2009 PopCap Games, Inc.
 * 
 * Copyright (C) 2026 Zhou Qiankang <wszqkzqk@qq.com>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later AND LicenseRef-PopCap
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

#include <SDL.h>

#include "SexyAppBase.h"
#include "graphics/GLInterface.h"
#include "graphics/GLImage.h"
#include "widget/WidgetManager.h"
#include "misc/KeyCodes.h"

// Game-specific headers (the desktop port links SexyAppFramework together with the
// Lawn game code in a single binary, so the gamepad backend can drive the Board).
#include "LawnApp.h"
#include "Lawn/Board.h"
#include "Lawn/CursorObject.h"
#include "Lawn/SeedPacket.h"
#include "Lawn/Widget/SeedChooserScreen.h"
#include "Lawn/Widget/GameButton.h"
#include "GameConstants.h"

using namespace Sexy;

namespace
{
	constexpr int GAMEPAD_DEADZONE = 8000;
	constexpr int GAMEPAD_REPEAT_MS = 180;
	constexpr float GAMEPAD_MENU_SPEED = 350.0f; // pixels per second

	static SDL_GameController* gGamepad = nullptr;

	// Virtual mouse cursor (widget coordinates) used in MENU / non-battle modes.
	static float gPadCursorX = 400.0f;
	static float gPadCursorY = 300.0f;

	// Current highlighted grid cell (battle mode).
	static int gPadGridX = 0;
	static int gPadGridY = 0;

	// Currently selected seed-bank packet index (battle mode), or -1 if none.
	static int gPadSelected = -1;

	// Edge-detection state for buttons.
	static bool gPadPrevA = false;
	static bool gPadPrevB = false;
	static bool gPadPrevStart = false;

	// Repeat timers (ms).
	static Uint32 gPadGridCD = 0;
	static Uint32 gPadSelCD = 0;
	static Uint32 gPadLastTick = 0;
	static bool gPadWasBattle = false;

	// Currently highlighted cell in the Seed Chooser grid (column, row).
	static int gPadChooserCol = 0;
	static int gPadChooserRow = 0;
	static bool gPadWasChooser = false;
	static bool gPadChooserDialog = false;

	static inline int PadClamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

	static void GamepadLoadMappings()
	{
		// Load community controller mappings from the SDL_GameControllerDB
		// (https://github.com/mdqinc/SDL_GameControllerDB/blob/master/gamecontrollerdb.txt).
		// Drop a `gamecontrollerdb.txt` next to the executable (or in the working directory)
		// to get correct button/axis mappings for a wider range of gamepads.
#ifdef __ANDROID__
		// On Android the DB is bundled as an APK asset, which SDL's
		// SDL_RWFromFile reads straight out of the .apk.
		SDL_RWops* rw = SDL_RWFromFile("gamecontrollerdb.txt", "rb");
		if (rw)
			SDL_GameControllerAddMappingsFromRW(rw, 1);
#else
		char* basePath = SDL_GetBasePath();
		if (basePath)
		{
			std::string path = std::string(basePath) + "gamecontrollerdb.txt";
			SDL_GameControllerAddMappingsFromFile(path.c_str());
			SDL_free(basePath);
		}
		SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
#endif
	}

	static int ControllerAxisCount(int i)
	{
		SDL_GameController* ctrl = SDL_GameControllerOpen(i);
		if (!ctrl)
			return 0;
		SDL_Joystick* joy = SDL_GameControllerGetJoystick(ctrl);
		int axes = joy ? SDL_JoystickNumAxes(joy) : 0;
		SDL_GameControllerClose(ctrl);
		return axes;
	}

	static void GamepadTryOpen()
	{
		if (gGamepad)
			return;
		int best = -1;
		int bestAxes = -1;
		for (int i = 0; i < SDL_NumJoysticks(); i++)
		{
			if (!SDL_IsGameController(i))
				continue;
			int axes = ControllerAxisCount(i);
			if (axes <= bestAxes)
				continue;
			best = i;
			bestAxes = axes;
		}
		if (best >= 0)
		{
			gGamepad = SDL_GameControllerOpen(best);
			if (gGamepad && !SDL_GameControllerGetAttached(gGamepad))
			{
				SDL_GameControllerClose(gGamepad);
				gGamepad = nullptr;
			}
		}
	}

	static int FirstValidPacket(Board* board)
	{
		SeedBank* sb = board->mSeedBank;
		for (int i = 0; i < sb->mNumPackets; i++)
		{
			if (sb->mSeedPackets[i].mPacketType != SeedType::SEED_NONE)
				return i;
		}
		return -1;
	}

	static int NextValidPacket(Board* board, int start, int dir)
	{
		SeedBank* sb = board->mSeedBank;
		int n = sb->mNumPackets;
		if (n <= 0)
			return -1;
		int i = start;
		for (int step = 0; step < n; step++)
		{
			i = PadClamp(i + dir, 0, n - 1);
			if (sb->mSeedPackets[i].mPacketType != SeedType::SEED_NONE)
				return i;
		}
		return start;
	}

	static void CellCenter(Board* board, int gx, int gy, int& cx, int& cy)
	{
		int cellH = (board->StageHasPool() || board->StageHasRoof()) ? 85 : 100;
		cx = board->GridToPixelX(gx, gy) + 40;
		cy = board->GridToPixelY(gx, gy) + cellH / 2;
	}

	static void AutoCollectSun(Board* board)
	{
		Coin* coin = nullptr;
		while (board->mCoins.IterateNext(coin))
		{
			if (!coin->mDead && !coin->mIsBeingCollected && (coin->IsSun() || coin->IsMoney()))
				coin->MouseDown((int)coin->mPosX, (int)coin->mPosY, 1);
		}
	}

	static int ChooserNumSeeds(LawnApp* app)
	{
		return app->mSeedChooserScreen->Has7Rows() ? 48 : 40;
	}

	static bool ChooserIsSelectable(LawnApp* app, SeedChooserScreen* sc, int col, int row)
	{
		int idx = row * 8 + col;
		if (idx < 0 || idx > 48)
			return false;
		SeedType st = (SeedType)idx;
		if (!app->HasSeedType(st))
			return false;
		if (idx < 48 && idx >= ChooserNumSeeds(app))
			return false;
		ChosenSeed& cs = sc->mChosenSeeds[st];
		if (cs.mSeedState == SEED_PACKET_HIDDEN)
			return false;
		if (sc->SeedNotAllowedToPick(st))
			return false;
		return true;
	}

	static void ChooserFirstSelectable(LawnApp* app, SeedChooserScreen* sc, int& col, int& row)
	{
		for (int r = 0; r < 7; r++)
		{
			for (int c = 0; c < 8; c++)
			{
				if (ChooserIsSelectable(app, sc, c, r))
				{
					col = c;
					row = r;
					return;
				}
			}
		}
		col = 0;
		row = 0;
	}

	static bool ChooserStep(LawnApp* app, SeedChooserScreen* sc, int& col, int& row, int dx, int dy)
	{
		int nc = col, nr = row;
		for (int step = 0; step < 14; step++)
		{
			nc = PadClamp(nc + dx, 0, 7);
			nr = PadClamp(nr + dy, 0, 6);
			if (nc == col && nr == row)
				break;
			if (ChooserIsSelectable(app, sc, nc, nr))
			{
				col = nc;
				row = nr;
				return true;
			}
		}
		return false;
	}

	static void ChooserCellCenter(LawnApp* app, SeedChooserScreen* sc, int col, int row, int& cx, int& cy)
	{
		SeedType st = (SeedType)(row * 8 + col);
		int lx = 0, ly = 0;
		sc->GetSeedPositionInChooser((int)st, lx, ly);
		cx = lx + SEED_PACKET_WIDTH / 2;
		cy = ly + SEED_PACKET_HEIGHT / 2;
	}

	static void GamepadPlant(LawnApp* app, Board* board)
	{
		int sel = gPadSelected;
		if (sel < 0 || sel >= board->mSeedBank->mNumPackets)
			return;
		SeedPacket* pkt = &board->mSeedBank->mSeedPackets[sel];
		if (!pkt->CanPickUp())
			return;

		board->mCursorObject->mType = pkt->mPacketType;
		board->mCursorObject->mImitaterType = pkt->mImitaterType;
		board->mCursorObject->mCursorType = CursorType::CURSOR_TYPE_PLANT_FROM_BANK;
		board->mCursorObject->mSeedBankIndex = sel;

		int cx = 0, cy = 0;
		CellCenter(board, gPadGridX, gPadGridY, cx, cy);
		app->mWidgetManager->MouseMove(cx, cy);
		app->mWidgetManager->MouseDown(cx, cy, 1);
		app->mWidgetManager->MouseUp(cx, cy, 1);
	}

	static void GamepadShovel(LawnApp* app, Board* board)
	{
		if (!board->mShowShovel || !board->CanInteractWithBoardButtons())
			return;
		board->PickUpTool(GameObjectType::OBJECT_TYPE_SHOVEL);
		int cx = 0, cy = 0;
		CellCenter(board, gPadGridX, gPadGridY, cx, cy);
		app->mWidgetManager->MouseMove(cx, cy);
		app->mWidgetManager->MouseDown(cx, cy, 1);
		app->mWidgetManager->MouseUp(cx, cy, 1);
		board->mCursorObject->mCursorType = CursorType::CURSOR_TYPE_NORMAL;
	}

	static bool GamepadTryCollectReward(Board* board)
	{
		int left = board->GridToPixelX(gPadGridX, gPadGridY);
		int top = board->GridToPixelY(gPadGridX, gPadGridY);
		int cellH = (board->StageHasPool() || board->StageHasRoof()) ? 85 : 100;
		int right = left + 80;
		int bottom = top + cellH;

		Coin* coin = nullptr;
		while (board->mCoins.IterateNext(coin))
		{
			if (coin->mDead || coin->mIsBeingCollected || !coin->IsLevelAward())
				continue;
			int cx = (int)coin->mPosX + coin->mWidth / 2;
			int cy = (int)coin->mPosY + coin->mHeight / 2;
			if (cx >= left && cx < right && cy >= top && cy < bottom)
			{
				coin->MouseDown((int)coin->mPosX, (int)coin->mPosY, 1);
				return true;
			}
		}
		return false;
	}

	static void GamepadUpdate()
	{
		LawnApp* app = gLawnApp;
		if (!app || !app->mWidgetManager)
			return;

		if (!gGamepad)
		{
			app->mGamepadPointerActive = false;
			app->mGamepadHideCursor = false;
			return;
		}

		if (!SDL_GameControllerGetAttached(gGamepad))
		{
			SDL_GameControllerClose(gGamepad);
			gGamepad = nullptr;
			app->mGamepadPointerActive = false;
			app->mGamepadHideCursor = false;
			return;
		}

		WidgetManager* wm = app->mWidgetManager;
		app->mGamepadHideCursor = true;
		Uint32 now = SDL_GetTicks();
		Uint32 dt = gPadLastTick ? (now - gPadLastTick) : 16;
		gPadLastTick = now;

		Board* board = app->mBoard;
		bool battle = board != nullptr
			&& app->mGameScene == GameScenes::SCENE_PLAYING
			&& !app->mSeedChooserScreen
			&& app->GetDialogCount() == 0
			&& !board->mPaused
			&& board->mBoardFadeOutCounter < 0;

		bool chooser = app->mSeedChooserScreen != nullptr;

		if (battle && !gPadWasBattle)
		{
			gPadGridX = 0;
			gPadGridY = 0;
			gPadSelected = -1;
			board->mGamepadSelectedPacket = gPadSelected;
		}
		else if (!battle && gPadWasBattle && board)
		{
			board->mGamepadGridActive = false;
			board->mGamepadSelectedPacket = -1;
		}
		gPadWasBattle = battle;

		Sint16 lx = SDL_GameControllerGetAxis(gGamepad, SDL_CONTROLLER_AXIS_LEFTX);
		Sint16 ly = SDL_GameControllerGetAxis(gGamepad, SDL_CONTROLLER_AXIS_LEFTY);
		Sint16 rx = SDL_GameControllerGetAxis(gGamepad, SDL_CONTROLLER_AXIS_RIGHTX);
		Sint16 ry = SDL_GameControllerGetAxis(gGamepad, SDL_CONTROLLER_AXIS_RIGHTY);

		bool a = SDL_GameControllerGetButton(gGamepad, SDL_CONTROLLER_BUTTON_A) != 0;
		bool b = SDL_GameControllerGetButton(gGamepad, SDL_CONTROLLER_BUTTON_B) != 0;
		bool start = SDL_GameControllerGetButton(gGamepad, SDL_CONTROLLER_BUTTON_START) != 0;

		if (battle)
		{
			board->mGamepadGridActive = true;
			board->mGamepadGridX = gPadGridX;
			board->mGamepadGridY = gPadGridY;
			board->mGamepadSelectedPacket = gPadSelected;

			// Left stick: grid navigation (discrete, with auto-repeat).
			if (std::abs(lx) > GAMEPAD_DEADZONE || std::abs(ly) > GAMEPAD_DEADZONE)
			{
				if (gPadGridCD <= 0)
				{
					int dx = 0, dy = 0;
					if (std::abs(lx) > std::abs(ly))
						dx = lx > GAMEPAD_DEADZONE ? 1 : -1;
					else
						dy = ly > GAMEPAD_DEADZONE ? 1 : -1;
					gPadGridX = PadClamp(gPadGridX + dx, 0, MAX_GRID_SIZE_X - 1);
					gPadGridY = PadClamp(gPadGridY + dy, 0, MAX_GRID_SIZE_Y - 1);
					int cx = 0, cy = 0;
					CellCenter(board, gPadGridX, gPadGridY, cx, cy);
					wm->MouseMove(cx, cy);
					gPadGridCD = GAMEPAD_REPEAT_MS;
				}
			}
			else
			{
				gPadGridCD = 0;
			}
			if (gPadGridCD > 0)
				gPadGridCD = (dt >= gPadGridCD) ? 0 : (gPadGridCD - dt);

			// Auto-collect all suns and money on the board.
			AutoCollectSun(board);

			// Right stick: cycle the selected seed-bank packet.
			if (!board->HasConveyorBeltSeedBank()
				&& (std::abs(rx) > GAMEPAD_DEADZONE || std::abs(ry) > GAMEPAD_DEADZONE))
			{
				if (gPadSelCD <= 0)
				{
					int dir = (std::abs(ry) >= std::abs(rx))
						? (ry > GAMEPAD_DEADZONE ? 1 : -1)
						: (rx > GAMEPAD_DEADZONE ? 1 : -1);
					gPadSelected = NextValidPacket(board, gPadSelected, dir);
					board->mGamepadSelectedPacket = gPadSelected;
					gPadSelCD = GAMEPAD_REPEAT_MS;
				}
			}
			else
			{
				gPadSelCD = 0;
			}
			if (gPadSelCD > 0)
				gPadSelCD = (dt >= gPadSelCD) ? 0 : (gPadSelCD - dt);

			// Gamepad pointer follows the current grid cell (battle mode shows
			// no cursor; the grid highlight + plant preview are the indicators).
			int pcx = 0, pcy = 0;
			CellCenter(board, gPadGridX, gPadGridY, pcx, pcy);
			app->mGamepadPointerX = pcx;
			app->mGamepadPointerY = pcy;
			app->mGamepadPointerActive = false;

			// Buttons: A = collect reward / plant, B = shovel, Start = Escape (pause menu).
			if (a && !gPadPrevA)
			{
				if (!GamepadTryCollectReward(board))
					GamepadPlant(app, board);
			}
			if (b && !gPadPrevB)
				GamepadShovel(app, board);
			if (start && !gPadPrevStart)
			{
				// Clear any held plant (grid mode leaves the cursor in
				// PLANT_FROM_BANK) so Escape opens the options menu instead of
				// just cancelling the selection.
				board->mCursorObject->mCursorType = CursorType::CURSOR_TYPE_NORMAL;
				wm->KeyDown(KEYCODE_ESCAPE);
				wm->KeyUp(KEYCODE_ESCAPE);
			}
			gPadWasChooser = false;
		}
		else if (chooser)
		{
			SeedChooserScreen* sc = app->mSeedChooserScreen;

			// While a dialog (e.g. the options menu) is open, fall back to the
			// free virtual cursor so the user can navigate the dialog normally.
			if (app->GetDialogCount() > 0)
			{
				if (!gPadChooserDialog)
				{
					int cx = 0, cy = 0;
					ChooserCellCenter(app, sc, gPadChooserCol, gPadChooserRow, cx, cy);
					gPadCursorX = (float)cx;
					gPadCursorY = (float)cy;
					gPadChooserDialog = true;
				}
				gPadWasChooser = false;
				gPadGridCD = 0;

				if (board)
				{
					board->mGamepadGridActive = false;
					board->mGamepadSelectedPacket = -1;
				}
				sc->mGamepadChooserActive = false;

				// Left stick: free virtual cursor.
				if (std::abs(lx) > GAMEPAD_DEADZONE || std::abs(ly) > GAMEPAD_DEADZONE)
				{
					gPadCursorX += (lx / 32767.0f) * GAMEPAD_MENU_SPEED * (dt / 1000.0f);
					gPadCursorY += (ly / 32767.0f) * GAMEPAD_MENU_SPEED * (dt / 1000.0f);
					gPadCursorX = PadClamp(gPadCursorX, 0.0f, (float)wm->mWidth);
					gPadCursorY = PadClamp(gPadCursorY, 0.0f, (float)wm->mHeight);
					wm->MouseMove((int)gPadCursorX, (int)gPadCursorY);
				}

				app->mGamepadPointerX = (int)gPadCursorX;
				app->mGamepadPointerY = (int)gPadCursorY;
				app->mGamepadPointerActive = true;

				// A = click, B = right click, Start = Escape (close dialog).
				int px = (int)gPadCursorX, py = (int)gPadCursorY;
				if (a && !gPadPrevA)
				{
					wm->MouseMove(px, py);
					wm->MouseDown(px, py, 1);
					wm->MouseUp(px, py, 1);
				}
				if (b && !gPadPrevB)
				{
					wm->MouseMove(px, py);
					wm->MouseDown(px, py, -1);
					wm->MouseUp(px, py, -1);
				}
				if (start && !gPadPrevStart)
				{
					wm->KeyDown(KEYCODE_ESCAPE);
					wm->KeyUp(KEYCODE_ESCAPE);
				}
			}
			else
			{
				gPadChooserDialog = false;

				if (!gPadWasChooser)
					ChooserFirstSelectable(app, sc, gPadChooserCol, gPadChooserRow);
				gPadWasChooser = true;

				// Left stick: grid navigation (discrete, with auto-repeat).
				if (std::abs(lx) > GAMEPAD_DEADZONE || std::abs(ly) > GAMEPAD_DEADZONE)
				{
					if (gPadGridCD <= 0)
					{
						int dx = 0, dy = 0;
						if (std::abs(lx) > std::abs(ly))
							dx = lx > GAMEPAD_DEADZONE ? 1 : -1;
						else
							dy = ly > GAMEPAD_DEADZONE ? 1 : -1;
						ChooserStep(app, sc, gPadChooserCol, gPadChooserRow, dx, dy);
						gPadGridCD = GAMEPAD_REPEAT_MS;
					}
				}
				else
				{
					gPadGridCD = 0;
				}
				if (gPadGridCD > 0)
					gPadGridCD = (dt >= gPadGridCD) ? 0 : (gPadGridCD - dt);

			// Move the virtual cursor onto the highlighted cell so the hover
			// highlight, tooltip and the gamepad border all track it.
			int hcx = 0, hcy = 0;
			ChooserCellCenter(app, sc, gPadChooserCol, gPadChooserRow, hcx, hcy);
			wm->MouseMove(hcx, hcy);
			app->mGamepadPointerX = hcx;
			app->mGamepadPointerY = hcy;
			app->mGamepadPointerActive = false;

				// Expose the highlighted cell for rendering a selection border.
				sc->mGamepadChooserCol = gPadChooserCol;
				sc->mGamepadChooserRow = gPadChooserRow;
				sc->mGamepadChooserActive = true;

				// A = select (if in chooser) or deselect (if already in bank).
				if (a && !gPadPrevA)
				{
					if (sc->mSeedsInFlight == 0 && sc->mChooseState == CHOOSE_NORMAL)
					{
						SeedType st = (SeedType)(gPadChooserRow * 8 + gPadChooserCol);
						if (st != SEED_NONE && app->HasSeedType(st))
						{
							ChosenSeed& cs = sc->mChosenSeeds[st];
							if (cs.mSeedState == SEED_IN_BANK && !cs.mCrazyDavePicked)
								sc->ClickedSeedInBank(cs);
							else if (cs.mSeedState == SEED_IN_CHOOSER)
								sc->ClickedSeedInChooser(cs);
						}
					}
				}

				// B = "Let's Rock" button.
				if (b && !gPadPrevB)
				{
					int bx = sc->mStartButton->mX + sc->mStartButton->mWidth / 2;
					int by = sc->mStartButton->mY + sc->mStartButton->mHeight / 2;
					wm->MouseMove(bx, by);
					wm->MouseDown(bx, by, 1);
					wm->MouseUp(bx, by, 1);
				}

				// Start = open the menu (options).
				if (start && !gPadPrevStart)
				{
					sc->ButtonDepress(104); // SeedChooserScreen::SeedChooserScreen_Menu
				}
			}
		}
		else
		{
			gPadWasChooser = false;
			if (board)
			{
				board->mGamepadGridActive = false;
				board->mGamepadSelectedPacket = -1;
			}

			// Left stick: free virtual cursor.
			if (std::abs(lx) > GAMEPAD_DEADZONE || std::abs(ly) > GAMEPAD_DEADZONE)
			{
				gPadCursorX += (lx / 32767.0f) * GAMEPAD_MENU_SPEED * (dt / 1000.0f);
				gPadCursorY += (ly / 32767.0f) * GAMEPAD_MENU_SPEED * (dt / 1000.0f);
				gPadCursorX = PadClamp(gPadCursorX, 0.0f, (float)wm->mWidth);
				gPadCursorY = PadClamp(gPadCursorY, 0.0f, (float)wm->mHeight);
				wm->MouseMove((int)gPadCursorX, (int)gPadCursorY);
			}

			// Gamepad pointer follows the virtual cursor.
			app->mGamepadPointerX = (int)gPadCursorX;
			app->mGamepadPointerY = (int)gPadCursorY;
			app->mGamepadPointerActive = true;

			// A = left click, B = right click, Start = Escape (pause menu).
			int px = (int)gPadCursorX, py = (int)gPadCursorY;
			if (a && !gPadPrevA)
			{
				wm->MouseMove(px, py);
				wm->MouseDown(px, py, 1);
				wm->MouseUp(px, py, 1);
			}
			if (b && !gPadPrevB)
			{
				wm->MouseMove(px, py);
				wm->MouseDown(px, py, -1);
				wm->MouseUp(px, py, -1);
			}
			if (start && !gPadPrevStart)
			{
				wm->KeyDown(KEYCODE_ESCAPE);
				wm->KeyUp(KEYCODE_ESCAPE);
			}
		}

		gPadPrevA = a;
		gPadPrevB = b;
		gPadPrevStart = start;
	}
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EM_JS(void, WasmStartSoftKeyboard, (), {
	var input = document.getElementById('pvz-soft-keyboard');
	if (!input) return;

	if (!Module.wasmSoftKeyboardState) {
		var state = {
			active: false,
			pendingChars: [],
			pendingKeys: [],
			lastValue: ""
		};

		function syncInputValue() {
			if (!state.active) return;

			var nextValue = input.value || "";
			var prefixLen = 0;
			while (prefixLen < state.lastValue.length && prefixLen < nextValue.length
				&& state.lastValue.charCodeAt(prefixLen) === nextValue.charCodeAt(prefixLen)) {
				prefixLen++;
			}

			for (var i = state.lastValue.length; i > prefixLen; --i) {
				state.pendingKeys.push(8);
			}

			for (var j = prefixLen; j < nextValue.length; ++j) {
				var charCode = nextValue.charCodeAt(j);
				if (charCode > 0 && charCode <= 0x7f) {
					state.pendingChars.push(charCode);
				}
			}

			state.lastValue = nextValue;
		}

		input.addEventListener('input', syncInputValue);
		input.addEventListener('keydown', function(event) {
			if (!state.active) return;

			switch (event.key) {
				case 'Enter':
					state.pendingKeys.push(13);
					event.preventDefault();
					break;
				case 'Escape':
					state.pendingKeys.push(27);
					event.preventDefault();
					break;
				case 'Tab':
					state.pendingKeys.push(9);
					event.preventDefault();
					break;
				case 'Delete':
					state.pendingKeys.push(46);
					event.preventDefault();
					break;
				case 'ArrowLeft':
					state.pendingKeys.push(37);
					event.preventDefault();
					break;
				case 'ArrowRight':
					state.pendingKeys.push(39);
					event.preventDefault();
					break;
				case 'Home':
					state.pendingKeys.push(36);
					event.preventDefault();
					break;
				case 'End':
					state.pendingKeys.push(35);
					event.preventDefault();
					break;
			}
		});

		Module.wasmSoftKeyboardState = state;
	}

	var state = Module.wasmSoftKeyboardState;
	state.active = true;
	state.pendingChars.length = 0;
	state.pendingKeys.length = 0;
	state.lastValue = "";
	input.value = "";
	if (typeof input.focus === 'function') {
		input.focus();
	}
	if (typeof input.setSelectionRange === 'function') {
		input.setSelectionRange(0, 0);
	}
});

EM_JS(void, WasmStopSoftKeyboard, (), {
	var input = document.getElementById('pvz-soft-keyboard');
	var state = Module.wasmSoftKeyboardState;
	if (state) {
		state.active = false;
		state.pendingChars.length = 0;
		state.pendingKeys.length = 0;
		state.lastValue = "";
	}
	if (input) {
		input.value = "";
		if (typeof input.blur === 'function') {
			input.blur();
		}
	}
	if (Module.canvas && typeof Module.canvas.focus === 'function') {
		Module.canvas.focus();
	}
});

EM_JS(int, WasmPopSoftKeyboardChar, (), {
	var state = Module.wasmSoftKeyboardState;
	if (!state || state.pendingChars.length === 0) return 0;
	return state.pendingChars.shift();
});

EM_JS(int, WasmPopSoftKeyboardKey, (), {
	var state = Module.wasmSoftKeyboardState;
	if (!state || state.pendingKeys.length === 0) return 0;
	return state.pendingKeys.shift();
});

EM_JS(int, WasmHasSoftKeyboardEvents, (), {
	var state = Module.wasmSoftKeyboardState;
	if (!state) return 0;
	return (state.pendingChars.length + state.pendingKeys.length) > 0 ? 1 : 0;
});
#endif

using namespace Sexy;

// Map SDL_Keycode to internal KeyCode (Windows VK-compatible).
static KeyCode SDLKeyToKeyCode(SDL_Keycode theSDLKey)
{
	if (theSDLKey >= SDLK_a && theSDLKey <= SDLK_z)
		return static_cast<KeyCode>(theSDLKey - SDLK_a + 'A');

	if (theSDLKey >= SDLK_0 && theSDLKey <= SDLK_9)
		return static_cast<KeyCode>(theSDLKey);

	switch (theSDLKey)
	{
		case SDLK_BACKSPACE:    return KEYCODE_BACK;
		case SDLK_TAB:          return KEYCODE_TAB;
		case SDLK_CLEAR:        return KEYCODE_CLEAR;
		case SDLK_RETURN:       return KEYCODE_RETURN;
		case SDLK_AC_BACK:
		case SDLK_ESCAPE:       return KEYCODE_ESCAPE;
		case SDLK_SPACE:        return KEYCODE_SPACE;
		case SDLK_DELETE:       return KEYCODE_DELETE;

		case SDLK_LEFT:         return KEYCODE_LEFT;
		case SDLK_UP:           return KEYCODE_UP;
		case SDLK_RIGHT:        return KEYCODE_RIGHT;
		case SDLK_DOWN:         return KEYCODE_DOWN;

		case SDLK_INSERT:       return KEYCODE_INSERT;
		case SDLK_HOME:         return KEYCODE_HOME;
		case SDLK_END:          return KEYCODE_END;
		case SDLK_PAGEUP:       return KEYCODE_PRIOR;
		case SDLK_PAGEDOWN:     return KEYCODE_NEXT;

		case SDLK_LSHIFT:
		case SDLK_RSHIFT:       return KEYCODE_SHIFT;
		case SDLK_LCTRL:
		case SDLK_RCTRL:        return KEYCODE_CONTROL;
		case SDLK_LALT:
		case SDLK_RALT:         return KEYCODE_MENU;
		case SDLK_PAUSE:        return KEYCODE_PAUSE;
		case SDLK_CAPSLOCK:     return KEYCODE_CAPITAL;
		case SDLK_NUMLOCKCLEAR: return KEYCODE_NUMLOCK;
		case SDLK_SCROLLLOCK:   return KEYCODE_SCROLL;

		case SDLK_KP_0:         return KEYCODE_NUMPAD0;
		case SDLK_KP_1:         return KEYCODE_NUMPAD1;
		case SDLK_KP_2:         return KEYCODE_NUMPAD2;
		case SDLK_KP_3:         return KEYCODE_NUMPAD3;
		case SDLK_KP_4:         return KEYCODE_NUMPAD4;
		case SDLK_KP_5:         return KEYCODE_NUMPAD5;
		case SDLK_KP_6:         return KEYCODE_NUMPAD6;
		case SDLK_KP_7:         return KEYCODE_NUMPAD7;
		case SDLK_KP_8:         return KEYCODE_NUMPAD8;
		case SDLK_KP_9:         return KEYCODE_NUMPAD9;
		case SDLK_KP_MULTIPLY:  return KEYCODE_MULTIPLY;
		case SDLK_KP_PLUS:      return KEYCODE_ADD;
		case SDLK_KP_MINUS:     return KEYCODE_SUBTRACT;
		case SDLK_KP_PERIOD:    return KEYCODE_DECIMAL;
		case SDLK_KP_DIVIDE:    return KEYCODE_DIVIDE;
		case SDLK_KP_ENTER:     return KEYCODE_RETURN;

		case SDLK_F1:           return KEYCODE_F1;
		case SDLK_F2:           return KEYCODE_F2;
		case SDLK_F3:           return KEYCODE_F3;
		case SDLK_F4:           return KEYCODE_F4;
		case SDLK_F5:           return KEYCODE_F5;
		case SDLK_F6:           return KEYCODE_F6;
		case SDLK_F7:           return KEYCODE_F7;
		case SDLK_F8:           return KEYCODE_F8;
		case SDLK_F9:           return KEYCODE_F9;
		case SDLK_F10:          return KEYCODE_F10;
		case SDLK_F11:          return KEYCODE_F11;
		case SDLK_F12:          return KEYCODE_F12;

		default:                return KEYCODE_UNKNOWN;
	}
}

// Synthesize a minimal ASCII char stream from keydown so legacy KeyChar hotkeys still work.
static bool SDLSynthesizeAsciiCharFromKeyDown(const SDL_KeyboardEvent& theEvent, char& theChar)
{
	theChar = 0;

	if (SDL_IsTextInputActive())
		return false;

	SDL_Keycode aSym = theEvent.keysym.sym;
	SDL_Keymod aMods = static_cast<SDL_Keymod>(theEvent.keysym.mod);
	const bool aHasCtrl = (aMods & KMOD_CTRL) != 0;
	const bool aHasAlt = (aMods & KMOD_ALT) != 0;
	const bool aHasGui = (aMods & KMOD_GUI) != 0;
	const bool aHasShift = (aMods & KMOD_SHIFT) != 0;

	if (aHasAlt || aHasGui)
		return false;

	if (aSym >= SDLK_a && aSym <= SDLK_z)
	{
		theChar = aHasCtrl
			? static_cast<char>(aSym - SDLK_a + 1)
			: static_cast<char>(aHasShift ? aSym - SDLK_a + 'A' : aSym);
		return true;
	}

	if (aHasCtrl)
		return false;

	switch (aSym)
	{
		case SDLK_KP_1: theChar = '1'; return true;
		case SDLK_KP_2: theChar = '2'; return true;
		case SDLK_KP_3: theChar = '3'; return true;
		case SDLK_KP_4: theChar = '4'; return true;
		case SDLK_KP_5: theChar = '5'; return true;
		case SDLK_KP_6: theChar = '6'; return true;
		case SDLK_KP_7: theChar = '7'; return true;
		case SDLK_KP_8: theChar = '8'; return true;
		case SDLK_KP_9: theChar = '9'; return true;
		case SDLK_KP_0: theChar = '0'; return true;
		case SDLK_KP_PLUS: theChar = '+'; return true;
		case SDLK_KP_MINUS: theChar = '-'; return true;
		case SDLK_KP_MULTIPLY: theChar = '*'; return true;
		case SDLK_KP_DIVIDE: theChar = '/'; return true;
		case SDLK_KP_PERIOD: theChar = '.'; return true;
		case SDLK_KP_EQUALS: theChar = '='; return true;
		case SDLK_1: theChar = aHasShift ? '!' : '1'; return true;
		case SDLK_2: theChar = aHasShift ? '@' : '2'; return true;
		case SDLK_3: theChar = aHasShift ? '#' : '3'; return true;
		case SDLK_4: theChar = aHasShift ? '$' : '4'; return true;
		case SDLK_5: theChar = aHasShift ? '%' : '5'; return true;
		case SDLK_6: theChar = aHasShift ? '^' : '6'; return true;
		case SDLK_7: theChar = aHasShift ? '&' : '7'; return true;
		case SDLK_8: theChar = aHasShift ? '*' : '8'; return true;
		case SDLK_9: theChar = aHasShift ? '(' : '9'; return true;
		case SDLK_0: theChar = aHasShift ? ')' : '0'; return true;
		case SDLK_MINUS: theChar = aHasShift ? '_' : '-'; return true;
		case SDLK_EQUALS: theChar = aHasShift ? '+' : '='; return true;
		case SDLK_LEFTBRACKET: theChar = aHasShift ? '{' : '['; return true;
		case SDLK_RIGHTBRACKET: theChar = aHasShift ? '}' : ']'; return true;
		case SDLK_BACKSLASH: theChar = aHasShift ? '|' : '\\'; return true;
		case SDLK_SEMICOLON: theChar = aHasShift ? ':' : ';'; return true;
		case SDLK_QUOTE: theChar = aHasShift ? '"' : '\''; return true;
		case SDLK_COMMA: theChar = aHasShift ? '<' : ','; return true;
		case SDLK_PERIOD: theChar = aHasShift ? '>' : '.'; return true;
		case SDLK_SLASH: theChar = aHasShift ? '?' : '/'; return true;
		case SDLK_BACKQUOTE: theChar = aHasShift ? '~' : '`'; return true;
		case SDLK_SPACE: theChar = ' '; return true;
		default: return false;
	}
}

void SexyAppBase::InitInput()
{
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
	GamepadLoadMappings();
	GamepadTryOpen();
}

bool SexyAppBase::StartTextInput(std::string& theInput)
{
	(void)theInput;
	SDL_StartTextInput();

#ifdef __EMSCRIPTEN__
	WasmStartSoftKeyboard();
#endif

	return false;
}

void SexyAppBase::StopTextInput()
{
	SDL_StopTextInput();

#ifdef __EMSCRIPTEN__
	WasmStopSoftKeyboard();
#endif
}

bool SexyAppBase::ProcessDeferredMessages(bool singleMessage)
{
#ifdef __EMSCRIPTEN__
	int aPendingKey = WasmPopSoftKeyboardKey();
	if (aPendingKey != 0)
	{
		mLastUserInputTick = mLastTimerTime;
		mWidgetManager->KeyDown(static_cast<KeyCode>(aPendingKey));
		return WasmHasSoftKeyboardEvents() || SDL_HasEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
	}

	int aPendingChar = WasmPopSoftKeyboardChar();
	if (aPendingChar != 0)
	{
		mLastUserInputTick = mLastTimerTime;
		mWidgetManager->KeyChar(static_cast<char>(aPendingChar));
		return WasmHasSoftKeyboardEvents() || SDL_HasEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
	}
#endif

	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				CloseRequestAsync();
				break;

		case SDL_CONTROLLERDEVICEADDED:
			if (!gGamepad)
			{
				gGamepad = SDL_GameControllerOpen(event.cdevice.which);
			}
			else if (!SDL_GameControllerGetAttached(gGamepad))
			{
				SDL_GameControllerClose(gGamepad);
				gGamepad = SDL_GameControllerOpen(event.cdevice.which);
			}
			else
			{
				int curAxes = SDL_JoystickNumAxes(SDL_GameControllerGetJoystick(gGamepad));
				int newAxes = ControllerAxisCount(event.cdevice.which);
				if (newAxes > curAxes)
				{
					SDL_GameControllerClose(gGamepad);
					gGamepad = SDL_GameControllerOpen(event.cdevice.which);
				}
			}
			break;

			case SDL_CONTROLLERDEVICEREMOVED:
				if (gGamepad && SDL_GameControllerGetAttached(gGamepad)
					&& SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gGamepad)) == event.cdevice.which)
				{
					SDL_GameControllerClose(gGamepad);
					gGamepad = nullptr;
				}
				break;

			case SDL_APP_WILLENTERBACKGROUND:
				mMinimized = true;
				RehupFocus();
				break;

			case SDL_APP_DIDENTERFOREGROUND:
				mMinimized = false;
				RehupFocus();
				mWidgetManager->MarkAllDirty();
				break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
						CloseRequestAsync();
						break;

					case SDL_WINDOWEVENT_RESIZED:
						mGLInterface->UpdateViewport();
						mWidgetManager->Resize(mScreenBounds, mGLInterface->mPresentationRect);
						mWidgetManager->MarkAllDirty();
						break;

					case SDL_WINDOWEVENT_MINIMIZED:
						mMinimized = true;
						RehupFocus();
						break;

					case SDL_WINDOWEVENT_RESTORED:
						mMinimized = false;
						RehupFocus();
						mWidgetManager->MarkAllDirty();
						break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
						mActive = event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED;
						RehupFocus();
						break;
				}
				break;

			case SDL_MOUSEWHEEL:
			{
				mLastUserInputTick = mLastTimerTime;
				mWidgetManager->MouseWheel(event.wheel.y);
				break;
			}

			case SDL_MOUSEMOTION:
			{
				if (!mMouseIn)
					mMouseIn = true;

				int x = event.motion.x;
				int y = event.motion.y;
				mWidgetManager->RemapMouse(x, y);

				mLastUserInputTick = mLastTimerTime;
				
				mWidgetManager->MouseMove(x, y);
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				if (!mMouseIn)
					mMouseIn = true;

				int x = event.button.x;
				int y = event.button.y;
				mWidgetManager->RemapMouse(x, y);

				mLastUserInputTick = mLastTimerTime;
				
				mWidgetManager->MouseMove(x, y);
				int btn =
					(event.button.button == SDL_BUTTON_LEFT) ? 1 :
					(event.button.button == SDL_BUTTON_RIGHT) ? -1 :
					3;
				if (event.button.clicks == 2)
					btn = (event.button.button == SDL_BUTTON_LEFT) ? 2 : -2;

				mWidgetManager->MouseDown(x, y, btn);
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				if (!mMouseIn)
					mMouseIn = true;

				int x = event.button.x;
				int y = event.button.y;
				mWidgetManager->RemapMouse(x, y);

				mLastUserInputTick = mLastTimerTime;
				
				mWidgetManager->MouseMove(x, y);
				int btn =
					(event.button.button == SDL_BUTTON_LEFT) ? 1 :
					(event.button.button == SDL_BUTTON_RIGHT) ? -1 :
					3;

				mWidgetManager->MouseUp(x, y, btn);
				break;
			}

			case SDL_KEYDOWN:
			{
				mLastUserInputTick = mLastTimerTime;

				if (mAllowAltEnter &&
					event.key.repeat == 0 &&
					(event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) &&
					(event.key.keysym.mod & KMOD_ALT))
				{
					SwitchScreenMode(!mIsWindowed);
					break;
				}

				mWidgetManager->KeyDown(SDLKeyToKeyCode(event.key.keysym.sym));

				char aSynthesizedChar = 0;
				if (SDLSynthesizeAsciiCharFromKeyDown(event.key, aSynthesizedChar))
					mWidgetManager->KeyChar(aSynthesizedChar);

				break;
			}

			case SDL_KEYUP:
				mLastUserInputTick = mLastTimerTime;
				mWidgetManager->KeyUp(SDLKeyToKeyCode(event.key.keysym.sym));
				break;

			case SDL_TEXTINPUT:
				mLastUserInputTick = mLastTimerTime;
				mWidgetManager->KeyChar((char)event.text.text[0]);
				break;
		}
	}

	GamepadUpdate();

	return SDL_HasEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}
