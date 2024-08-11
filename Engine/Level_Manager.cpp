#include "..\Public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Initialize()
{


	return S_OK;
}

void CLevel_Manager::Tick(_cref_time fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);
}

void CLevel_Manager::Late_Tick(_cref_time fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Late_Tick(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		RETURN_EFAIL;

	return m_pCurrentLevel->Render();
}


HRESULT CLevel_Manager::Open_Level(_uint iCurrentLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == pNewLevel)
		RETURN_EFAIL;

	/* 자원들을 정리한다.*/
	if (nullptr != m_pCurrentLevel)
		m_pGameInstance->Clear(m_iCurrentLevelIndex);

	if (0 != Safe_Release(m_pCurrentLevel))
		RETURN_EFAIL;

	m_pCurrentLevel = pNewLevel;

	m_iCurrentLevelIndex = iCurrentLevelIndex;

	return S_OK;
}

CLevel_Manager * CLevel_Manager::Create()
{
	CLevel_Manager*		pInstance = new CLevel_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCurrentLevel);
}

