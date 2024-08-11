#include "AnimNotifyComp.h"



CAnimNotifyComp::CAnimNotifyComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pDevice, pContext)
{
}

CAnimNotifyComp::CAnimNotifyComp(const CAnimNotifyComp& rhs)
	: BASE(rhs)
	, m_NotifyCollections(rhs.m_NotifyCollections)
	, m_RegisterCallback(rhs.m_RegisterCallback)
{

	for (auto iter = m_NotifyCollections.begin(); iter != m_NotifyCollections.end(); ++iter)
	{
		(*iter).second = (*iter).second->Clone();
	}
}

HRESULT CAnimNotifyComp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimNotifyComp::Initialize_Prototype(const wstring& strNotifyFilePath, NotifyRegisterDelegate Register)
{
	m_RegisterCallback = Register;

	FRapidJson Data;
	if (FAILED(Load_AnimNotify(strNotifyFilePath)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAnimNotifyComp::Initialize(void* pArg)
{
	return S_OK;
}

shared_ptr<CAnimNotifyComp> CAnimNotifyComp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	DERIVED* pInstance = new DERIVED(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CAnimNotifyComp : Create Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared(pInstance);
}

shared_ptr<CAnimNotifyComp> CAnimNotifyComp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
	const wstring& strNotifyFilePath, NotifyRegisterDelegate Register)
{
	DERIVED* pInstance = new DERIVED(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNotifyFilePath, Register)))
	{
		MSG_BOX("CAnimNotifyComp : Create Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared(pInstance);
}

shared_ptr<CComponent> CAnimNotifyComp::Clone(void* Arg)
{
	DERIVED* pInstance = new DERIVED(*this);

	if (FAILED(pInstance->Initialize(Arg)))
	{
		MSG_BOX("CAnimNotifyComp : Create Failed");
		Safe_Release(pInstance);
	}

	return DynPtrCast<CComponent>(BaseMakeShared(pInstance));
}

void CAnimNotifyComp::Free()
{
	__super::Free();

}


HRESULT CAnimNotifyComp::Regist_AnimNotifyCreatorCallback(NotifyRegisterDelegate Callback)
{
	if (Callback.empty())
		return E_FAIL;

	m_RegisterCallback = Callback;

	return S_OK;
}

HRESULT CAnimNotifyComp::Output_AnimNotify(const wstring& strNotifyFilePath)
{
	// ������ �ֱ�
	FRapidJson AnimListData;
	
	for (auto iter = m_NotifyCollections.begin(); iter != m_NotifyCollections.end(); ++iter)
	{
		AnimListData.Pushback_ObjectToArray("AnimLists", (*iter).second->Bake_Collection());
	}
	AnimListData.Output_Json(strNotifyFilePath);

	return S_OK;
}

HRESULT CAnimNotifyComp::Load_AnimNotify(const wstring& strNotifyFilePath)
{
	if (m_RegisterCallback.empty())
		RETURN_EFAIL;
	
	// ������ �ֱ�
	FRapidJson AnimListData;
	if (FAILED(AnimListData.Input_Json(strNotifyFilePath)))
		RETURN_EFAIL;

	// �ִϸ��̼� ����Ʈ�� ���� ��Ƽ���� ����ֱ�
	_uint iSize = AnimListData.Read_ArraySize("AnimLists");
	for (_uint i = 0; i < iSize; i++)
	{
		FRapidJson AnimData;
		AnimListData.Read_ObjectFromArray("AnimLists", i, AnimData);
		
		// �ִϸ��̼� �̸�
		string strAnimName = "";
		if (FAILED(AnimData.Read_Data("AnimName", strAnimName)))
			RETURN_EFAIL;

		// ��Ƽ���̿� �̹� ����Ǿ� �ִ��� �ľ�
		auto iter = m_NotifyCollections.find(strAnimName);
		if (iter != m_NotifyCollections.end())
			RETURN_EFAIL;

		// ��Ƽ���� �÷��� ����
		auto pCollection = m_RegisterCallback();
		if (nullptr == pCollection)
			RETURN_EFAIL;

		// �߰�
		m_NotifyCollections.emplace(strAnimName, pCollection);
		pCollection->Set_AnimName(strAnimName);

		// ���� �÷��ǿ� JSON ������ ����
		pCollection->Input_Json(AnimData);
	}

	return S_OK;
}

HRESULT CAnimNotifyComp::Regist_ModelComponent(weak_ptr<CCommonModelComp> pModelComp)
{
	if (pModelComp.expired())
		RETURN_EFAIL;

	m_pModelComp = pModelComp;
	for (auto iter = m_NotifyCollections.begin(); iter != m_NotifyCollections.end(); ++iter)
	{
		(*iter).second->Regist_ModelComponent(pModelComp);
	}

	return S_OK;
}

HRESULT CAnimNotifyComp::Regist_EventToNotify(
	const string& strAnimName, const string& strGroupName,
	const string& strNotifyName, FastDelegate0<> Event)
{
	// �ִϸ��̼� �̸����� ��Ƽ���� �÷����� ã�´�.
	auto iter = m_NotifyCollections.find(strAnimName);
	if (iter != m_NotifyCollections.end())
	{
		// �÷����� ������ ó���Ѵ�.
		(*iter).second->Regist_EventToNotify(strGroupName, strNotifyName, Event);
	}
	else
		RETURN_EFAIL;

	return S_OK;
}

void CAnimNotifyComp::Clear_NotifyAll()
{
	m_NotifyCollections.clear();
}

void CAnimNotifyComp::Trigger_AnimNotifyByCurAnim(const _float& fTrackPos)
{
	// �ִϸ��̼� �̸����� ��Ƽ���� �÷����� ã�´�.
	auto iter = m_NotifyCollections.find(m_strAnimName);
	if (iter != m_NotifyCollections.end())
	{
		// �÷����� ������ ó���Ѵ�.
		(*iter).second->OnTrigger(fTrackPos);
	}
}

void CAnimNotifyComp::ResetTrigger_AnimNotifyByCurAnim()
{
	// �ִϸ��̼� �̸����� ��Ƽ���� �÷����� ã�´�.
	auto iter = m_NotifyCollections.find(m_strAnimName);
	if (iter != m_NotifyCollections.end())
	{
		// �÷����� ������ ó���Ѵ�.
		(*iter).second->OnReset();
	}
}

void CAnimNotifyComp::Set_CurrentAnimation(const string& strAnimName)
{
	if (m_strAnimName != strAnimName)
		ResetTrigger_AnimNotifyByCurAnim();
	m_strAnimName = strAnimName;
}

void CAnimNotifyComp::Reserve_Collection(const string& strCollectionName)
{
	if (m_RegisterCallback.empty())
		return;

	auto iter = m_NotifyCollections.find(m_strAnimName);
	if (iter == m_NotifyCollections.end())
	{
		auto pCollection = m_RegisterCallback();
		m_NotifyCollections.emplace(m_strAnimName, pCollection);
		pCollection->Reserve_Collection(strCollectionName);
		pCollection->Set_AnimName(m_strAnimName);
	}
	else
		(*iter).second->Reserve_Collection(strCollectionName);
}

shared_ptr<CAnimNotifyCollection> CAnimNotifyComp::Get_CollectionByCurAnim()
{
	auto iter = m_NotifyCollections.find(m_strAnimName);
	if (iter == m_NotifyCollections.end())
		return nullptr;

	return (*iter).second;
}
