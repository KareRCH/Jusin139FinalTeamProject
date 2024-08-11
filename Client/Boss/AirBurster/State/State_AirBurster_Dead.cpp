#include "stdafx.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Dead.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "Camera_Manager.h"

CState_AirBurster_Dead::CState_AirBurster_Dead(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Dead::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Die01_0", 1.f, false);

	if (m_bCheck)
	{
		m_bCheck = false;
		m_pActor.lock()->Get_PhysXColliderCom().lock()->PutToSleep();
		// �ݶ��̴� off
	}

	/*if (!m_pBehaviorTree)
	{
		

		m_pBehaviorTree = Builder()
			.build();
	}*/


	m_pActor.lock()->TurnOn_State(OBJSTATE::DeadAnim);

	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(0, 0, "AirBurster");
	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(1, 27);
	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(2, 44);

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Airburster_Dead_Following", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE, m_pActor.lock());

	m_pGameInstance->Play_BGM(TEXT("FF7_BGM"), TEXT("bgm_ungd6_03_field#1 (Music-Section0).mp3"), 1.f);

	//_float3 PosTemp = {};
	//_int	PosDir = {};
	//GET_SINGLE(CUI_Manager)->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_FloorDir"), &(PosTemp = { 84.14f, 2.61f, 93.f }), nullptr, &(PosDir = 0), "S3_FloorDir");

	m_pActor.lock()->TurnOn_State(OBJSTATE::DeadAnim);
	return S_OK;
}

void CState_AirBurster_Dead::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	//m_pBehaviorTree->update(fTimeDelta);

	//if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	//{
	//	m_pActor.lock()->Set_Dead();
	//	return;
	//} // ���� ó��
}

void CState_AirBurster_Dead::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_Dead::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Dead::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_AirBurster_Dead::isValid_NextState(CState* state)
{
	return true;

}

void CState_AirBurster_Dead::Free()
{
}
