#pragma once
#include "Component.h"

#ifdef _DEBUG
#ifndef TEXTURE_COMP_DEBUG
#define TEXTURE_COMP_DEBUG 0
#endif  
#endif // _DEBUG




BEGIN(Engine)

class CTexture_Manager;

/// <summary>
/// �ؽ�ó�� �����ϴ� ������Ʈ.
/// �ؽ�ó�� �ε��� �� �ؽ�ó �Ŵ����� �ִٸ�
/// �ؽ�ó�� ������ش�.
/// ���� �ؽ�ó�� �ҷ����� ��� ��ü�� ���� �ؽ�ó �Ŵ����� ���������� ����
/// ������Ʈ ���ο��� ����� ������ �����̰�, �Ŵ����� ���������� ��� ��Ű�� ������� �� �����̴�.
/// 
/// ���ο� ����صΰ� ���Ǵ� ������Ʈ�̴�.
/// �ܺο��� �ؽ�ó�� ���� ������ ��� �ʹٸ� ������ ��ü���� �Լ��� �����Ͽ� �򵵷� �����Ѵ�.
/// </summary>
class ENGINE_DLL CTextureComponent : public CComponent
{
	DERIVED_CLASS(CComponent, CTextureComponent)

public:

protected:
	explicit CTextureComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CTextureComponent(const CTextureComponent& rhs);
	virtual ~CTextureComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	static shared_ptr<CTextureComponent> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr);

protected:
	virtual void	Free();


public:
	// ���� SRV�� ��� �Լ� [��]
	ID3D11ShaderResourceView*	Get_ShaderResourseView(const _uint iIndex);
	// �ؽ�ó �迭�� �޾� �ش� �迭�� SRV�� �Ѱ��ִ� �Լ� [������]
	HRESULT						Get_ShaderResourceViews(ID3D11ShaderResourceView** Arr, const _uint iNumTextures);

public:
	// �ؽ�ó�� ���ε��Ѵ�. ���ÿ� �ؽ�ó �Ŵ����� ��ϵ� �Ѵ�. [��]
	HRESULT Ready_Texture(const wstring& strFilePath, const _uint iNumTextures = 1, _bool bUseMainPath = true);
	// �ؽ�ó �����ϱ� [��]
	HRESULT Release_Texture();

public:
	// ����Ʈ�� �ؽ�ó ���ε� [��]
	HRESULT Bind_SRVToEffect(class CShader* pShader, const _char* pTextureSymbolName, const _uint iIndex);

private:
	// �ؽ�ó �Ŵ����� �����ϴ� ���� �Լ� [��]
	HRESULT Link_TextureManager();


private:
	// S���ξ�� �ɺ��̶�� ��
	using SShaderResourceViews = vector<ComPtr<ID3D11ShaderResourceView>>;
	CTexture_Manager*		m_pTexture_Manager = { nullptr };	// ���� �ؽ�ó ���ҽ� ������ ���δ�.

	_uint					m_iNumTextures = 0;
	SShaderResourceViews	m_SRVs = { nullptr };		// ���� ������ �Ǵ� �ؽ�ó�� �ּ��̴�.
};

END