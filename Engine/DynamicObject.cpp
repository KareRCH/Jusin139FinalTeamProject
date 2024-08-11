#include "DynamicObject.h"

#include "PhysX_Collider.h"
#include "PhysX_Controller.h"

IMPLEMENT_CREATE(CDynamicObject)
IMPLEMENT_CLONE(CDynamicObject, CGameObject)

CDynamicObject::CDynamicObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CDynamicObject::CDynamicObject(const CDynamicObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDynamicObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CDynamicObject::Initialize(void* pArg)
{
	TurnOn_State(OBJSTATE::DynamicObject);

	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CDynamicObject::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CDynamicObject::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	//PhysX ����� Transform�� ����
	//Controller������ Controller�� PhysX ����� ����, �������� Collider�� PhysX ����� ����
	//Collider�� RigidDynamic + NON-Kinematic ���� �Ͽ� ����ǵ��� �Ѵ�. (Kinematic�� PhysX ��� � ���� X)
	if (m_pPhysXControllerCom)
		m_pPhysXControllerCom->Synchronize_Transform(m_pTransformCom, m_vPhysXControllerLocalOffset, m_vPhysXControllerWorldOffset);
	else if (m_pPhysXColliderCom && !m_pPhysXColliderCom->Get_ColliderDesc().bKinematic)
		m_pPhysXColliderCom->Synchronize_Transform(m_pTransformCom,m_vPhysXColliderLocalOffset,m_vPhysXColliderWorldOffset);
}

void CDynamicObject::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDynamicObject::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CDynamicObject::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
	
	PxControllerFilters filters;

	//�߷� ���� (�߷� ���°� Controller���� m_bEnableGravity ��Ȱ��ȭ)
	if (m_pPhysXControllerCom)
	{
		PxControllerCollisionFlags m_LastCollisionFlags = m_pPhysXControllerCom->MoveGravity(fTimeDelta, filters);
		if (m_LastCollisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)	
			m_pPhysXControllerCom->Reset_Gravity();
	}	

	//Transform����� PhysX�� ���� (Controller�� Transform���� �����϶� �ٷ� �����,NonKinematic�� Transform���� �������� �����Ƿ� PhysX ����� Transform�� �����ϴ� �۾��� �Ѵ�.)
	if (m_pPhysXColliderCom && m_pPhysXColliderCom->Get_ColliderDesc().bKinematic)
		m_pPhysXColliderCom->Synchronize_Collider(m_pTransformCom, m_vPhysXColliderLocalOffset, m_vPhysXColliderWorldOffset);
}

void CDynamicObject::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CDynamicObject::Render()
{
	return S_OK;
}

HRESULT CDynamicObject::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CDynamicObject::Bind_ShaderResources()
{
	return S_OK;
}

void CDynamicObject::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CDynamicObject::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CDynamicObject::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CDynamicObject::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CDynamicObject::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CDynamicObject::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CDynamicObject::Free()
{
	__super::Free();
}
