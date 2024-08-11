#pragma once

#include "Component.h"

BEGIN(Engine)

class CGameObject;

/// <summary>
/// ��ǥ����� ���� ������Ʈ Ŭ����
/// ���� ��ǥ����� �ʿ� ���� �Ļ�Ŭ������ ����մϴ�.
/// </summary>
class ENGINE_DLL CGameObjectComp abstract : public CComponent
{
	DERIVED_CLASS(CComponent, CGameObjectComp)
protected:
	explicit CGameObjectComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CGameObjectComp(const CGameObjectComp& rhs);
	virtual ~CGameObjectComp() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* Arg = nullptr) PURE;
	virtual void	Priority_Tick(const _float& fTimeDelta) PURE;
	virtual void	Tick(const _float& fTimeDelta) PURE;
	virtual void	Late_Tick(const _float& fTimeDelta) PURE;
	virtual HRESULT	Render() PURE;


public:
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr) PURE;

protected:
	virtual void Free() override;



#pragma region �⺻ �Ӽ�
public:
	GETSET_EX2(wstring, m_strPrototypeName, ProtoName, GET_C_REF, SET_C_REF)

protected:
	wstring	m_strPrototypeName = { TEXT("") };			// ������Ÿ�� ����� ���

public:
	_float Get_Priority() const { return m_fPriority; }
	void Set_Priority(_float fPriority) { m_fPriority = fPriority; }

private:	// �⺻ �Ӽ�
	_float					m_fPriority = { 0.f };		// �켱��
#pragma endregion


#pragma region ��������
public:
	// �ܺο����� ������ ���� �Ұ��� �������� �ּҸ� ����
	GETSET_EX2(CGameObject*, m_pOwnerObject, OwnerObject, GET_REF_C, SET__C)

private:
	CGameObject* m_pOwnerObject = { nullptr };			// �����ϰ� �ִ� ���ӿ�����Ʈ, �̴� �ݵ�� ������ �������־�� ��.
#pragma endregion

};

END