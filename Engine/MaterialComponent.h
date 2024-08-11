#pragma once

#include "Component.h"
#include "TextureComponent.h"



BEGIN(Engine)

/// <summary>
/// ���͸����� ������ �� �ִ� ������Ʈ
/// �𵨷κ��� ���͸��� ������ �ε��Ͽ� ������ ���͸����� �ؽ�ó ������ ���⿡ �����Ѵ�.
/// </summary>
class ENGINE_DLL CMaterialComponent final : public CComponent
{
	INFO_CLASS(CMaterialComponent, CComponent)
protected:
	explicit CMaterialComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CMaterialComponent(const CMaterialComponent& rhs);
	virtual ~CMaterialComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	static shared_ptr<CMaterialComponent> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;

protected:
	virtual void	Free() override;

public:
	const static _uint	g_iNumTextures = TEXTURETYPE_MAX;

public:
	// �𵨿� ���ε� �� �ؽ�ó�� �غ��ϴ� �Լ��̴�.
	HRESULT Ready_Materials_ByModel(const wstring& strModelKey, _uint iMesh_MatIndex);

	// Ŀ�������� ���͸��� �ؽ�ó�� ���ε��ϴ� �Լ��̴�. �ؽ�ó���, �ε��Ͽ� �ְԵ� ���͸��� �ε���, �ε��� �ؽ����� ���� ���� �����ϴ�.
	// ����Ʈ ����ڰ� ���� ����
	HRESULT Ready_CustomSingleMaterial(_uint iMatIndex, const wstring& strTexturePath, _uint iNumTextures);

	// �ش� �ε����� �ؽ�ó�κ��� SRV�� ������ �Լ�
	ID3D11ShaderResourceView* Find_SRV(_uint iMatIndex, _uint iTexIndex);
	ID3D11ShaderResourceView* Find_SRV(TEXTURETYPE eIndex, _uint iTexIndex) { return Find_SRV(ECast(eIndex), iTexIndex); }

	// �ؽ�ó�� ���̴��� ���ε�, �ܺηκ��� ���̴��� �޾� ������ �ؽ�ó�� GPU�� ���ε��Ѵ�.
	HRESULT Bind_SRVToShader(class CShader* pShader, const _char* pConstantName, _uint iMatIndex, _uint iTexIndex,_bool bCustom = false);

	// ���� �ؽ��ĸ� ���ε��� �� ���
	HRESULT Bind_SRVToShaders(class CShader* pShader, const _char* pConstantName, _uint iTexNum);

	ID3D11ShaderResourceView* Find_DefaultSRV(_uint iMatIndex);

private:
	array< shared_ptr<class CTextureComponent>, g_iNumTextures>	m_arrTextureComps = {};
	
};

END