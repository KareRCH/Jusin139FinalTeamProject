#pragma once

#include "GameObjectComp.h"
#include "Transform.h"
#include "GameObject.h"

BEGIN(Engine)


/// <summary>
/// ���������� Ʈ������ ������Ʈ�� �����ϴ� Ŭ����
/// �� Ŭ������ ��ӹ����� Ʈ������ �Ӽ��� ���� ������ �ȴ�.
/// </summary>
class ENGINE_DLL CSceneComponent abstract : public CGameObjectComp
{
	DERIVED_CLASS(CGameObjectComp, CSceneComponent)
protected:
	explicit CSceneComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CSceneComponent(const CSceneComponent& rhs);
	virtual ~CSceneComponent() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT Initialize(void* Arg = nullptr) override;
	virtual void	Priority_Tick(const _float& fTimeDelta);
	virtual void	Tick(const _float& fTimeDelta);
	virtual void	Late_Tick(const _float& fTimeDelta);
	virtual HRESULT	Render() PURE;

public:
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr) PURE;

protected:
	virtual void	Free() override;



#pragma region �� ������Ʈ ����
	// �� ������Ʈ�� �������踦 ������. �ڽ��� ���� ���� �� ������, �׿� ���� ����� �����Ѵ�.
public:
	GETSET_EX1(CSceneComponent*, m_pParentSceneComp, ParentSceneComp, GET)

	CSceneComponent*	Get_FirstChildSceneComp();
	CSceneComponent*	Get_ChildSceneComp(_uint iIndex);
	void				Add_ChildSceneComp(CSceneComponent* const pComp);
	_bool				Insert_ChildSceneComp(_uint iIndex, CSceneComponent* const pComp);

protected:
	CSceneComponent*			m_pParentSceneComp = { nullptr };	// �θ� �� ������Ʈ
	vector<CSceneComponent*>	m_vecChildSceneComp;				// �ڽ� �� ������Ʈ  
#pragma endregion



#pragma region Ʈ������
public:
	_matrix Calculate_TransformMatrixFromParent();
	_float4x4 Calculate_TransformFloat4x4FromParent();
	// �ڽ��� ���� �θ��� Ʈ�������� ���
	_matrix Calculate_TransformMatrixFromParentNoSelf();
	_float4x4 Calculate_TransformFloat4x4FromParentNoSelf();

public:
	CTransform& Transform() { return *m_pTransformComp; }

private:
	shared_ptr<CTransform>		m_pTransformComp = { nullptr };

#pragma endregion


};

END