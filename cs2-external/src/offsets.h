#pragma once
#include <iostream>

namespace offsets
{
	inline std::ptrdiff_t dwLocalPlayer = 0x17262E8; //public const nint dwLocalPlayerPawn
	inline std::ptrdiff_t dwEntityList = 0x18B0FC8;
	inline std::ptrdiff_t dwViewMatrix = 0x19102B0;

	inline std::ptrdiff_t m_pGameSceneNode = 0x318;
	//inline std::ptrdiff_t m_SanitizedName = 0x738;
	inline std::ptrdiff_t m_hPlayerPawn = 0x7E4;
	inline std::ptrdiff_t m_iPawnHealth = 0x7F0;
	inline std::ptrdiff_t m_vecOrigin = 0x12EC; //public const nint m_vecOriginalSpawnLocation
	inline std::ptrdiff_t iIDEntIndex = 0x15A4;
	inline std::ptrdiff_t m_iTeamNum = 0x3CB;
	//inline std::ptrdiff_t m_bSpotted = 0x8;
	inline std::ptrdiff_t modelState = 0x160;
	inline std::ptrdiff_t boneArray = 0x1E0; //public const nint m_bInCrouch
	inline std::ptrdiff_t m_iHealth = 0x334;
	inline std::ptrdiff_t m_fFlags = 0x63;
}
