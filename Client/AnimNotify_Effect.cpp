#include "stdafx.h"
#include "AnimNotify_Effect.h"
#include "GameInstance.h"
#include "Level.h"
#include "Effect.h"

#include "Effect_Group.h"

#include "Effect_Manager.h"

CAnimNotify_Effect::CAnimNotify_Effect()
{
}

CAnimNotify_Effect::CAnimNotify_Effect(const CAnimNotify_Effect& rhs)
	: BASE(rhs)
{
	if (m_pNotifyData != nullptr)
	{
		auto pClone = new FNotifyType_Effect;
		(*pClone) = (*Cast<FNotifyType_Effect*>(m_pNotifyData));
		m_pNotifyData = pClone;
	}
}

HRESULT CAnimNotify_Effect::Initialize(_float fTrackPos, FNotifyType_Effect Desc)
{
	m_pNotifyData = new FNotifyType_Effect;
	FNotifyType_Effect& NotiData = (*Cast<FNotifyType_Effect*>(m_pNotifyData));
	NotiData.iEffectType = Desc.iEffectType;
	NotiData.strEffectName = Desc.strEffectName;
	NotiData.vPos = Desc.vPos;
	NotiData.vRot = Desc.vRot;

	__super::Initialize(fTrackPos, Desc.strName);

	return S_OK;
}

HRESULT CAnimNotify_Effect::Initialize(FRapidJson& InputData)
{
	return Input_RpJson(InputData);
}

void CAnimNotify_Effect::OnReset()
{
	__super::OnReset();

}

_bool CAnimNotify_Effect::OnTrigger()
{
	if (!__super::OnTrigger())
		return false;

	FNotifyType_Effect& Data = (*ReCast<FNotifyType_Effect*>(m_pNotifyData));
	if (&Data == nullptr)
		return false;
	
	shared_ptr<CEffect> pEffect = { nullptr };

	if (m_pGI->Get_CreateLevelIndex() == LEVEL_TOOL)
	{
		if (FAILED(m_pGI->Add_CloneObject(m_pGI->Get_CreateLevelIndex(), L_EFFECT, Data.strEffectName, nullptr, &pEffect)))
		{
			assert(true);
			return false;
		}

		//pEffect->Set_ApplyTransform(true); // ������ ������Ʈ�� Ʈ������ ��� ���
	}
	else 
	{

#if EFFECT_LOAD != 0

		return true;
#endif

		// ���⿡ ����Ʈ�� �����ϴ� �ڵ�
		pEffect = GET_SINGLE(CObjPool_Manager)->Create_Object<CEffect>(Data.strEffectName, L_EFFECT, nullptr, nullptr);

		if (pEffect == nullptr)
			return false;
	}

	if (!m_pCollection->Get_ModelComp().expired())
	{
		pEffect->Set_Owner(m_pCollection->Get_ModelComp().lock()->Get_Owner().lock());
		pEffect->Set_OwnerTransformCom(pEffect->Get_Owner().lock()->Get_TransformCom().lock());
		// ���� ����

		_matrix	matWorldBackUp = pEffect->Get_TransformCom().lock()->Get_WorldMatrix();
		// ����� ���

		pEffect->Get_TransformCom().lock()->Set_Position(1.f, XMVectorSetW(Data.vPos, 1.f));
		// ��ġ ����
		pEffect->Get_TransformCom().lock()->Rotation_Quaternion(Data.vRot);
		// ȸ���� ����
		
		_matrix matOnwerWorld = Get_MatrixNormalize(Get_MatrixNormalize(pEffect->Get_OwnerTransformCom().lock()->Get_WorldFloat4x4()));

		pEffect->Set_matWorld(pEffect->Get_TransformCom().lock()->Get_WorldMatrix() *
			matOnwerWorld);
		// m_matWorld -> �ʱ� ����
		// Ʈ�������� ��������� �����ϸ� �ȵȴ�.

		pEffect->Set_OneTImeWorld(Get_MatrixNormalize(matOnwerWorld));
		// �ܹ߼��� ������ �� �������� ����� �ʿ�
	}

	return true;
}

shared_ptr<CAnimNotify_Effect> CAnimNotify_Effect::Create(_float fTrackPos, FNotifyType_Effect Desc)
{
	DERIVED* pInstance = new DERIVED();

	if (FAILED(pInstance->Initialize(fTrackPos, Desc)))
	{
		MSG_BOX("AnimNotify_Effect : Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared(pInstance);
}

shared_ptr<CAnimNotify_Effect> CAnimNotify_Effect::Create(FRapidJson& InputData)
{
	DERIVED* pInstance = new DERIVED();

	if (FAILED(pInstance->Initialize(InputData)))
	{
		MSG_BOX("AnimNotify_Effect : Failed");
		Safe_Release(pInstance);
	}

	return BaseMakeShared(pInstance);
}

void CAnimNotify_Effect::Free()
{
	__super::Free();

}

shared_ptr<CAnimNotify> CAnimNotify_Effect::Clone()
{
	DERIVED* pInstance = new DERIVED(*this);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("AnimNotify_Effect : Failed");
		Safe_Release(pInstance);
	}

	return DynPtrCast<CAnimNotify>(BaseMakeShared(pInstance));
}

FRapidJson CAnimNotify_Effect::Bake_Notify()
{
	FRapidJson Data = __super::Bake_Notify();

	FNotifyType_Effect& NotiDat = (*Cast<FNotifyType_Effect*>(m_pNotifyData));
	if (&NotiDat == nullptr)
	{
		cerr << "�Ľ� ����" << endl;
		return FRapidJson();
	}

	Data.Write_Data("NotifyID", 0);
	_int iTemp = (_int)NotiDat.iEffectType;
	Data.Write_Data("EffectType", iTemp);
	Data.Write_StringData("EffectName", ConvertToString(NotiDat.strEffectName));
	Data.Write_StringData("SocketName", NotiDat.strSocketName);
	Data.Write_Data("Pos", NotiDat.vPos);
	Data.Write_Data("Rot", NotiDat.vRot);

	return Data;
}

HRESULT CAnimNotify_Effect::Input_RpJson(FRapidJson InputData)
{
	if (nullptr == m_pNotifyData)
		m_pNotifyData = new FNotifyType_Effect;


	FNotifyType_Effect& NotiDat = (*Cast<FNotifyType_Effect*>(m_pNotifyData));
	//NotiDat.Event = InputData.Event;
	_int iTemp = 0;
	InputData.Read_Data("NotifyName", m_strName);
	InputData.Read_Data("EffectType", iTemp);
	NotiDat.iEffectType = (EFFECT_TYPE)iTemp;
	InputData.Read_Data("EffectName", NotiDat.strEffectName);
	InputData.Read_Data("SocketName", NotiDat.strSocketName);
	InputData.Read_Data("Pos", NotiDat.vPos);
	InputData.Read_Data("Rot", NotiDat.vRot);
	InputData.Read_Data("TrackPos", m_fTrackPos);

	return S_OK;
}
