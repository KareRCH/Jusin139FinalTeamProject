#pragma once

#pragma warning(disable : 4251)
#pragma warning(disable : 26495)	// ��Ƽ����Ʈ ���� ����
#pragma warning(disable : 4244)		// wchar_t���� const _Elem ��ȯ�� ������ �սǰ�� ����
#pragma warning(disable: 4503)		// ���ø� �ν��Ͻ�ȭ ��� ������
#pragma warning(disable: 4267)		// �� �̰�Ǯ�� ��� 10�ﰳ ���ɴϴ�.
#pragma warning(disable: 4099)		// PDB ã�� �� ���� ��� ����


#define IMGUI_DEFINE_MATH_OPERATORS
namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };
	enum SPACETYPE { SP_LOCAL, SP_WORLD, SP_PROJ, SP_VIEW };
	enum KEYSTATE {KEY_DOWN,KEY_UP,KEY_PRESS,KEY_NONE};

}

// DirectX11 ���̺귯��
#include <d3d11.h>
#include <DirectXMath.h>
#include "Effects11/d3dx11effect.h"

// ���̷�ƮXTK ���̺귯��
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/ScreenGrab.h"
#include <DirectXCollision.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/VertexTypes.h>

#include <wrl/client.h>
using namespace Microsoft::WRL;

// ����� ���̺귯��
#ifdef _DEBUG
#include <scene.h>
#include <postprocess.h>
#include <Importer.hpp>
#endif

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <d3dcompiler.h>

using namespace DirectX;

// ���Ĵٵ� ���̺귯��
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <queue>

#include <typeinfo>
#include <functional>
#include <string>
#include <cassert>
#include <memory>
#include <sstream>
#include <limits>
#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <future>
using namespace std;

// ����ó��
#include <Vfw.h>
#pragma comment(lib, "vfw32.lib")

// FMOD ���� ���̺귯��
#include <fmod.h>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_studio.h>
#include <fmod_studio.hpp>

#include <io.h>				// ������ ����
#include <fmod_common.h>



// ���ǵ� ���̽� ���̺귯��
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include <locale>
#include <codecvt>
using namespace rapidjson;

#include "commdlg.h"
#include "shlwapi.h"
#include <fstream>

//HBAO+ ���̺귯��
#include "HBAO/GFSDK_SSAO.h"

//PhysX ���̺귯��
#include <PhysX/PxPhysics.h>
#include <PhysX/PxPhysicsAPI.h>
using namespace physx;


#include "Delegate.h"


namespace Engine
{
	static const wchar_t* g_pTransformTag = TEXT("Com_Transform");
}

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Enums.h"
using namespace Engine;


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG




