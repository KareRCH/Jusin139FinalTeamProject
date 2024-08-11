#pragma once

#include "Client_Defines.h"
#include "AnimNotify.h"
#include "Utility/RapidJson_Utility.h"


BEGIN(Client)

struct FNotifyType_Sound : public FNotifyType
{
	vector<wstring> strSoundGroupTags;	// ���� �׷�Ű
	vector<wstring> strSoundNameTags;	// ���� �̸�Ű
	_int			iSoundChannel;		// ä��
	_float			fSoundVolume;		// ����
	_float			fRandom;			// ���� ���Ȯ��
	_float3			vOffsetPos;			// 3D ȿ���� �ֱ����� ��ġ��
	// ��Ÿ ���� ������
};

class CAnimNotify_Sound final : public CAnimNotify
{
	INFO_CLASS(CAnimNotify_Sound, CAnimNotify)

public:
	explicit CAnimNotify_Sound();
	explicit CAnimNotify_Sound(const CAnimNotify_Sound& rhs);
	virtual ~CAnimNotify_Sound() = default;

public:
	virtual HRESULT Initialize() override { return S_OK; }
	virtual HRESULT Initialize(_float m_fTrackPos, FNotifyType_Sound Desc);
	virtual HRESULT Initialize(FRapidJson& InputData);

	// Ʈ���Ÿ� �۵��ϱ� �����·� �ǵ�����.
	virtual void OnReset() override;

	// �۵��� �߻���ų ���
	virtual _bool OnTrigger() override;

public:
	static shared_ptr<CAnimNotify_Sound> Create(_float fTrackPos, FNotifyType_Sound Desc);
	static shared_ptr<CAnimNotify_Sound> Create(FRapidJson& InputData);
	virtual void Free() override;
	virtual shared_ptr<CAnimNotify> Clone() override;


public:
	virtual FRapidJson Bake_Notify() override;
	HRESULT Input_RpJson(FRapidJson InputData);

};

END