#pragma once

#include "Client_Defines.h"
#include "Component.h"
#include "Utility/LogicDeviceBasic.h"
#include "Utility/RapidJson_Utility.h"
#include "IStatusInterface.h"

BEGIN(Client)

#pragma region ���� Ÿ��

// ��ų ���̺� ��Ű��
struct TSkillSchema
{
	_int iID;
	string strSkillName;
	_int iDamageType;
	_int iBaseDamage;
	_int iPenetration;
	_int iBurst;
	_int iHitPower;
	_int iAddHitPower;
};

struct TCharacterSchema
{
	_uint iLV;
	_uint iHP;
	_uint iHP_Growth;
	_uint iMP;
	_uint iMP_Growth;
	_uint iAttack;
	_uint iAttack_Growth;
	_uint iMagic;
	_uint iMagic_Growth;
	_uint iDefence;
	_uint iDefence_Growth;
	_uint iM_Defence;
	_uint iM_Defence_Growth;
	_uint iExp_Gauge;
	_uint iExp_Growth;
};

struct TEnemySchema
{
	_uint iLV;
	_uint iHP;
	_uint iMP;
	_uint iAttack;
	_uint iMagic;
	_uint iDefence;
	_uint iM_Defence;
	_uint iExp_Give;
};

struct TActionInfoSchema
{
	_uint iMP;
};

#pragma endregion

/// <summary>
/// ĳ������ ���¸� �����ϴ� ������Ʈ
/// </summary>
class CStatusComp final : public CComponent
{
	INFO_CLASS(CStatusComp, CComponent)
public:
	enum class ETableType
	{
		Character,
		Enemy,
		Boss
	};

public:
	struct TStatusCompDesc
	{
		_int	iLV;
		_float	fHP;
		_float	fMP;
		_uint	iAttack;
		_uint	iDefence;
	};

private:
	CStatusComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CStatusComp(const CStatusComp& rhs);
	virtual ~CStatusComp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	// �����ε� ����� ������
	static shared_ptr<CStatusComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	// ���� ����� ������
	static shared_ptr<CStatusComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;

public:
	// ����� ����� ���� ������ ���̺� �ε�
	static HRESULT Initialize_SkillDataTable(const wstring& strFileName);
	static HRESULT Initialize_CharacterDataTable(const wstring& strFileName);
	static HRESULT Initialize_EnemyDataTable(const wstring& strFileName);
	static HRESULT Initialize_ActionInfoTable(const wstring& strFileName);

public:
	_uint Find_ConsumeMP(const string& strSkillName);
	static _uint StFind_ConsumeMP(const string& strSkillName);

private:
	static _unmap<string, TSkillSchema> g_SkillTable;
	static _unmap<string, TCharacterSchema> g_CharacterTable;
	static _unmap<string, TEnemySchema> g_EnemyTable;
	static _unmap<string, TEnemySchema> g_BossTable;
	static _unmap<string, TActionInfoSchema> g_ActionInfoTable;
	
public:	// ü�°��� �Լ�
	// �ܼ� GETSET �Լ���
	GETSET_EX2(FGauge, m_fHP, HP, GET_C_REF, SET)
	void Init_HP(_float fMaxHP) { m_fHP.Readjust(fMaxHP); m_fHP.Set_Max(); }
	_float	Get_HP_Percent() { return m_fHP.Get_Percent(); }
	_float  Get_CurHP() const { return m_fHP.fCur; }
	_float  Get_MaxHP() const { return m_fHP.fMax; }
	_bool	IsZeroHP() { return m_fHP.fCur <= 0.f; }
	void    Set_HP_Full() { m_fHP.Set_Max(); }
	void	Set_CurHP(_float fCur) { m_fHP.fCur = fCur; }
	// ����Ÿ�԰� �������ͽ� ������Ʈ�� ���� ������� ����ϰ� ü���� ��� �Ѵ�.
	// ��ȯ���� UI ��¿� ���δ�.
	_uint	Apply_Damage(const string& strSkillName, CStatusComp& Other, _uint& iDamageStatus, _uint& iHitPower, _uint& iOutAddHitPower);

	void	Set_Damage(_float fDamage) { m_fHP.Decrease(fDamage); }
	
public:	// MP���� �Լ�
	// �ܼ� GETSET �Լ���
	GETSET_EX2(FGauge, m_fMP, MP, GET_C_REF, SET)
	void Init_MP(_float fMaxMP) { m_fMP.Readjust(fMaxMP); m_fMP.Set_Max(); }
	_float	Get_MP_Percent() { return m_fMP.Get_Percent(); }
	_float  Get_CurMP() const { return m_fMP.fCur; }
	_float  Get_MaxMP() const { return m_fMP.fMax; }
	void	Set_CurMP(_float fCur) { m_fMP.fCur = fCur; }
	// �ܼ��� MP �Ҹ� �������� �Ǻ��ϴ� �Լ��̴�.
	_bool IsCanConsume_MP(_int iConsumeMP) const;
	// ��ȯ���� �Ҹ� �����ߴ��� �Ǵ��ϰ� MP�� ��´�.
	_bool Consume_MP(_int iConsumeMP);

public:
	const FGauge& CRef_LimitBreak() const { return m_fLimitBreak; }
	_bool IsCanUse_LimitBreak() { return m_fLimitBreak.IsMax(); }

	//���� // Limit ���� ���� ���Ƿ� ����
	void	Init_LimitBreak(_float fMaxLimitBreak) { m_fLimitBreak.Readjust(fMaxLimitBreak);}
	_bool	Add_LimitBreak(_float _AddLP) { return m_fLimitBreak.Increase(_AddLP); }
	_float  Get_CurLimitBreak() const { return m_fLimitBreak.fCur; }
	_float  Get_MaxLimitBreak() const { return m_fLimitBreak.fMax; }


protected:
	FGauge			m_fHP;			// ü�� ��ġ
	FGauge			m_fMP;			// ���� ���� ��
	FGauge			m_fATB;			// 2�� �ִ�ġ
	FGauge			m_fLimitBreak;	// �ʻ�� ������

public:
	// ���̺� �ε� ����, ���̺��� ������ ã�� ������ �������ش�.
	void Init_StatsByTable(ETableType eType, const string& strName);
	// ���� ����
	void Init_Stats(_int iLV, _int iAttack, _int iMagic, _int iDefence, _int iMagicDefence);
	// ���� ����, �׷��� ĳ������ �����͸� �Ŵ����� ���ε��ϰ� �� �� ����� �Լ�. (�غ���)
	void Init_Stats(TCharacterSchema tSchema);
	// ������� �ൿ�� ���� ���ݷ��� �����ش�.
	void Calculate_ActionPower(const string& strSkillName, _int& iDamage, _int& iDamageType, _int& iPenetration, _int& iBurst, _uint& iHitPower, _uint& iOutAddHitPower) const;

#pragma region ��������
protected:
	_int			m_iLV = { 1 };
	_int			m_iAttack = { 0 };
	_int			m_iMagic = { 0 };
	_int			m_iDefence = { 0 };
	_int			m_iMagicDefence = { 0 };
	_int			m_iCriticalRate = { 25 };
#pragma endregion


#pragma region ��ġ���� (AI��)
public:
	// �ൿ ���� ������Ʈ, �ʿ��� ��� ���º��� �������־�� �Ѵ�. (��ų ������ �ƴ�)
	// ���¸� ó�� �� �� ����Ѵ�.
	void Update_ActionPower(_int iPower) { m_iActionPower = iPower; }
	// �ൿ ������ ���ϴ� ������Ʈ, �ൿ�� ������ ���� �����ϴ� 
	void Update_ActionPowerAdd(_int iPower) { m_iActionPower += iPower; }
	// �ൿ�� ���� ������ �����Ѵ�.
	void Reset_ActionPower() { m_iActionPower = 0; }
	// �ൿ�� ���� �񱳸� �Ѵ�.
	_bool IsActionPowerEqGreater(_int iCompare) const { return m_iActionPower >= iCompare; }
	_bool IsActionPowerEqLess(_int iCompare) const { return m_iActionPower <= iCompare; }
	_bool IsActionPowerEqual(_int iCompare) const { return m_iActionPower == iCompare; }
	_int Get_ActionPower() const { return m_iActionPower; }

	// ���¸� ó�� �� �� ����Ѵ�.
	void Update_ActionGap(_int iGap) { m_iActionGap = iGap; }
	// �ൿ ������ ���ϴ� ������Ʈ, �ൿ�� ��ƴ ����
	void Update_ActionGapAdd(_int iGap) { m_iActionGap += iGap; }
	// �ൿ�� ���� ������ �����Ѵ�.
	void Reset_ActionGap() { m_iActionGap = 0; }
	// �ൿ�� ���� �񱳸� �Ѵ�.
	_bool IsActionGapEqGreater(_int iCompare) const { return m_iActionGap >= iCompare; }
	_bool IsActionGapEqLess(_int iCompare) const { return m_iActionGap <= iCompare; }
	_int Get_ActionGap() const { return m_iActionGap; }

	void Reset_AllActionValue() { m_iActionPower = 0; m_iActionGap = 0; }
	
	// ������ ��ġ (��ü���� �ɷ�ġ�� �����)
	_int Calculate_TotalPower() const;
	// ���������� ��ġ
	_int Calculate_PowerGap(const CStatusComp& OtherStatusComp) const;
protected:
	_int m_iActionPower = { 0 };
	_int m_iActionGap = { 0 };
#pragma endregion



#pragma region ���°���
public:
	GETSET_EX2(_bool, m_bIsGuard, Guard, GET_C_REF, SET_C_REF)
	GETSET_EX2(_bool, m_bIsInvincible, IsInvincible, GET_C_REF, SET_C_REF)

protected:
	FGauge	m_fBurstGauge = { 1000.f };
	FGauge	m_fBurstTime = { 10.f, true };
	_bool	m_bIsGuard = { false };
	_bool	m_bIsInvincible = { false };
#pragma endregion


private:
	mutable _float	m_fLastRandomDamage = { 1.f };
	// ���������� ���� �����. (UI)
	mutable _int	m_iLastDamage = { 0 };

};

END