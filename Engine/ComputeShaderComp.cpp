#include "ComputeShaderComp.h"

#include "GameInstance.h"

IMPLEMENT_CREATE(CComputeShaderComp)
IMPLEMENT_CREATE_EX2(CComputeShaderComp, const wstring&, strShaderFilePath, const _char*, pEntryPoint)
IMPLEMENT_CLONE(CComputeShaderComp, CComponent)

CComputeShaderComp::CComputeShaderComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CComputeShaderComp::CComputeShaderComp(const CComputeShaderComp& rhs)
	: CComponent(rhs)
	, m_pShader(rhs.m_pShader)
	, m_pShaderManager(rhs.m_pShaderManager)
{
	Safe_AddRef(m_pShaderManager);
}

HRESULT CComputeShaderComp::Initialize_Prototype()
{
	if (FAILED(Link_ToShaderManager()))
		return E_FAIL;

	return S_OK;
}

HRESULT CComputeShaderComp::Initialize_Prototype(const wstring& strShaderFilePath, const _char* pEntryPoint)
{
	if (FAILED(Link_Shader(strShaderFilePath, pEntryPoint, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CComputeShaderComp::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CComputeShaderComp::Link_Shader(const wstring& strEffectFileName, const _char* pEntryPoint, const D3D_SHADER_MACRO* pShaderMacro)
{
	// ���̴� �Ŵ����� �������� ����, �׻� ���������� �ٷ�
	if (FAILED(Link_ToShaderManager()))
		return E_FAIL;

	// �ִ��� ����
	Unlink_Shader();

	// �ִ��Ÿ� ���� �̸����� �˻��ؼ� ã�´�.
	m_pShader = m_pShaderManager->Find_ShaderBuffer<EShaderType::Compute>(strEffectFileName);
	if (nullptr == m_pShader)
	{
		// ������ �ε� ��Ű��, �׷��� �����ϸ� ���� �� ã�°���.
		if (FAILED(m_pShaderManager->Load_Shader(strEffectFileName, EShaderType::Compute, pEntryPoint)))
			return E_FAIL;

		// �ε强���� �ٽ� ã�ƿ´�.
		m_pShader = m_pShaderManager->Find_ShaderBuffer<EShaderType::Compute>(strEffectFileName);
	}

	return S_OK;
}

HRESULT CComputeShaderComp::Unlink_Shader()
{
	m_pShader.Reset();

	return S_OK;
}

HRESULT CComputeShaderComp::IsShader_Ready()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

HRESULT CComputeShaderComp::Link_ToShaderManager()
{
	if (m_pShaderManager)
		return S_OK;

	if (nullptr == m_pGameInstance
		|| nullptr == (m_pShaderManager = m_pGameInstance->Get_ShaderManager()))
		return E_FAIL;

	Safe_AddRef(m_pShaderManager);

	return S_OK;
}

void CComputeShaderComp::Free()
{
	__super::Free();

	Safe_Release(m_pShaderManager);
}
