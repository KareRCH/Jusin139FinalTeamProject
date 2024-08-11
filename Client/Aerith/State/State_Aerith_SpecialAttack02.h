#pragma once


#include "Client_Defines.h"
#include "Aerith/State/State_Aerith.h"

#include "Utility/LogicDeviceBasic.h"


BEGIN(Client)

class CState_Aerith_SpecialAttack02 : public CState_Aerith
{
	INFO_CLASS(CState_Aerith_SpecialAttack02, CState_Aerith)

public:
	explicit CState_Aerith_SpecialAttack02(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine);
	virtual ~CState_Aerith_SpecialAttack02() = default;

public:
	virtual HRESULT			Initialize_State(CState* pPreviousState)	override;
	virtual void			Priority_Tick(_cref_time fTimeDelta)		override;
	virtual void			Tick(_cref_time fTimeDelta)					override;
	virtual void			Late_Tick(_cref_time fTimeDelta)			override;
	virtual void			Transition_State(CState* pNextState)		override;
	virtual bool			isValid_NextState(CState* state)			override;

private:
	_bool 	m_bMouseDown = false;
	_bool	m_bMouseRelease = false;
	FGauge	m_fChargeGauge = FGauge(1.f);
	_bool	m_bTempest = false;		// 자치샷 발동조건

protected:
	virtual void		Free();
};

END