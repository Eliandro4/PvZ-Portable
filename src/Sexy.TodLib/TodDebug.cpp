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

#include <time.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdexcept>
#include <fstream>
#include <string>
#include <string_view>

#ifdef __SWITCH__
#include <switch.h>
#endif

#include "TodDebug.h"
#include "TodCommon.h"
#include "misc/Debug.h"
#include "../SexyAppFramework/Common.h"
#include "../SexyAppFramework/SexyAppBase.h"

using namespace Sexy;

static std::string gLogFileName;
static std::string gDebugDataFolder;

void TodErrorMessageBox(std::string_view theMessage, std::string_view theTitle)
{
	std::string aMsg(theMessage);
	std::string aTitle(theTitle);
#ifdef __SWITCH__
	ErrorApplicationConfig c;
	errorApplicationCreate(&c, aTitle.c_str(), aMsg.c_str());
	errorApplicationShow(&c);
#else
	throw std::runtime_error("Error Box\n--" + aTitle + "--\n" + aMsg);
#endif
}

void TodTraceMemory()
{
}

void* TodMalloc(int theSize)
{
	TOD_ASSERT(theSize > 0);
	return malloc(theSize);
}

void TodFree(void* theBlock)
{
	if (theBlock != nullptr)
	{
		free(theBlock);
	}
}

void TodAssertFailed(const char* theCondition, const char* theFile, int theLine, const char* theMsg, ...)
{
	va_list argList;
	va_start(argList, theMsg);
	std::string aFormattedMsg = Sexy::VFormat(theMsg, argList);
	va_end(argList);

	if (!aFormattedMsg.empty() && aFormattedMsg.back() != '\n')
	{
		aFormattedMsg.push_back('\n');
	}

	std::string aBuffer;
	if (*theCondition != '\0')
	{
		aBuffer = Sexy::StrFormat("\n%s(%d)\nassertion failed: '%s'\n%s\n", theFile, theLine, theCondition, aFormattedMsg.c_str());
	}
	else
	{
		aBuffer = Sexy::StrFormat("\n%s(%d)\nassertion failed: %s\n", theFile, theLine, aFormattedMsg.c_str());
	}
	TodTrace("%s", aBuffer.c_str());

	TodErrorMessageBox(aBuffer, "Assertion failed");

	exit(0);
}

void TodLog(const char* theFormat, ...)
{
	va_list argList;
	va_start(argList, theFormat);
	std::string aButter = Sexy::VFormat(theFormat, argList);
	va_end(argList);

	if (aButter.empty() || aButter.back() != '\n')
	{
		aButter.push_back('\n');
	}

	TodLogString(aButter);
}

void TodLogString(std::string_view theMsg)
{
#ifdef PVZ_DEBUG
	std::ofstream f(Sexy::PathFromU8(gLogFileName), std::ios::app | std::ios::binary);
	if (!f)
	{
		fprintf(stderr, "Failed to open log file '%s'\n", gLogFileName.c_str());
		return;
	}

	f.write(theMsg.data(), (std::streamsize)theMsg.size());
	if (!f)
	{
		fprintf(stderr, "Failed to write to log file\n");
	}
#endif
}

void TodTrace(const char* theFormat, ...)
{
	va_list argList;
	va_start(argList, theFormat);
	std::string aButter = Sexy::VFormat(theFormat, argList);
	va_end(argList);

	if (aButter.empty() || aButter.back() != '\n')
	{
		aButter.push_back('\n');
	}

	Sexy::PrintF("%s", aButter.c_str());
}

void TodHesitationTrace(...)
{
}

void TodTraceAndLog(const char* theFormat, ...)
{
	va_list argList;
	va_start(argList, theFormat);
	std::string aButter = Sexy::VFormat(theFormat, argList);
	va_end(argList);

	if (aButter.empty() || aButter.back() != '\n')
	{
		aButter.push_back('\n');
	}

	Sexy::PrintF("%s", aButter.c_str());
	TodLogString(aButter);
}

void TodTraceWithoutSpamming(const char* theFormat, ...)
{
	static uint64_t gLastTraceTime = 0LL;
	uint64_t aTime = time(nullptr);
	if (aTime < gLastTraceTime)
	{
		return;
	}

	gLastTraceTime = aTime;
	va_list argList;
	va_start(argList, theFormat);
	std::string aButter = Sexy::VFormat(theFormat, argList);
	va_end(argList);

	if (aButter.empty() || aButter.back() != '\n')
	{
		aButter.push_back('\n');
	}

	Sexy::PrintF("%s", aButter.c_str());
}

void TodAssertInitForApp()
{
	MkDir(GetAppDataPath("userdata"));
	std::string aRelativeUserPath = GetAppDataPath("userdata/");
	gDebugDataFolder = aRelativeUserPath;
	gLogFileName = gDebugDataFolder + "log.txt";

	TodLog("Started %d\n", static_cast<uint64_t>(time(nullptr)));
}
