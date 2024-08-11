#pragma once

#include "DynamicObject.h"

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CDynamicObject
{
protected:
	CBlendObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBlendObject(const CBlendObject& rhs);
	virtual ~CBlendObject() = default;

public:
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Begin_Play(_cref_time fTimeDelta);
	virtual void Priority_Tick(_cref_time fTimeDelta);
	virtual void Tick(_cref_time fTimeDelta);
	virtual void Late_Tick(_cref_time fTimeDelta);
	virtual void Before_Render(_cref_time fTimeDelta);
	virtual HRESULT Render();

	virtual	void	Reset_Effect(_bool bActivate = false) {}; // ����Ʈ���� ���� ��Ű�� ���.
protected:
	class CTexture* m_pTextureCom = { nullptr };
protected:
	_float			m_fCamDistance = { 0.0f };
	_bool			m_bIsInGroupEffect = { false }; // �׷� ����Ʈ������ �Ǵ��ϱ� ����.


protected:
	_float4x4		m_matWorld;
public:
	GETSET_EX2(_float4x4, m_matWorld, matWorld, GET, SET)
	GETSET_EX2(_bool, m_bIsInGroupEffect,IsInGroupEffect, GET, SET)

protected:
	void Compute_CamDistance();

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END