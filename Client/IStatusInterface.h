#pragma once

#include "Base.h"
//#include "StatusComp.h"

/*

*/
#define STATUS_DISPLAY_HP 0

BEGIN(Engine)
class CGameObject;
class CPhysX_Collider;
END

BEGIN(Client)

/// <summary>
/// �������ͽ� �������
/// </summary>
enum class EStatusModified : _uint
{
	None = EBIT_FLAG32_0,
	Block = EBIT_FLAG32_1,
	Damage = EBIT_FLAG32_2,
	Critical = EBIT_FLAG32_3,
	Burst = EBIT_FLAG32_4,
	BurstCritical = EBIT_FLAG32_5,
	Guard = EBIT_FLAG32_6,
	GuardCritical = EBIT_FLAG32_7,
	Heal = EBIT_FLAG32_8
};

/// <summary>
/// �������ͽ����� ��ȣ�ۿ��� ����ϴ� �������̽� �Դϴ�.
/// </summary>
class IStatusInterface abstract
{
public:
	// ����� ����� �����մϴ�. �̹� ������ �Լ�
	_uint Status_DamageTo(const string& strSkillName, CPhysX_Collider* pDstCol, weak_ptr<CGameObject> pDst, weak_ptr<CGameObject> pSrc);
	// ������� �޾��� �� �۵��� �Լ�
	virtual void Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower) = 0;
	// ��ӹ޾� �����Ͽ� ���� �Լ���
	virtual HRESULT Set_StatusComByGameObject(shared_ptr<CGameObject> pGameObject) = 0;
	virtual HRESULT Set_StatusComByOwner(const string& strSkillName) = 0;
	virtual weak_ptr<class CStatusComp> Get_StatusCom() = 0;

protected:
	//weak_ptr<class CStatusComp>	m_pStatusCom;	// ����� ����� ���� �����ϴ� �������ͽ�
};

END