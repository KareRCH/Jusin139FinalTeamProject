#pragma once

#include "Client_Defines.h"
#include "Bullet.h"


BEGIN(Client)

class CEffect;

/// <summary>
/// 에어리스 템페스트 투사체.
/// 
/// </summary>
class CAerith_RayOfJudge : public CBullet
{
	INFO_CLASS(CAerith_RayOfJudge, CBullet)

private:
	CAerith_RayOfJudge(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	CAerith_RayOfJudge(const CAerith_RayOfJudge& rhs);
	virtual ~CAerith_RayOfJudge() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Begin_Play(_cref_time fTimeDelta) override;
	virtual void	Priority_Tick(_cref_time fTimeDelta) override;
	virtual void	Tick(_cref_time fTimeDelta) override;
	virtual void	Late_Tick(_cref_time fTimeDelta) override;
	virtual void	Before_Render(_cref_time fTimeDelta) override;
	virtual void	End_Play(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components(void* pArg);

public:
	virtual void PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);
	virtual void PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo);

public:
	static shared_ptr<CAerith_RayOfJudge> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

private:
	_bool m_bIsChaseEnd = { false };		// 체이스 종료하면 쭉 직진
	_float3	m_vTargetPos = {};				// 목표 타격지점.
	_float m_fAccChaseValue = { 0.f };		// 추적 각도 크기. 점점 커지도록 만든다.
	_float m_fSpeed = { 3.2f };
	_float m_fCheckPosTime = { 1.f };		// 추적 끄는 시간
	FTimeChecker m_fDamageTick = { 0.2f };	// 대미지 주는 간격

private:
	shared_ptr<CEffect>	m_pEffect = { nullptr };
};

END