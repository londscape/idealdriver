#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdint>
#include <chrono>

namespace config
{
	namespace esp
	{
		extern bool bEnable;
		extern bool bBoxes;
		extern bool bSnaplines;
		extern bool bHealthBar;
		extern bool bHealthInt;
		extern bool bHeadCircles;
		extern float bRounding;
		extern float bLineout;
		extern bool bShowTeam;
		extern bool bSkeleton;
		extern bool bWeapon;
		extern bool bName;
		extern float crossSize;
		extern bool bDemo;
		extern bool bRadar;

		extern float boxColor[];
		extern float SnaplineColor[];
		extern float TextColor[];
		extern float headCirclesColor[];
		extern float skeletonColor[];
		extern float fovCircleColor[];
	}

	namespace aim
	{
		extern bool bAimbot;
		extern bool bFovCircle;
		extern bool bSemirage;
		extern bool bFillc;
		extern bool bTrigger;
		extern bool bAutofire;
		extern float fovSlider;
		extern float fovtrigger;
		extern float triggerdelay;
		extern float smoothness;
		extern float crossSize;
		extern int smoothingSlider;
	}

	namespace misc
	{
		extern bool bCros;
	}
}

namespace globals
{
	extern const int windowWidth;
	extern const int windowHeight;
	extern uintptr_t base_module;
}

namespace gui
{
	extern bool isRunning;
	extern bool isOpen;

	extern bool showVisuals;
	extern bool showMisc;
	extern bool showAim;
	extern bool showConfig;
	extern bool showAbout;

}

namespace vars
{
	extern std::chrono::seconds remainingTime;
}

#endif

#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

namespace config
{
	namespace esp
	{
		inline bool bEnable = true;
		inline bool bBoxes = true;
		inline bool bSnaplines = false;
		inline float bLineout = false;
		inline bool bHealthBar = false;
		inline bool bHealthInt = false;
		inline bool bHeadCircles = false;
		inline float bRounding = false;
		inline bool bShowTeam = false;
		inline bool bSkeleton = false;
		inline bool bWeapon = false;
		inline bool bName = false;
		inline float crossSize = false;
		inline bool bDemo = false;
		inline bool bRadar = false;

		extern float boxColor[4];
		extern float SnaplineColor[4];
		extern float TextColor[4];
		extern float headCirclesColor[4];
		extern float skeletonColor[4];
		extern float fovCircleColor[4];
	}

	namespace aim
	{
		inline bool bAimbot = false;
		inline bool bFovCircle = false;
		inline bool bSemirage = false;
		inline bool bFillc = false;
		inline bool bTrigger = false;
		inline bool bAutofire = false;
		inline float fovSlider = 10.f;
		inline float fovtrigger = 10.f;
		inline float triggerdelay = 10.f;
		inline float smoothness = 10.f;
		inline float crossSize = 1.f;
		inline int smoothingSlider = 1;
	}

	namespace misc
	{
		inline bool bCros = false;
	}
}

namespace globals
{
	const int windowWidth = 1920;
	const int windowHeight = 1080;
	uintptr_t base_module;
}

namespace gui
{
	bool isRunning = true;
	bool isOpen = true;

	bool showVisuals = false;
	bool showMisc = false;
	bool showAim = false;
	bool showConfig = false;
	bool showAbout = false;
}

#endif