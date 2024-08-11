#pragma once

#include "Engine_Defines.h"
#include "../Json/Json_Utility.h"
#include "PhysX_Utility.h"

/* ���۷���ī��Ʈ�� �����ϱ����� ����� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* ���۷��� ī��Ʈ�� ������Ű��. */
	/* _ulong : ������Ű�� �� ������ ���� ����. */
	_ulong AddRef();

	/* ���۷��� ī��Ʈ�� ���ҽ�Ű�ų� �����ϰų�.. */
	/* _ulong : ���ҽ�Ű�� ������ ���� ���� .*/
	_ulong Release();
	void Release_Shared();

	_ulong Get_RefCnt() { return m_dwRefCnt; }
private:
	_ulong			m_dwRefCnt = { 0 };

public:
	virtual void Write_Json(json& Out_Json) {};
	virtual void Load_Json(const json& In_Json) {};

public:
	virtual void Free() {}
};

END