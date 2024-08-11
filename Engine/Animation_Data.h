#pragma once
#include "Base.h"
#include "Channel_Data.h"
#include "Bone_Data.h"

BEGIN(Engine)

struct ENGINE_DLL Animation_Data
{
	string szName = "";
	_uint				iNumChannels = { 0 }; /* �� �ִϸ��̼��� ����ϴ� ���� ����. */
	_float				fDuration = { 0.0f }; /* �� �ִϸ��̼��� ��ü ����ϱ����� ��ü ����. */
	_float				fTickPerSecond = { 0.f }; /* �ִϸ��̼��� ��� �ӵ� : m_TickPerSecond * fTimeDelta */

	vector<CHANNEL_DATA>	Channels;


#ifdef _DEBUG
	HRESULT Bake_AnimationData(const aiAnimation* pAIAnimation, const vector <BONE_DATA> Bones);
#endif
	HRESULT Bake_Binary(ofstream& fout);
	HRESULT	Load_Binary(ifstream& fin);
}typedef ANIMATION_DATA;


END