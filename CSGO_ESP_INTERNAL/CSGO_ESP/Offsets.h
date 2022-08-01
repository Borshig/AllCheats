#pragma once

/*
#define dwEntityList 0x4D0D094
#define dwViewMatrix 0x4CFEAC4
#define m_ireamNum 0xF4
#define m_iHelath 0x100
#define m_vecOrigin 0x138
*/

namespace offsets
{
	const int timestamp = 1608284679;
	static class netvars
	{
		const int cs_gamerules_data = 0x0;
		const int m_ArmorValue = 0xB378;
		const int m_Collision = 0x320;
		const int m_CollisionGroup = 0x474;
		const int m_Local = 0x2FBC;
		const int m_MoveType = 0x25C;
		const int m_OriginalOwnerXuidHigh = 0x31C4;
		const int m_OriginalOwnerXuidLow = 0x31C0;
		const int m_SurvivalGameRuleDecisionTypes = 0x1328;
		const int m_SurvivalRules = 0xD00;
		const int m_aimPunchAngle = 0x302C;
		const int m_aimPunchAngleVel = 0x3038;
		const int m_angEyeAnglesX = 0xB37C;
		const int m_angEyeAnglesY = 0xB380;
		const int m_bBombPlanted = 0x9A5;
		const int m_bFreezePeriod = 0x20;
		const int m_bGunGameImmunity = 0x3944;
		const int m_bHasDefuser = 0xB388;
		const int m_bHasHelmet = 0xB36C;
		const int m_bInReload = 0x32A5;
		const int m_bIsDefusing = 0x3930;
		const int m_bIsQueuedMatchmaking = 0x74;
		const int m_bIsScoped = 0x3928;
		const int m_bIsValveDS = 0x7C;
		const int m_bSpotted = 0x93D;
		const int m_bSpottedByMask = 0x980;
		const int m_bStartedArming = 0x33F0;
		const int m_bUseCustomAutoExposureMax = 0x9D9;
		const int m_bUseCustomAutoExposureMin = 0x9D8;
		const int m_bUseCustomBloomScale = 0x9DA;
		const int m_clrRender = 0x70;
		const int m_dwBoneMatrix = 0x26A8;
		const int m_fAccuracyPenalty = 0x3330;
		const int m_fFlags = 0x104;
		const int m_flC4Blow = 0x2990;
		const int m_flCustomAutoExposureMax = 0x9E0;
		const int m_flCustomAutoExposureMin = 0x9DC;
		const int m_flCustomBloomScale = 0x9E4;
		const int m_flDefuseCountDown = 0x29AC;
		const int m_flDefuseLength = 0x29A8;
		const int m_flFallbackWear = 0x31D0;
		const int m_flFlashDuration = 0xA420;
		const int m_flFlashMaxAlpha = 0xA41C;
		const int m_flLastBoneSetupTime = 0x2924;
		const int m_flLowerBodyYawTarget = 0x3A90;
		const int m_flNextAttack = 0x2D70;
		const int m_flNextPrimaryAttack = 0x3238;
		const int m_flSimulationTime = 0x268;
		const int m_flTimerLength = 0x2994;
		const int m_hActiveWeapon = 0x2EF8;
		const int m_hMyWeapons = 0x2DF8;
		const int m_hObserverTarget = 0x338C;
		const int m_hOwner = 0x29CC;
		const int m_hOwnerEntity = 0x14C;
		const int m_iAccountID = 0x2FC8;
		const int m_iClip1 = 0x3264;
		const int m_iCompetitiveRanking = 0x1A84;
		const int m_iCompetitiveWins = 0x1B88;
		const int m_iCrosshairId = 0xB3E4;
		const int m_iEntityQuality = 0x2FAC;
		const int m_iFOV = 0x31E4;
		const int m_iFOVStart = 0x31E8;
		const int m_iGlowIndex = 0xA438;
		const int m_iHealth = 0x100;
		const int m_iItemDefinitionIndex = 0x2FAA;
		const int m_iItemIDHigh = 0x2FC0;
		const int m_iMostRecentModelBoneCounter = 0x2690;
		const int m_iObserverMode = 0x3378;
		const int m_iShotsFired = 0xA390;
		const int m_iState = 0x3258;
		const int m_iTeamNum = 0xF4;
		const int m_lifeState = 0x25F;
		const int m_nFallbackPaintKit = 0x31C8;
		const int m_nFallbackSeed = 0x31CC;
		const int m_nFallbackStatTrak = 0x31D4;
		const int m_nForceBone = 0x268C;
		const int m_nTickBase = 0x3430;
		const int m_rgflCoordinateFrame = 0x444;
		const int m_szCustomName = 0x303C;
		const int m_szLastPlaceName = 0x35B4;
		const int m_thirdPersonViewAngles = 0x31D8;
		const int m_vecOrigin = 0x138;
		const int m_vecVelocity = 0x114;
		const int m_vecViewOffset = 0x108;
		const int m_viewPunchAngle = 0x3020;
	};
	static class signatures
	{
		const int anim_overlays = 0x2980;
		const int clientstate_choked_commands = 0x4D30;
		const int clientstate_delta_ticks = 0x174;
		const int clientstate_last_outgoing_command = 0x4D2C;
		const int clientstate_net_channel = 0x9C;
		const int convar_name_hash_table = 0x2F0F8;
		const int dwClientState = 0x58EFE4;
		const int dwClientState_GetLocalPlayer = 0x180;
		const int dwClientState_IsHLTV = 0x4D48;
		const int dwClientState_Map = 0x28C;
		const int dwClientState_MapDirectory = 0x188;
		const int dwClientState_MaxPlayer = 0x388;
		const int dwClientState_PlayerInfo = 0x52C0;
		const int dwClientState_State = 0x108;
		const int dwClientState_ViewAngles = 0x4D90;
		const int dwEntityList = 0x4D9FBD4;
		const int dwForceAttack = 0x31D1134;
		const int dwForceAttack2 = 0x31D1140;
		const int dwForceBackward = 0x31D1188;
		const int dwForceForward = 0x31D1194;
		const int dwForceJump = 0x5249B34;
		const int dwForceLeft = 0x31D1110;
		const int dwForceRight = 0x31D1104;
		const int dwGameDir = 0x62D7F8;
		const int dwGameRulesProxy = 0x52BCE2C;
		const int dwGetAllClasses = 0xDAFD74;
		const int dwGlobalVars = 0x58ECE8;
		const int dwGlowObjectManager = 0x52E81B0;
		const int dwInput = 0x51F11B8;
		const int dwInterfaceLinkList = 0x943C54;
		const int dwLocalPlayer = 0xD882BC;
		const int dwMouseEnable = 0xD8DE60;
		const int dwMouseEnablePtr = 0xD8DE30;
		const int dwPlayerResource = 0x31CF490;
		const int dwRadarBase = 0x51D494C;
		const int dwSensitivity = 0xD8DCFC;
		const int dwSensitivityPtr = 0xD8DCD0;
		const int dwSetClanTag = 0x8A1A0;
		const int dwViewMatrix = 0x4D914D4;
		const int dwWeaponTable = 0x51F1C78;
		const int dwWeaponTableIndex = 0x325C;
		const int dwYawPtr = 0xD8DAC0;
		const int dwZoomSensitivityRatioPtr = 0xD92D60;
		const int dwbSendPackets = 0xD723A;
		const int dwppDirect3DDevice9 = 0xA7050;
		const int find_hud_element = 0x27DEEE80;
		const int force_update_spectator_glow = 0x3AE1F2;
		const int interface_engine_cvar = 0x3E9EC;
		const int is_c4_owner = 0x3BACB0;
		const int m_bDormant = 0xED;
		const int m_flSpawnTime = 0xA370;
		const int m_pStudioHdr = 0x294C;
		const int m_pitchClassPtr = 0x51D4BE8;
		const int m_yawClassPtr = 0xD8DAC0;
		const int model_ambient_min = 0x59205C;
		const int set_abs_angles = 0x1E05B0;
		const int set_abs_origin = 0x1E03F0;
	};
} // namespace hazedumper