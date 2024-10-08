#include "stdafx.h"
#include "State_list_Cloud.h"

#include "WeaponPart.h"

CState_Cloud_BraveAttack02::CState_Cloud_BraveAttack02(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Cloud(pActor, pStatemachine)
{
}

HRESULT CState_Cloud_BraveAttack02::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	if (m_TargetInfo.pTarget.lock() == nullptr || m_TargetInfo.pTarget.expired())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(CLOUD);

	if (m_TargetInfo.pTarget.lock() != nullptr)
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);
	
	m_pActor_ModelCom.lock()->Set_Animation("Battle01|B_SAttack02_0", 1.15f, false, true, 2.f,false);

	m_bMouseDown = false;

	// ���⼳��
	auto pWeapon = DynPtrCast<CWeaponPart>(m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon")));
	if (pWeapon) { pWeapon->Set_SkillName("Cloud_BraveAttack2"); }

	TurnOn_WeaponEffect();

	m_bNextAttack = !m_bPlayable && (STATE_CLOUD::eVirtualActionKey == STATE_CLOUD::BRAVE_ATTACK3);

	return S_OK;
}

void CState_Cloud_BraveAttack02::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	TurnOnOff_Weapon(14.f, 25.f,true);

	if(!m_bMouseDown)
		m_bMouseDown = m_pGameInstance->Mouse_Down(DIM_LB);

	if ((m_bNextAttack || m_bMouseDown) && m_pActor_ModelCom.lock()->IsAnimation_Range(25.f, 27.f))
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_BraveAttack03>();
	else if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		m_pStateMachineCom.lock()->Enter_State<CState_Cloud_Idle>();
}

void CState_Cloud_BraveAttack02::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Cloud_BraveAttack02::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Cloud_BraveAttack02::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);

	TurnOff_Weapon();

	TurnOff_WeaponEffect();
}

bool CState_Cloud_BraveAttack02::isValid_NextState(CState* state)
{
	return true;
}

void CState_Cloud_BraveAttack02::Free()
{
}
