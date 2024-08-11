#pragma once

#include "Component.h"
#include "Utility/DelegateTemplate.h"

#ifdef _DEBUG
#ifndef SHADER_COMP_DEBUG
#define SHADER_COMP_DEBUG 1
#endif  
#endif // _DEBUG




BEGIN(Engine)
class FEffectData;

class ENGINE_DLL CShader final : public CComponent
{
	INFO_CLASS(CShader,CComponent)

private:
	CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;


public:	// �ܺο� �Լ�
	// ���̴� ������Ʈ �߿� ���ϴ� ������Ʈ�� ã�´�.
	static shared_ptr<CShader> Find_ShaderCompByID(_uint iID);
	// ����Ʈ �ҷ�����
	HRESULT Link_Effect(const wstring & strEffectFileName, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements, const D3D_SHADER_MACRO * pShaderMacro = nullptr);
	// ����Ʈ Ǯ��
	HRESULT Unlink_Effect();
	// �ܺο��� 
	HRESULT	IsRender_Ready();
	// ���� ����
	void Set_Owner(weak_ptr<CBase> pOwner) { m_pBaseOwner = pOwner; }
	// ���� ��ŷ, ���ϴ� Ÿ������. �������� ������� ��ԵǴ� �����͸� ������� CPU������ ó���ϱ� ���� ���δ�.
	template<typename T>
	shared_ptr<T>	Lock_Owner()
	{
		if (m_pBaseOwner.expired())
			return nullptr;

		return DynPtrCast<T>(m_pBaseOwner.lock());
	}

private:

	// ���������� ���̴� ���̴� �Ŵ��� ��ũ �Լ�. �̸� ���� ���������� ���̴��� ����ϰ� �����ϴ� ������ �Ѵ�.
	HRESULT Link_ToShaderManager();
	
	static _uint Find_Unused_RenderID(_bool bAllocate);
	void Allocate_RenderID();
	// �ڽ��� ������ �ִ� ID�� �������� ���ܽ�Ų��.
	void Remove_RenderID();
	

private:
	// ����� ���̴� �Ŵ���
	class CShader_Manager*	m_pShaderManager = { nullptr };
	// CBase�� ���ʷ� ���� �� �ִ�. DynPtrCast�� Ȱ���� �پ��� ������ ����� �� �ֵ��� �Ѵ�.
	weak_ptr<CBase>			m_pBaseOwner;

	// ������� ID
	static vector<_uint_64>				g_UsingRenderIDs;
	// ��ϵ� ���̴� ������Ʈ
	static vector<weak_ptr<CShader>>	g_ShaderComps;
	
	
public:
	// Begin�� ���ÿ� BindRenderID�� �۵���ų �� �ִ�. �ش� �Լ��� �ʿ��� ������ ���������� ������ �� �� �ִ�.
	HRESULT Begin(_uint iPassIndex, _bool bBindRenderID = false, 
		 const _char* pConstantName = "g_iRenderID", const _char* pFlagConstantName = "g_iRenderFlag");
	HRESULT Bind_RenderID(const _char* pConstantName, const _char* pFlagConstantName);
	HRESULT Bind_Matrix(const _char * pConstantName, const _float4x4 * pMatrix);
	HRESULT Bind_Matrices(const _char * pConstantName, const _float4x4 * pMatrix, _uint iNumMatrices);
	HRESULT Bind_SRV(const _char * pConstantName, ID3D11ShaderResourceView * pSRV);
	HRESULT Bind_SRVs(const _char * pConstantName, ID3D11ShaderResourceView * *ppSRV, _uint iNumTextures);
	HRESULT Bind_RawValue(const _char * pConstantName, const void* pData, _uint iSize);

private:
	// ����Ʈ �����͸� �����ϴ� ��ü����, ���� ������ �Ŵ������� �̷�����.
	shared_ptr<FEffectData>		m_pEffectData = { nullptr };
	// ������ ID�� ����ϱ� ���� ���̴� �ĺ���ȣ
	_uint						m_iRenderID = { UINT_MAX };
	// ���̴� �ڵ峻���� �����÷��� ���� ����Ͽ� �پ��� ó���� �� �� �ִ�.
	_uint						m_iRenderFlag = { 0 };

public:
	// �����ε� ����� ������
	static shared_ptr<CShader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	// ���� ����� ������
	static shared_ptr<CShader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;

};

END