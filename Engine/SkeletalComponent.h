#pragma once

#include "Component.h"

BEGIN(Engine)

enum class EModelGroupIndex : _uint;
class CBoneGroup;

/// <summary>
/// ���̷����� ������ �� �ִ� ������Ʈ
/// �ϳ��� �ν��Ͻ�ȭ �� ���̷��� �׷��� ������.
/// �� ������Ʈ�� �޽� ������Ʈ�� ��ǰ���� ��޵ȴ�.
/// ���� �����Ͽ� ����Ѵ�.
/// </summary>
class ENGINE_DLL CSkeletalComponent final : public CComponent
{
	DERIVED_CLASS(CComponent, CSkeletalComponent)
	
protected:
	explicit CSkeletalComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CSkeletalComponent(const CSkeletalComponent& rhs);
	virtual ~CSkeletalComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* Arg = nullptr) override;
	
public:
	static shared_ptr<CSkeletalComponent> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr) override;

protected:
	virtual void	Free() override;

public:
	// ���� �ε��Ѵ�.
	HRESULT		Load_Skeletal(const wstring& strModelFilePath);

	// ���� Ʈ�������� �ּҸ� ������ ���͸� ������, ���ۿ� ���� �� �� �ֵ��� ���ش�.
	CBoneGroup* Get_BoneGroup();
	void		Invalidate_BoneTransforms();
	_float4x4	Get_BoneTransformFloat4x4(_uint iIndex);
	_matrix		Get_BoneTransformMatrix(_uint iIndex);

	_matrix Get_BoneTransformMatrix(const wstring& strBoneName);

private:	// �� ����
	class CBoneGroup* m_pBoneGroup = { nullptr };		// ��� ���� ���� ������ ������ �ִ� ��ü
	
};

END