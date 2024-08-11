#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"

#include "Aerith/Weapon/Aerith_NormalBullet.h"
#include "PartObject.h"

CState_Aerith_NormalAttack04::CState_Aerith_NormalAttack04(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	: BASE(pActor, pStatemachine)
{
	// 노티파이에 공격 이벤트 연결
	auto pModelCom = pActor->Get_ModelCom().lock();
	if (pModelCom)
	{
		pModelCom->Get_AnimationComponent()
			->Regist_EventToNotify("Battle00|B_NAttack04_0", "Event", "Shoot1",
				MakeDelegate(this, &CState_Aerith_NormalAttack04::Create_Bullet));
		pModelCom->Get_AnimationComponent()
			->Regist_EventToNotify("Battle00|B_NAttack04_0", "Event", "Shoot2",
				MakeDelegate(this, &CState_Aerith_NormalAttack04::Create_Bullet));
	}
}

HRESULT CState_Aerith_NormalAttack04::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK2);

	if (m_TargetInfo.pTarget.lock() == nullptr || m_TargetInfo.pTarget.expired())
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Get_TargetMonster(AERITH);

	if (m_TargetInfo.pTarget.lock() != nullptr)
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_TargetInfo.vTargetPos);

	m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_NAttack04_0", 1.15f, false, true, 2.f);

	m_bMouseDown = false;

	m_bNextAttack = !m_bPlayable && ( STATE_AERITH::eVirtualActionKey == STATE_AERITH::NORMAL_ATTACK5);

	return S_OK;
}

void CState_Aerith_NormalAttack04::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (!m_bMouseDown)
		m_bMouseDown = m_pGameInstance->Mouse_Down(DIM_LB) || m_pGameInstance->Mouse_Pressing(DIM_LB);

	if (!m_bPlayable)
		m_bMouseDown = false;

	// 연계 코드
	if (m_bNextAttack || (m_bMouseDown && m_pActor_ModelCom.lock()->IsAnimation_Range(36.f, 37.f)))
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_NormalAttack05>();
	else if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_pStateMachineCom.lock()->Enter_State<CState_Aerith_Idle>();
		GET_SINGLE(CCamera_Manager)->Set_ThirdCam_OffsetType(OFFSET_TYPE::TYPE_ATTACK1);
	}
}

void CState_Aerith_NormalAttack04::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Aerith_NormalAttack04::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_NormalAttack04::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_cast<CState_Player*>(pNextState)->Set_TargetInfo(m_TargetInfo);
}

bool CState_Aerith_NormalAttack04::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_NormalAttack04::Free()
{
}

void CState_Aerith_NormalAttack04::Create_Bullet()
{
	shared_ptr<CAerith_NormalBullet> pBullet = { nullptr };
	m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
		TEXT("Prototype_GameObject_Aerith_NormalBullet"), nullptr, &pBullet);
	pBullet->Set_Owner(m_pActor);
	pBullet->Set_StatusComByOwner("Aerith_NormalAttack4");
	auto pPart = m_pActor.lock()->Find_PartObject(TEXT("Part_Weapon"));
	if (pPart)
	{
		_matrix SocketMatrix = pPart->Get_WorldMatrixFromBone(TEXT("L_RodAGimmickS_End"));
		_matrix ActorMatrix = m_pActor.lock()->Get_TransformCom().lock()->Get_WorldFloat4x4();
		//SocketMatrix *= ActorMatrix;

		pBullet->Get_TransformCom().lock()->Set_Look_Manual(ActorMatrix.r[2]);
		pBullet->Get_TransformCom().lock()->Set_Position(0.f, SocketMatrix.r[3]);
	}
}
