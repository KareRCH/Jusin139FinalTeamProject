#include "stdafx.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_HeatDmg.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"

CState_AirBurster_HeatDmg::CState_AirBurster_HeatDmg(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_HeatDmg::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle02_1", 1.f, false);

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Pass
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Wait_State<CState_AirBurster_Transform>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.leaf<FunctionNode>(Pass)
			.end()
			.build();
	}
	return S_OK;
}

void CState_AirBurster_HeatDmg::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_HeatDmg::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_HeatDmg::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_HeatDmg::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_AirBurster_HeatDmg::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_AirBurster_HeatDmg::Free()
{
}
