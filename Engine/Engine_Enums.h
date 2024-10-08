#pragma once

enum DIK_EX { DIK_MD_NONE, DIK_MD_LCTRL, DIK_MD_LSHIFT, DIK_MD_LALT, DIKK_MD_LCTRL_SHIFT, DIK_MD_LCTRL_ALT, DIKK_MD_LSHIFT_ALT, DIK_MD_LCTRL_SHIFT_ALT };

enum MODELTYPE { NONANIM, ANIM, MODELTYPE_END };

enum LAYERTYPE { L_CAMERA, L_OBJECT,L_ANIMOBJECT, L_INSTANCE ,L_PLAYER,L_MONSTER,L_NPC,L_MOB, L_EFFECT, L_ENVIRONMENT, L_UI,L_TRIGGER,L_COLLIDER,L_TOOLMAP ,LAYERTYPE_END };

enum COLLIDER_LAYER 
{
	CL_PLAYER_CONTROLLER, CL_MONSTER_CONTROLLER,CL_NPC_CONTROLLER,CL_MOB_CONTROLLER, CL_PLAYER_BODY, CL_MONSTER_BODY,CL_MOB_BODY,CL_NPC_BODY, CL_PLAYER_ATTACK, CL_MONSTER_ATTACK, CL_PLAYER_DOTATTACK, CL_MONSTER_DOTATTACK, CL_MAP_STATIC, CL_MAP_DYNAMIC, CL_MAP_DYNAMIC_SPLIT, CL_WALL,CL_LASER, CL_TRIGGER,CL_CAMERA, COLLIDER_LAYER_END
};

enum OBJTYPE
{
	PLAYER,
	ENEMY,
	ATTACK,
	EFFECT,
	LIGHT,
	TRIGGER,
	MAP,
	OBJECT,
	SKYBOX,
	UI,
	CAMERA,
	OBJTYPE_END
};

enum PART_OBJTYPE { PART_WEAPON,PART_EFFECT,PART_FOOT,PART_OBJTYPE_END };


#pragma region 32비트형 플래그 enum 
enum EBIT_FLAG_32
{
	EBIT_FLAG32_0 = 1U << 0,
	EBIT_FLAG32_1 = 1U << 1,
	EBIT_FLAG32_2 = 1U << 2,
	EBIT_FLAG32_3 = 1U << 3,
	EBIT_FLAG32_4 = 1U << 4,
	EBIT_FLAG32_5 = 1U << 5,
	EBIT_FLAG32_6 = 1U << 6,
	EBIT_FLAG32_7 = 1U << 7,
	EBIT_FLAG32_8 = 1U << 8,
	EBIT_FLAG32_9 = 1U << 9,
	EBIT_FLAG32_10 = 1U << 10,
	EBIT_FLAG32_11 = 1U << 11,
	EBIT_FLAG32_12 = 1U << 12,
	EBIT_FLAG32_13 = 1U << 13,
	EBIT_FLAG32_14 = 1U << 14,
	EBIT_FLAG32_15 = 1U << 15,
	EBIT_FLAG32_16 = 1U << 16,
	EBIT_FLAG32_17 = 1U << 17,
	EBIT_FLAG32_18 = 1U << 18,
	EBIT_FLAG32_19 = 1U << 19,
	EBIT_FLAG32_20 = 1U << 20,
	EBIT_FLAG32_21 = 1U << 21,
	EBIT_FLAG32_22 = 1U << 22,
	EBIT_FLAG32_23 = 1U << 23,
	EBIT_FLAG32_24 = 1U << 24,
	EBIT_FLAG32_25 = 1U << 25,
	EBIT_FLAG32_26 = 1U << 26,
	EBIT_FLAG32_27 = 1U << 27,
	EBIT_FLAG32_28 = 1U << 28,
	EBIT_FLAG32_29 = 1U << 29,
	EBIT_FLAG32_30 = 1U << 30,
	EBIT_FLAG32_31 = 1U << 31,
};
#pragma endregion

enum class OBJSTATE
{
	WillRemoved = EBIT_FLAG32_0,
	Active = EBIT_FLAG32_1,				// 활성화
	Pause = EBIT_FLAG32_2,				// 완전 정지
	Begin_Play = EBIT_FLAG32_3,			// 생성 후 첫 틱
	End_Play = EBIT_FLAG32_4,			// 죽기 전 마지막 틱
	Render = EBIT_FLAG32_5,				// 렌더링 유무	
	Tick = EBIT_FLAG32_6,				// 틱
	DynamicObject = EBIT_FLAG32_7,		// 동적 객체 확인용 태그 
	Cull = EBIT_FLAG32_8,				// 컬링 옵션
	Shadow = EBIT_FLAG32_9,				// 그림자 그리기 옵션
	Tool = EBIT_FLAG32_10,				// 툴 태그
	DeadAnim = EBIT_FLAG32_11			// 죽기 전 애니메이션 상태	
};


enum DIR_TYPE
{
	NONE = 0, UP = (0x01 << 0), DOWN = (0x01 << 1), RIGHT = (0x01 << 2), LEFT = (0x01 << 3)
};

enum RENDERTYPE
{
	DEFAULT,MOTIONBLUR,RENDERTYPE_END
};

enum TEXTURETYPE
{
	TextureType_NONE = 0,
	TextureType_DIFFUSE = 1,
	TextureType_SPECULAR = 2,
	TextureType_AMBIENT = 3,
	TextureType_EMISSIVE = 4,
	TextureType_HEIGHT = 5,
	TextureType_NORMALS = 6,
	TextureType_SHININESS = 7,
	TextureType_OPACITY = 8,
	TextureType_DISPLACEMENT = 9,
	TextureType_LIGHTMAP = 10,
	TextureType_REFLECTION = 11,
	TextureType_BASE_COLOR = 12,
	TextureType_NORMAL_CAMERA = 13,
	TextureType_EMISSION_COLOR = 14,
	TextureType_METALNESS = 15,
	TextureType_DIFFUSE_ROUGHNESS = 16,
	TextureType_AMBIENT_OCCLUSION = 17,
	TextureType_UNKNOWN = 18,
	TextureType_SHEEN = 19,
	TextureType_CLEARCOAT = 20,
	TextureType_TRANSMISSION = 21
};

#define TEXTURETYPE_MAX  TextureType_UNKNOWN


enum PHYSXACTOR_TYPE
{
	ACTOR_DYNAMIC, //움직이는 액터
	ACTOR_YFIXED_DYNAMIC,
	ACTOR_STATIC, //바닥, 벽 등 안움직이는 액터
	ACTOR_COLLISION_NONE, //콜라이더 X 또는 충돌처리 안할 것
	ACTOR_TYPE_END
};


enum class PHYSXCOLLIDER_TYPE
{
	SPHERE,
	BOX,
	CYLINDER,
	CONE,
	CONVEXMESH,
	MESH,
	MODEL,
	CONVEXMODEL,
	BOXWITHPOINTS,
	TYPE_END
};