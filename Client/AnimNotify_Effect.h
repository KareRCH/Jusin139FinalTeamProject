#pragma once

#include "Client_Defines.h"
#include "AnimNotify.h"
#include "Utility/RapidJson_Utility.h"
#include "Utility/DelegateTemplate.h"


BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

//typedef FastDelegate1<weak_ptr<CGameObject>, void> Delegate;
//typedef MulticastDelegate<Delegate, weak_ptr<CGameObject>>	MulticastDelegate;

struct FNotifyType_Effect : public FNotifyType
{
	EFFECT_TYPE	iEffectType;
	wstring		strEffectName;
	string		strSocketName;		// ���� ���� ���� ������ ã�� ���߿� ó���� ���ְ� �ȴ�.
	_float3		vPos;
	_float4		vRot;
};

class CAnimNotify_Effect : public CAnimNotify
{
	INFO_CLASS(CAnimNotify_Effect, CAnimNotify)

public:
	explicit CAnimNotify_Effect();
	explicit CAnimNotify_Effect(const CAnimNotify_Effect& rhs);
	virtual ~CAnimNotify_Effect() = default;

public:
	virtual HRESULT Initialize() override { return S_OK; }
	virtual HRESULT Initialize(_float fTrackPos, FNotifyType_Effect Desc);
	virtual HRESULT Initialize(FRapidJson& InputData);

	// Ʈ���Ÿ� �۵��ϱ� �����·� �ǵ�����.
	virtual void OnReset() override;

	// �۵��� �߻���ų ���
	virtual _bool OnTrigger() override;

public:
	static shared_ptr<CAnimNotify_Effect> Create(_float fTrackPos, FNotifyType_Effect Desc);
	static shared_ptr<CAnimNotify_Effect> Create(FRapidJson& InputData);
	virtual void Free() override;
	virtual shared_ptr<CAnimNotify> Clone() override;

public:
	virtual FRapidJson Bake_Notify() override;
	HRESULT Input_RpJson(FRapidJson InputData);
};

END