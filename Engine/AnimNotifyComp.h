#pragma once

#include "Component.h"
#include "AnimNotify.h"
#include "Utility/DelegateTemplate.h"

BEGIN(Engine)

typedef FastDelegate0<shared_ptr<CAnimNotifyCollection>>		NotifyRegisterDelegate;

/// <summary>
/// ��Ƽ���� �÷����� �����ϰ� �׿� ���� ó���� �����ϴ� ������Ʈ
/// </summary>
class ENGINE_DLL CAnimNotifyComp : public CComponent
{
	INFO_CLASS(CAnimNotifyComp, CComponent)
public:

protected:
	explicit CAnimNotifyComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CAnimNotifyComp(const CAnimNotifyComp& rhs);
	virtual ~CAnimNotifyComp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(const wstring& strNotifyFilePath, NotifyRegisterDelegate Register);
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	static shared_ptr<CAnimNotifyComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static shared_ptr<CAnimNotifyComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
		const wstring& strNotifyFilePath, NotifyRegisterDelegate Register);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr);

protected:
	virtual void	Free();

public:
	using SNotifyCollections = _unmap<string, shared_ptr<CAnimNotifyCollection>>;

public:
	// �ڵ� ��Ͽ� �ݹ��Լ� ��ϱ�
	HRESULT Regist_AnimNotifyCreatorCallback(NotifyRegisterDelegate Callback);
	// ��Ƽ���� ��������
	HRESULT Output_AnimNotify(const wstring& strNotifyFilePath);
	// ��Ƽ���� �ε�
	HRESULT Load_AnimNotify(const wstring& strNotifyFilePath);
	// �� ���
	HRESULT Regist_ModelComponent(weak_ptr<CCommonModelComp> pModelComp);
	// ��Ƽ���̿� �Լ� ���, ��Ÿ�ӿ� ����ؾ���.
	HRESULT Regist_EventToNotify(const string& strAnimName, const string& strGroupName, 
		const string& strNotifyName, FastDelegate0<> Event);
	// ��� ��Ƽ���̸� ����.
	void Clear_NotifyAll();

	// ���� �ִϸ��̼��� ��Ƽ����
	virtual void Trigger_AnimNotifyByCurAnim(const _float& fTrackPos);
	// ���� Ʈ���� Ʈ���Ÿ� ���� ��Ų��.
	virtual void ResetTrigger_AnimNotifyByCurAnim();
	void Set_CurrentAnimation(const string& strAnimName);

public:
	// ������ ����
	void Reserve_Collection(const string& strCollectionName);
	shared_ptr<CAnimNotifyCollection> Get_CollectionByCurAnim();

protected:
	SNotifyCollections			m_NotifyCollections;		// ��Ƽ���� ������
	NotifyRegisterDelegate		m_RegisterCallback;

	string						m_strAnimName = "";			// ������� �ִϸ��̼� �̸�
	weak_ptr<CCommonModelComp>	m_pModelComp;				// �𵨿� ���� ������

};

END