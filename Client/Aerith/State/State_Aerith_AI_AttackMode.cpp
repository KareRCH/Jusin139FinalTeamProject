#include "stdafx.h"
#include "Aerith/State/State_List_Aerith.h"
#include "Event_Manager.h"
#include "StatusComp.h"

using namespace STATE_AERITH;

CState_Aerith_AI_AttackMode::CState_Aerith_AI_AttackMode(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Aerith(pActor, pStatemachine)
{
}

HRESULT CState_Aerith_AI_AttackMode::Initialize_State(CState* pPreviousState)
{
	if (m_pBehaviorTree == nullptr)
		Build_BehaviorTree();

	if (pPreviousState && (typeid(*pPreviousState) == typeid(CState_Aerith_Damage_Back) || typeid(*pPreviousState) == typeid(CState_Aerith_Damage_Front)))
		++m_iDamageCount;

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	m_pActor_ModelCom.lock()->Set_Animation(m_strBattleMode[0] + "B_Idle01_1", 1.f, true);

	return S_OK;
}

void CState_Aerith_AI_AttackMode::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	if (m_bPlayable)
	{
		m_pStateMachineCom.lock()->Set_State<CState_Aerith_Idle>();
		return;
	}

	eVirtualActionKey = VIRTUAL_ACTIONKEY_NONE;

	if (!m_TargetInfo.pTarget.lock() || m_TargetInfo.pTarget.lock()->IsState(OBJSTATE::DeadAnim) || !m_TargetInfo.pTarget.lock()->IsState(OBJSTATE::Active))
	{
		m_TargetInfo = GET_SINGLE(CClient_Manager)->Find_TargetMonster(AERITH, 0);
	}
	else
		GET_SINGLE(CClient_Manager)->Update_TargetInfo(m_pActor, m_TargetInfo);

	m_pBehaviorTree->getBlackboard()->m_vPosition = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);

	m_pBehaviorTree->update(fTimeDelta);

	m_bIsExcute = Action_Key_Input(m_pGameInstance, m_pStateMachineCom, m_bPlayable);
}

void CState_Aerith_AI_AttackMode::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
}

void CState_Aerith_AI_AttackMode::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Aerith_AI_AttackMode::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Aerith_AI_AttackMode::isValid_NextState(CState* state)
{
	return true;
}

void CState_Aerith_AI_AttackMode::Build_BehaviorTree()
{
	FUNCTION_NODE Condition_Heal
		= FUNCTION_NODE_MAKE
	{
		if (m_pStatusCom.lock()->Get_HP_Percent() <= 0.4 )
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Call_Heal
		= FUNCTION_NODE_MAKE
	{
		Set_VirtualActionKey(ITEM_POTION);
		return BT_STATUS::Success;
	};


	FUNCTION_NODE Condition_AttackMode
		= FUNCTION_NODE_MAKE
	{
		if ((m_TargetInfo.pTarget.lock() == nullptr) || (m_TargetInfo.pTarget.lock()->IsState(OBJSTATE::DeadAnim) || m_TargetInfo.fDistance > 15.f || m_TargetInfo.pTarget.lock()->Get_ObjectType() == OBJTYPE::PLAYER))
			m_TargetInfo = GET_SINGLE(CClient_Manager)->Find_TargetMonster(AERITH, 0);

		if (m_TargetInfo.pTarget.lock() != nullptr)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_FollowPlayer
		= FUNCTION_NODE_MAKE
	{
		TARGETINFO TargetInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor,CLOUD);

		if (TargetInfo.fDistance >= 20.f)
		{
			m_TargetInfo = TargetInfo;
			return BT_STATUS::Success;
		}
		else
		{
			return BT_STATUS::Failure;
		}
	};

	FUNCTION_NODE Condition_ActionPower_Minus10
		= FUNCTION_NODE_MAKE
	{
		_bool bActionPower = DynPtrCast<IStatusInterface>(m_TargetInfo.pTarget.lock())->Get_StatusCom().lock()->IsActionPowerEqual(-10);

		if (bActionPower)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};


	FUNCTION_NODE Condition_ActionPower_0
		= FUNCTION_NODE_MAKE
	{
		_bool bActionPower = DynPtrCast<IStatusInterface>(m_TargetInfo.pTarget.lock())->Get_StatusCom().lock()->IsActionPowerEqual(0);

		if (bActionPower)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};


	FUNCTION_NODE Condition_ActionPower_5
		= FUNCTION_NODE_MAKE
	{
		_bool bActionPower = DynPtrCast<IStatusInterface>(m_TargetInfo.pTarget.lock())->Get_StatusCom().lock()->IsActionPowerEqual(5);

		if (bActionPower)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};


	FUNCTION_NODE Condition_ActionPower_10
		= FUNCTION_NODE_MAKE
	{
		_bool bActionPower = DynPtrCast<IStatusInterface>(m_TargetInfo.pTarget.lock())->Get_StatusCom().lock()->IsActionPowerEqual(10);

		if (bActionPower)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_ActionPower_20
		= FUNCTION_NODE_MAKE
	{
		if (!m_TargetInfo.pTarget.lock())
			return BT_STATUS::Success;

		_bool bActionPower = DynPtrCast<IStatusInterface>(m_TargetInfo.pTarget.lock())->Get_StatusCom().lock()->IsActionPowerEqual(20);
		if (bActionPower)
		{
			m_TargetInfo = TARGETINFO();
		}
		return BT_STATUS::Success;		
	};

	FUNCTION_NODE Call_Idle
		= FUNCTION_NODE_MAKE
	{
		if (!m_pActor_ModelCom.lock()->IsCurrentAnimation(m_strBattleMode[0] + "B_Idle01_1"))
		{
			m_pActor_ModelCom.lock()->Set_Animation(m_strBattleMode[0] + "B_Idle01_1", 1.f, true);
			m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
		}

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Random_SkillType
		= FUNCTION_NODE_MAKE
	{
		_int iRandomNum = m_pGameInstance->RandomInt(0,3);
		pBlackboard->setInt("SkillType", iRandomNum);
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Condition_Forward_Distance
		= FUNCTION_NODE_MAKE
	{
		if (m_TargetInfo.fDistance >= 5.f)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Condition_Backward_Distance
		= FUNCTION_NODE_MAKE
	{
		if (m_TargetInfo.fDistance <= 0.7f)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Call_LookToTarget
		= FUNCTION_NODE_MAKE
	{
		m_pActor_TransformCom.lock()->Set_Look(m_TargetInfo.vDirToTarget);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_LookToTarget_Inv
		= FUNCTION_NODE_MAKE
	{
		m_pActor_TransformCom.lock()->Set_Look(-m_TargetInfo.vDirToTarget);

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_TurnToTarget
		= FUNCTION_NODE_MAKE
	{
		m_pActor_TransformCom.lock()->Set_BaseDir(m_TargetInfo.vDirToTarget);

		if (m_pActor_TransformCom.lock()->Rotate_On_BaseDir(fTimeDelta, 0.f))
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;
	};



	FUNCTION_NODE Call_TurnToTarget_WithAnim
		= FUNCTION_NODE_MAKE
	{
		_bool bRun = eVirtualMoveKey == RUN ? true : false;
		m_DirKeyInfo = STATE_AERITH::eDirKeyInfo;

		if (m_pActor_ModelCom.lock()->Get_CurrentAnimationName().find("Run") == string::npos && m_pActor_ModelCom.lock()->Get_CurrentAnimationName().find("Walk") == string::npos)
		{
			m_pActor_TransformCom.lock()->Set_BaseDir(XMVector3Normalize(m_TargetInfo.vTargetPos - pBlackboard->m_vPosition));
			_float fAngle = m_pActor_TransformCom.lock()->Check_Look_On_BaseDir(m_DirKeyInfo.fRadianFromBaseDir);
			_float fDegree = XMConvertToDegrees(fAngle);

			m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

			if (InRange(fAngle, 0.35f * PI, 0.65f * PI, "[]"))
			{
				bRun ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0_R90", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0_R90", 2.0f, false);
				pBlackboard->setBool("TurnDir", false);
			}
			else if (InRange(fAngle, 1.35f * PI, 1.65f * PI, "[]"))
			{
				bRun ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0_L90", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0_L90", 2.0f, false);
				pBlackboard->setBool("TurnDir", false);
			}
			else if (InRange(fAngle, 0.75f * PI, 1.f * PI, "[)"))
			{
				bRun ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0_R180", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0_R180", 2.0f, false);
				pBlackboard->setBool("TurnDir", false);
			}
			else if (InRange(fAngle, 1.f * PI, 1.25f * PI, "[)"))
			{
				bRun ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0_L180", 2.0f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0_L180", 2.0f, false);
				pBlackboard->setBool("TurnDir", false);
			}
			else
			{
				bRun ? m_pActor_ModelCom.lock()->Set_Animation("Battle00|B_Run01_0", 1.3f, false) : m_pActor_ModelCom.lock()->Set_Animation(m_strCurBattleMode + "B_Walk01_0", 1.3f, false);
				pBlackboard->setBool("TurnDir", true);
			}
		}
		else
		{
			if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
				return BT_STATUS::SuccessForRepeater;
			else
				return BT_STATUS::Running;
		}

		if (pBlackboard->getBool("TurnDir"))
			Turn_Dir(fTimeDelta * 1.5f);

		return BT_STATUS::Running;
	};

	FUNCTION_NODE Call_Dir_UP
		= FUNCTION_NODE_MAKE
	{
		DirToRadian(UP);
		STATE_AERITH::eDirKeyInfo = m_DirKeyInfo;
		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_Dir_DOWN
		= FUNCTION_NODE_MAKE
	{
		DirToRadian(DOWN);
		STATE_AERITH::eDirKeyInfo = m_DirKeyInfo;
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Dir_DOWNLEFT
		= FUNCTION_NODE_MAKE
	{
		DirToRadian(DOWN | LEFT);
		STATE_AERITH::eDirKeyInfo = m_DirKeyInfo;
		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_Dir_RIGHT
		= FUNCTION_NODE_MAKE
	{
		DirToRadian(RIGHT);
		STATE_AERITH::eDirKeyInfo = m_DirKeyInfo;
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Dir_LEFT
		= FUNCTION_NODE_MAKE
	{
		DirToRadian(LEFT);
		STATE_AERITH::eDirKeyInfo = m_DirKeyInfo;
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Move_Run
		= FUNCTION_NODE_MAKE
	{
		Set_VirtualMoveKey(RUN);
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Move_Walk
		= FUNCTION_NODE_MAKE
	{
		Set_VirtualMoveKey(WALK);
		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_Move_WalkB
		= FUNCTION_NODE_MAKE
	{
		Set_VirtualMoveKey(WALKB);
		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_Move_WalkL
		= FUNCTION_NODE_MAKE
	{
		Set_VirtualMoveKey(WALKL);
		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_Move_WalkR
		= FUNCTION_NODE_MAKE
	{
		Set_VirtualMoveKey(WALKR);
		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_MoveToTarget_M5F
		= FUNCTION_NODE_MAKE
	{
		eVirtualMoveKey == RUN ? m_pStateMachineCom.lock()->Set_State<CState_Aerith_Run>() : m_pStateMachineCom.lock()->Set_State<CState_Aerith_Walk>();

		m_pGameInstance->Add_TickEvent(m_pActor.lock(), TICKEVENT_MAKE
		{
			if (m_bPlayable || !GET_SINGLE(CClient_Manager)->Get_AttackMode())
				return false;

			GET_SINGLE(CClient_Manager)->Update_TargetInfo(m_pActor, m_TargetInfo);

			if (m_TargetInfo.fDistance >= 6.f)
				eVirtualMoveKey = RUN;
			else
				eVirtualMoveKey = WALK;

			if (m_TargetInfo.fDistance <= 5.f)
			{
				if (m_TargetInfo.pTarget.lock() && m_TargetInfo.pTarget.lock()->Get_ObjectType() != OBJTYPE::PLAYER)
					Set_VirtualActionKey(NORMAL_ATTACK1);
				else
					Set_VirtualActionKey(VIRTUAL_ACTIONKEY_NONE);

				return false;
			}
			else
			{
				CState* pCurState = m_pStateMachineCom.lock()->Get_CurState();
				if (typeid(*pCurState) == typeid(CState_Aerith_AI_AttackMode))
					eVirtualMoveKey == RUN ? m_pStateMachineCom.lock()->Set_State<CState_Aerith_Run>() : m_pStateMachineCom.lock()->Set_State<CState_Aerith_Walk>();

				m_pActor_TransformCom.lock()->Set_BaseDir(m_TargetInfo.vDirToTarget);
				return true;
			}
		});

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_MoveToTarget_L5F
		= FUNCTION_NODE_MAKE
	{
		switch (STATE_AERITH::eVirtualMoveKey)
		{
		case STATE_AERITH::WALK:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_Walk>();
			break;
		case STATE_AERITH::WALKR:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkR>();
			break;
		case STATE_AERITH::WALKL:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkL>();
			break;
		case STATE_AERITH::WALKB:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkB>();
			break;
		case STATE_AERITH::RUN:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_Run>();
			break;
		}

		m_pGameInstance->Add_TickEvent(m_pActor.lock(), TICKEVENT_MAKE
		{
			if (m_bPlayable || !GET_SINGLE(CClient_Manager)->Get_AttackMode())
				return false;

			GET_SINGLE(CClient_Manager)->Update_TargetInfo(m_pActor, m_TargetInfo);

			if (m_TargetInfo.fDistance >= 5.f)
			{
				Reset_VirtualKey();
				return false;
			}
			else
			{
				CState* pCurState = m_pStateMachineCom.lock()->Get_CurState();
				if (typeid(*pCurState) == typeid(CState_Aerith_AI_AttackMode))
				{
					switch (STATE_AERITH::eVirtualMoveKey)
					{
					case STATE_AERITH::WALK:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_Walk>();
						break;
					case STATE_AERITH::WALKR:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkR>();
						break;
					case STATE_AERITH::WALKL:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkL>();
						break;
					case STATE_AERITH::WALKB:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkB>();
						break;
					case STATE_AERITH::RUN:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_Run>();
						break;
					}
				}

				m_pActor_TransformCom.lock()->Set_BaseDir(m_TargetInfo.vDirToTarget);
				return true;
			}
		});

		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_TimeMax_1F
		= FUNCTION_NODE_MAKE
	{
		m_fTimeMax = 1.f;
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_TimeMax_1F_HALF
		= FUNCTION_NODE_MAKE
	{
		m_fTimeMax = 1.5f;
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_TimeMax_2F
		= FUNCTION_NODE_MAKE
	{
		m_fTimeMax = 2.f;
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_TimeMax_2F_HALF
		= FUNCTION_NODE_MAKE
	{
		m_fTimeMax = 2.5f;
		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_MoveToTarget_Time
		= FUNCTION_NODE_MAKE
	{
		switch (STATE_AERITH::eVirtualMoveKey)
		{
		case STATE_AERITH::WALK:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_Walk>();
			break;
		case STATE_AERITH::WALKR:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkR>();
			break;
		case STATE_AERITH::WALKL:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkL>();
			break;
		case STATE_AERITH::WALKB:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkB>();
			break;
		case STATE_AERITH::RUN:
			m_pStateMachineCom.lock()->Set_State<CState_Aerith_Run>();
			break;
		}

		m_pGameInstance->Add_TickEvent(m_pActor.lock(), TICKEVENT_MAKE
		{
			if (m_bPlayable)
				return false;

			GET_SINGLE(CClient_Manager)->Update_TargetInfo(m_pActor, m_TargetInfo);


			if (m_fTimeMax.Update(fTimeDelta))
			{
				//if (m_TargetInfo.pTarget.lock() && m_TargetInfo.pTarget.lock()->Get_ObjectType() != OBJTYPE::PLAYER)
				//	Set_VirtualActionKey(ASSERT_ATTACK1);
				//else
					Set_VirtualActionKey(STATE_AERITH::VIRTUAL_ACTIONKEY_NONE);

				return false;
			}
			else
			{
				CState* pCurState = m_pStateMachineCom.lock()->Get_CurState();
				if (typeid(*pCurState) == typeid(CState_Aerith_AI_AttackMode))
				{
					switch (STATE_AERITH::eVirtualMoveKey)
					{
					case STATE_AERITH::WALK:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_Walk>();
						break;
					case STATE_AERITH::WALKR:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkR>();
						break;
					case STATE_AERITH::WALKL:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkL>();
						break;
					case STATE_AERITH::WALKB:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_WalkB>();
						break;
					case STATE_AERITH::RUN:
						m_pStateMachineCom.lock()->Set_State<CState_Aerith_Run>();
						break;
					}
				}
				m_pActor_TransformCom.lock()->Set_BaseDir(m_TargetInfo.vDirToTarget);
				return true;
			}
		});

		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_Action_NormalAttack_1
		= FUNCTION_NODE_MAKE
	{
		STATE_AERITH::eVirtualActionKey = STATE_AERITH::NORMAL_ATTACK1;

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Action_NormalAttack_2
		= FUNCTION_NODE_MAKE
	{
		STATE_AERITH::eVirtualActionKey = STATE_AERITH::NORMAL_ATTACK2;

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Action_NormalAttack_3
		= FUNCTION_NODE_MAKE
	{
		STATE_AERITH::eVirtualActionKey = STATE_AERITH::NORMAL_ATTACK3;

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Action_NormalAttack_4
		= FUNCTION_NODE_MAKE
	{
		STATE_AERITH::eVirtualActionKey = STATE_AERITH::NORMAL_ATTACK4;

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Action_NormalAttack_5
		= FUNCTION_NODE_MAKE
	{
		STATE_AERITH::eVirtualActionKey = STATE_AERITH::NORMAL_ATTACK5;

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Action_SpecialAttack_2
		= FUNCTION_NODE_MAKE
	{
		STATE_AERITH::eVirtualActionKey = STATE_AERITH::SPECIAL_ATTACK2;

		return BT_STATUS::Success;
	};

	FUNCTION_NODE Call_Action_SpecialAttack_3
		= FUNCTION_NODE_MAKE
	{
		STATE_AERITH::eVirtualActionKey = STATE_AERITH::SPECIAL_ATTACK3;

		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_Action_AbilityRayOfJudge
		= FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("SkillType") == 0)
		{
			STATE_AERITH::eVirtualActionKey = STATE_AERITH::ABILITY_ATTACK_RAYOFJUDGE;
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Call_Action_AbilitySorcerousStorm
		= FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("SkillType") == 1)
		{
			STATE_AERITH::eVirtualActionKey = STATE_AERITH::ABILITY_ATTACK_SORCEROUSSTORM;
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;

	};

	FUNCTION_NODE Call_Action_AbilityFlameSpell
		= FUNCTION_NODE_MAKE
	{
		if (pBlackboard->getInt("SkillType") == 2)
		{
			STATE_AERITH::eVirtualActionKey = STATE_AERITH::ABILITY_ATTACK_FLAMESPELL;
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;

	};




	FUNCTION_NODE Condition_Avoid
		= FUNCTION_NODE_MAKE
	{
		if (m_iDamageCount >= 5)
		{
			m_iDamageCount = 0;
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	};

	FUNCTION_NODE Call_Action_Avoid
		= FUNCTION_NODE_MAKE
	{
		STATE_AERITH::eVirtualActionKey = STATE_AERITH::AVOID;

		return BT_STATUS::Success;
	};


	FUNCTION_NODE Call_Action_Guard
		= FUNCTION_NODE_MAKE
	{
		STATE_AERITH::eVirtualActionKey = STATE_AERITH::GUARD;
		//Guard�ð� + TickEvnet�ֱ�
		m_fTimeMax = 4.f;

		m_pStateMachineCom.lock()->Set_State<CState_Aerith_Guard>();
		m_pGameInstance->Add_TickEvent(m_pActor.lock(), TICKEVENT_MAKE
		{

			if (m_bPlayable || !GET_SINGLE(CClient_Manager)->Get_AttackMode())
			{
				m_pStateMachineCom.lock()->Set_State<CState_Aerith_Idle>();
				return false;
			}

			GET_SINGLE(CClient_Manager)->Update_TargetInfo(m_pActor, m_TargetInfo);
			m_pActor_TransformCom.lock()->Set_Look(m_TargetInfo.vDirToTarget);

			if (m_fTimeMax.Update(fTimeDelta) || (m_TargetInfo.pTarget.lock() && DynPtrCast<IStatusInterface>(m_TargetInfo.pTarget.lock())->Get_StatusCom().lock()->Get_ActionPower() != 5))
			{
				m_pStateMachineCom.lock()->Set_State<CState_Aerith_AI_AttackMode>();
				return false;
			}
			else
				return true;

		});

		return BT_STATUS::Success;
	};


	m_pBehaviorTree = Builder()
			.composite<Selector>()
				.composite<Sequence>() //ü�� üũ 
					.leaf<FunctionNode>(Condition_Heal)
					.leaf<FunctionNode>(Call_Heal)
				.end() 
				.composite<Sequence>()//����X + �÷��̾�� �Ÿ� ����
					.leaf<FunctionNode>(Condition_FollowPlayer)		
					.leaf<FunctionNode>(Call_Dir_UP)
					.leaf<FunctionNode>(Call_Move_Walk)
					.decorator<Repeater>()
						.leaf<FunctionNode>(Call_TurnToTarget_WithAnim)
					.end()
					.leaf<FunctionNode>(Call_MoveToTarget_M5F)
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_ActionPower_20)
					.leaf<FunctionNode>(Condition_AttackMode) //���� ����
					.composite<Selector>()
					    .composite<Sequence>()
							.leaf<FunctionNode>(Condition_Avoid)
							.leaf<FunctionNode>(Call_Dir_DOWNLEFT)
							.decorator<Repeater>()
								.leaf<FunctionNode>(Call_TurnToTarget_WithAnim)
							.end()
							.leaf<FunctionNode>(Call_Action_Avoid)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Forward_Distance)
							.leaf<FunctionNode>(Call_Dir_UP)
							.leaf<FunctionNode>(Call_Move_Walk)
							.decorator<Repeater>()
								.leaf<FunctionNode>(Call_TurnToTarget_WithAnim)
							.end()
							.leaf<FunctionNode>(Call_MoveToTarget_M5F)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Backward_Distance)
							.leaf<FunctionNode>(Call_Dir_UP)
							.leaf<FunctionNode>(Call_Move_WalkB)
							.leaf<FunctionNode>(Call_MoveToTarget_L5F)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_ActionPower_Minus10)
							.composite<StatefulSelector>()
								.composite<Sequence>()
									.leaf<FunctionNode>(Random_SkillType)
									.composite<Selector>()
										.leaf<FunctionNode>(Call_Action_AbilityRayOfJudge)
										.leaf<FunctionNode>(Call_Action_AbilityFlameSpell)
										.leaf<FunctionNode>(Call_Action_AbilitySorcerousStorm)
									.end()
								.end()
								.leaf<FunctionNode>(Call_Action_NormalAttack_2)
								.leaf<FunctionNode>(Call_Action_NormalAttack_4)
							.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_ActionPower_0)
							.composite<StatefulSelector>()	
								.leaf<FunctionNode>(Call_Action_NormalAttack_3)
								.leaf<FunctionNode>(Call_Action_SpecialAttack_3)
								.leaf<FunctionNode>(Call_Action_NormalAttack_4)
								.composite<Sequence>()
									.leaf<FunctionNode>(Call_Dir_UP)
									.leaf<FunctionNode>(Call_Move_WalkR)
									.leaf<FunctionNode>(Call_TimeMax_1F_HALF)
									.leaf<FunctionNode>(Call_MoveToTarget_Time)
								.end()
								.leaf<FunctionNode>(Call_Action_NormalAttack_5)
								.leaf<FunctionNode>(Call_Action_SpecialAttack_2)
								.composite<Sequence>()
									.leaf<FunctionNode>(Call_Dir_UP)
									.leaf<FunctionNode>(Call_Move_WalkB)
									.leaf<FunctionNode>(Call_TimeMax_1F_HALF)
									.leaf<FunctionNode>(Call_MoveToTarget_Time)
								.end()
								.leaf<FunctionNode>(Call_Action_NormalAttack_4)
								.leaf<FunctionNode>(Call_Action_SpecialAttack_3)
								.leaf<FunctionNode>(Call_Action_NormalAttack_2)
								.composite<Sequence>()
									.leaf<FunctionNode>(Call_Dir_UP)
									.leaf<FunctionNode>(Call_Move_WalkL)
									.leaf<FunctionNode>(Call_TimeMax_1F_HALF)
									.leaf<FunctionNode>(Call_MoveToTarget_Time)
								.end()
							.end()
						.end()
						.composite<Sequence>()							
							.leaf<FunctionNode>(Condition_ActionPower_5)
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Backward_Distance)
								.leaf<FunctionNode>(Call_Dir_DOWN)
								.decorator<Repeater>()
									.leaf<FunctionNode>(Call_TurnToTarget_WithAnim)
								.end()
								.leaf<FunctionNode>(Call_LookToTarget_Inv)
								.decorator<Repeater>(2)
									.leaf<FunctionNode>(Call_Action_Avoid)
								.end()						
							.end()
							.leaf<FunctionNode>(Call_Action_Guard)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_ActionPower_10)
							.leaf<FunctionNode>(Call_Dir_DOWN)
							.leaf<FunctionNode>(Call_Move_Run)
							.decorator<Repeater>()
								.leaf<FunctionNode>(Call_TurnToTarget_WithAnim)
							.end()
							.leaf<FunctionNode>(Call_MoveToTarget_L5F)
						.end()
					.end()
				.end()
				.leaf<FunctionNode>(Call_Idle)
			.end()
		.build();	
}

void CState_Aerith_AI_AttackMode::Free()
{
}