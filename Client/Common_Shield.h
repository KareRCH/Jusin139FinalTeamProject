#pragma once

#include "Client_Defines.h"
#include "Bullet.h"
#include "Utility/LogicDeviceBasic.h"

BEGIN(Client)

class CEffect;

/// <summary>
/// ����� ���佺Ʈ ����ü.
/// 
/// </summary>
class CCommon_Shield : public CBullet
{
	INFO_CLASS(CCommon_Shield, CBullet)

private:
	CCommon_Shield(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CCommon_Shield(const CCommon_Shield& rhs);
	virtual ~CCommon_Shield() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual void End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components(void* pArg);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	static shared_ptr<CCommon_Shield> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

public:
	void Set_Offset(_float3 vPos) { m_vOffset = vPos; }

private:
	_bool m_bIsChaseEnd = { false };		// ü�̽� �����ϸ� �� ����
	_float3	m_vTargetPos = {};				// ��ǥ Ÿ������.
	_float m_fAccChaseValue = { 0.f };		// ���� ���� ũ��. ���� Ŀ������ �����.
	_float m_fSpeed = { 3.2f };
	_float m_fCheckPosTime = { 1.f };		// ���� ���� �ð�
	_float m_fRadius = { 20.f };				// ���� �Ÿ��� ����

	FGauge			m_fFlameTickStart = { 0.2f };
	FTimeChecker	m_fFlameTick = { 0.05f };
	_float3			m_vOffset = { };

private:
	shared_ptr<CEffect>	m_pEffect = { nullptr };
	shared_ptr<CEffect> m_pDisasterEffect = { nullptr };

};

END