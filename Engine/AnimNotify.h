#pragma once

#include "Base.h"
#include "Utility/DelegateTemplate.h"
#include "Utility/RapidJson_Utility.h"

BEGIN(Engine)

typedef FastDelegate0<void> NotifyEventDelegate0;

struct FNotifyType
{
	NotifyEventDelegate0	Event;
	string					strName;
};

class CAnimNotifyCollection;

/// <summary>
/// ��Ƽ������ �⺻�� �Ǵ� Ŭ����
/// �ִϸ��̼��� Ű�������� ������ �̺�Ʈ�� �߻���Ų��.
/// �̺�Ʈ�� ��Ƽ���̸� ����ϴ� ��⿡�� �۵���Ų��.(�ִϸ��̼� ������Ʈ)
/// �ش� ��⿡�� ���ǿ� ���� �ߵ���Ű��, �̴� ������Ʈ�� ���ǿ� ���� �ٲ��.
/// 
/// ��Ƽ���̴� �ִϸ��̼ǵ�� ������ Ʈ���� ������
/// Ʈ���� �ִٴ� �����Ͽ� �ִϸ��̼� ������Ʈ���� ���� Ʈ���� �Բ� üũ�Ѵ�.
/// Ʈ���� �����ϸ� 
/// </summary>
class ENGINE_DLL CAnimNotify abstract : public CBase
{
	INFO_CLASS(CAnimNotify, CBase)

public:
	explicit CAnimNotify();
	explicit CAnimNotify(const CAnimNotify& rhs);
	virtual ~CAnimNotify() = default;

public:
	virtual HRESULT Initialize() PURE;
	virtual HRESULT Initialize(_float fTrackPos, const string& strName);

	// Ʈ���Ÿ� �۵��ϱ� �����·� �ǵ�����.
	virtual void OnReset();

	// �۵��� �߻���ų ���
	virtual _bool OnTrigger();

public:
	virtual void Free() override;
	virtual shared_ptr<CAnimNotify> Clone() = 0;

public:
	virtual FRapidJson Bake_Notify();

protected:
	class CGameInstance* m_pGI = { nullptr };

public:
	void			Set_NotifyTrackPos(const _float fTrackPos);
	const _float&	Get_NotifyTrackPos() const { return m_fTrackPos; }
	void			Set_NotifyCollection(CAnimNotifyCollection* pNotifyCollection) { m_pCollection = pNotifyCollection; }
	const string&	Get_Name() const { return m_strName; }
	void			Set_Name(const string& strName) { m_strName = strName; }
	HRESULT			Set_Event(NotifyEventDelegate0 Event);

protected:
	FNotifyType*			m_pNotifyData = { nullptr };
	_float					m_fTrackPos = { 0.f };
	CAnimNotifyCollection*	m_pCollection = { nullptr };
	string					m_strName = { "" };				// ��Ƽ���̿� �Ҵ�� �̸�

private:
	_bool			m_bIsTriggered = { false };
	

};


/// <summary>
/// �÷����� �ִԳ�Ƽ���̸� �� ���� �����ϱ� ���� �ٸ� ��⿡�� ������ �������� �ʰ�
/// ����� �� �ֵ��� ĸ��ȭ�� �����ϴ� Ŭ�����Դϴ�.
/// �ϳ��� �÷��Ǵ� �� �ִϸ��̼� ��Ʈ�� ���� ��Ƽ���̸� �����մϴ�.
/// �뵵���� �÷����� ����� ����մϴ�.
/// ex) ����Ʈ, ����, �̺�Ʈ
/// </summary>
class ENGINE_DLL CAnimNotifyCollection abstract : public CBase
{
	INFO_CLASS(CAnimNotifyCollection, CBase)

protected:
	explicit CAnimNotifyCollection();
	explicit CAnimNotifyCollection(const CAnimNotifyCollection& rhs);
	virtual ~CAnimNotifyCollection() = default;

public:
	// ��� ����� ��Ƽ���̸� ���½�Ŵ
	void OnReset();

	// ���� �������� Ʈ������� ����� PrevTrackPos�� ���� Ʈ���Ÿ� �۵�
	void OnTrigger(_cref_time m_fTrackPos);

public:
	virtual void Free() override;
	virtual shared_ptr<CAnimNotifyCollection> Clone() = 0;

public:
	using SNotifyContainer = _unmap<string, vector<shared_ptr<CAnimNotify>>>;

public:
	void Regist_ModelComponent(weak_ptr<CCommonModelComp> pModelCom);
	void Reserve_Collection(const string& strNotifyTag);
	void Add_Notify(const string& strNotifyTag, shared_ptr<CAnimNotify>& pNotify);
	SNotifyContainer& Get_NotifyList() { return m_Notifies; }
	void Sort_Notifies();
	void Clear_Collections();
	void Clear_OnlyNotifies();
	vector<shared_ptr<CAnimNotify>>* Get_NotifyGroupPtr(const string& strGroupTag);

	HRESULT Regist_EventToNotify(const string& strGroupName, const string& strNotifyName, FastDelegate0<> Event);
	weak_ptr<CCommonModelComp>& Get_ModelComp() { return m_pModelCom; }
	void Set_AnimName(const string& strAnimName) { m_strAnimName = strAnimName; }
	const string& Get_AnimName() const { return m_strAnimName; }

public:
	FRapidJson Bake_Collection();
	virtual HRESULT Input_Json(FRapidJson& InputData) = 0;

protected:
	_float							m_fPrevTrackPos = { 0.f };			// Ʈ���Ÿ� �۵���Ű������ �ʿ��� ����
	SNotifyContainer				m_Notifies;							// ��Ƽ���� ���� �����̳�
	weak_ptr<CCommonModelComp>		m_pModelCom;						// ��Ƽ���̵��� �� �� ������Ʈ�� �����Ѵ�.
	string							m_strAnimName = { "" };				// Ʈ���� �̸�, �� �ִϸ��̼� �̸�

};

END