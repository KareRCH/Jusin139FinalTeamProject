#pragma once

#include "Base.h"

/* ���� ������ ƽ, ������ ������ ȣ���Ѵ�. */

/* Open_Level() */
/* ���� ȭ�鿡 ��������� ������ �ּҸ� ���´�. */
/* ���� ������ �����Ѵ�. */
/* ���� ������ �ڿ����� �����Ѵ�. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	INFO_CLASS(CLevel_Manager, CBase)

private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_cref_time fTimeDelta);
	void Late_Tick(_cref_time fTimeDelta);
	HRESULT Render();

public:
	HRESULT Open_Level(_uint iCurrentLevelIndex, class CLevel* pNewLevel);

	_bool	IsVisitedLevel(_uint iCurrentLevelIndex)
	{
		if (m_bVisitedLevelList.find(iCurrentLevelIndex) == m_bVisitedLevelList.end())
			return false;
		else
			return true;
	}

	void	Set_VisitedLevel(_uint iCurrentLevelIndex)
	{
			m_bVisitedLevelList.insert(iCurrentLevelIndex);
	}
		
	_uint	Get_CreateLevelIndex() { return m_iCreateLevelIndex; }
	void	Set_CreateLevelIndex(_uint eLevelIndex) { m_iCreateLevelIndex = eLevelIndex; }

	class CLevel* Get_CurrentLevel() { return m_pCurrentLevel; }
	_uint	Get_CurrentLevelIndex() { return m_iCurrentLevelIndex; }

private:
	_uint						m_iCurrentLevelIndex = { 0 };
	_uint						m_iCreateLevelIndex = { 0 };
	class CLevel*				m_pCurrentLevel = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	set<_uint>					m_bVisitedLevelList;

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END