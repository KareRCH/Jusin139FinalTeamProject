#include "..\Public\Component_Manager.h"

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, shared_ptr<class CComponent> pPrototype)
{
	if (nullptr == pPrototype || 
		iLevelIndex >= m_iNumLevels || 
		nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		RETURN_EFAIL;

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

shared_ptr<class CComponent> CComponent_Manager::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, shared_ptr<CGameObject> pOwner, void* pArg)
{
	shared_ptr<class CComponent>		pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	shared_ptr<class CComponent>		pComponent = pPrototype->Clone(pArg);
	pComponent->Set_Owner(pOwner);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

void CComponent_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	m_pPrototypes[iLevelIndex].clear();
}

shared_ptr<class CComponent> CComponent_Manager::Find_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag)
{
	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLevelIndex].end())
	{
		iter = m_pPrototypes[0].find(strPrototypeTag);
		if (iter == m_pPrototypes[0].end())
			return nullptr;
	}

	return iter->second;	
}

CComponent_Manager * CComponent_Manager::Create(_uint iNumLevels)
{
	CComponent_Manager*		pInstance = new CComponent_Manager;

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CComponent_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CComponent_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
