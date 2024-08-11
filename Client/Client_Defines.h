#pragma once

namespace Client 
{
	static const unsigned int	g_iWinSizeX = 1280;
	static const unsigned int	g_iWinSizeY = 720;

	static const unsigned int	g_iPlayerNum = 2;
			
	enum LEVEL { LEVEL_STATIC, LEVEL_TOOL = 1, LEVEL_LOGO, LEVEL_STAGE1, LEVEL_STAGE2, LEVEL_STAGE3, LEVEL_BOSS1, LEVEL_BOSS2, LEVEL_VILLAGE1, LEVEL_VILLAGE2, LEVEL_LOADING = 99, LEVEL_END };
	
	enum PLAYER_TYPE {CLOUD, AERITH ,PLAYER_END};

	enum MATERIALTYPE { MATERIATYPE_NONE, MATERIATYPE_DIFFUSE, MATERIATYPE_MASK, MATERIATYPE_NOISE, MATERIATYPE_DISSOLVE, MATERIATYPE_END };


	/// <summary>
	/// 사운드 채널
	/// </summary>
	enum SOUND_CHANNEL_TYPE {
		// 공통 채널
		SOUND_COMMON1, SOUND_COMMON2, SOUIND_COMMON3, SOUND_COMMON4, SOUND_COMMON5, SOUND_COMMON6,
		// 목소리
		SOUND_VOICE1, SOUND_VOICE2, SOUND_VOICE3, SOUND_VOICE4, SOUND_VOICE5, SOUND_VOICE6,
		// 플레이어 사운드 효과
		SOUND_PLAYER1, SOUND_PLAYER2, SOUND_PLAYER3, SOUND_PLAYER4, SOUND_PLAYER5, SOUND_PLAYER6,
		// 적 사운드 효과
		SOUND_ENEMY1, SOUND_ENEMY2, SOUND_ENEMY3, SOUND_ENEMY4, SOUND_ENEMY5, SOUND_ENEMY6,
		// 환경 사운드
		SOUND_ENVIRONMENT1, SOUND_ENVIRONMENT2, SOUND_ENVIRONMENT3, SOUND_ENVIRONMENT4,
		// 시스템 효과음
		SOUND_SYSTEM1, SOUND_SYSTEM2,
		// BGM
		SOUND_BGM1, SOUND_BGM2,
		// 최대 채널 개수
		END_SOUND_CHANNEL
	};

	const char* SOUND_CHANNEL_NAMES[] = {
		// 공통 채널
		"SOUND_COMMON1", "SOUND_COMMON2", "SOUIND_COMMON3", "SOUND_COMMON4", "SOUND_COMMON5", "SOUND_COMMON6",
		// 목소리
		"SOUND_VOICE1", "SOUND_VOICE2", "SOUND_VOICE3", "SOUND_VOICE4", "SOUND_VOICE5", "SOUND_VOICE6",
		// 플레이어 사운드 효과
		"SOUND_PLAYER1", "SOUND_PLAYER2", "SOUND_PLAYER3", "SOUND_PLAYER4", "SOUND_PLAYER5", "SOUND_PLAYER6",
		// 적 사운드 효과
		"SOUND_ENEMY1", "SOUND_ENEMY2", "SOUND_ENEMY3", "SOUND_ENEMY4", "SOUND_ENEMY5", "SOUND_ENEMY6",
		// 환경 사운드
		"SOUND_ENVIRONMENT1", "SOUND_ENVIRONMENT2", "SOUND_ENVIRONMENT3", "SOUND_ENVIRONMENT4",
		// 시스템 효과음
		"SOUND_SYSTEM1", "SOUND_SYSTEM2",
		// BGM
		"SOUND_BGM1", "SOUND_BGM2",
	};




#pragma region 이펙트 타입

	enum EFFECT_TYPE { EFFECT_PARTICLE, EFFECT_RECT, EFFECT_MESH, EFFECT_TRAIL, EFFECT_GROUP, EFFECT_TYPE_END };

#pragma endregion

/* 카메라 Offset Type*/
	enum OFFSET_TYPE {
		TYPE_SHORT, TYPE_LONG, TYPE_ATTACK1, TYPE_ATTACK2, OFFSET_TYPE_END
	};

}

using namespace Client;


extern HWND g_hWnd;
extern HINSTANCE g_hInst;


