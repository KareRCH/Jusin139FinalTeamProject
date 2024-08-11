#include "Shader.h"
#include "Texture.h"

#include "GameInstance.h"
#include "Shader_Manager.h"

vector<_uint_64>				CShader::g_UsingRenderIDs;
vector<weak_ptr<CShader>>		CShader::g_ShaderComps;

IMPLEMENT_CREATE(CShader)
IMPLEMENT_CREATE_EX3(CShader, const wstring&, strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC*, pElements, _uint, iNumElements)
IMPLEMENT_CLONE(CShader, CComponent)

CShader::CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffectData(rhs.m_pEffectData)
	, m_pShaderManager(rhs.m_pShaderManager)
	, m_iRenderFlag(rhs.m_iRenderFlag)
{
	Safe_AddRef(m_pShaderManager);
}

HRESULT CShader::Initialize_Prototype()
{
	if (FAILED(Link_ToShaderManager()))
		RETURN_EFAIL;

	return S_OK;
}

/* ���̴������� �����Ͽ� ID3DX11Effect�� �����´�. */
/* ID3DX11Effect�κ��� Pass�� ���� �����Ͽ� ���ڵ� ���� ���������� �� ���̴� �н����� �� �޾��� ���ִ����� ���� InputLayout�� �������. */
HRESULT CShader::Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	Link_Effect(strShaderFilePath, pElements, iNumElements);
//;	if (FAILED(Link_ToShaderManager()))
//		RETURN_EFAIL;
//
//	_uint		iHlslFlag = 0;
//
//#ifdef _DEBUG
//	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
//#else
//	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
//#endif
//
//	/*strShaderFilePath��ο� �ۼ��Ǿ��ִ� hlsl��� ���� �����Ͽ� ID3DX11Effect��� �༮�� ������.  */
//	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice.Get(), &m_pEffect, nullptr)))
//		RETURN_EFAIL;
//
//	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
//	if (nullptr == pTechnique)
//		RETURN_EFAIL;
//
//	pTechnique->GetDesc(&m_TechniqueDesc);
//
//	for (uint32_t i = 0; i < m_TechniqueDesc.Passes; i++)
//	{
//		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
//
//		D3DX11_PASS_DESC		PassDesc;
//		pPass->GetDesc(&PassDesc);
//
//		/* InputLayout : ���� �׸������� ����ϴ� ������ �Է�����.  */
//		/* dx11������ ������� ���������������ο� ���� ����� �����Ǿ���. */
//		/* �츮�� ���� ������ ������������ �����ؾ��Ѵ�.(��������� ����������������) -> Shader */
//		/* �׷��� �츮���� �ݵ�� ���̴��� �ʿ��ϴ�. */
//		/* �츮�� �� �������� �׸������ؼ��� ���̴��� �ʿ��ϰ�, �� ���̴��� �ݵ�� ���� �׸������ϴ� ������ �޾��� �� �־���Ѵ�. */
//		/* ���� �׸������ϴ� ������ ����Ϸ����ϴ� ���̴��� �Է��� ��������?�� ���� üũ�� ������ �̸� ó���ϰ�.
//		�����ϴٸ� dx11�� InputLayout�̶� ��ü�� ������ش�. */
//		ID3D11InputLayout* pInputLayout = nullptr;
//
//		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
//			RETURN_EFAIL;
//
//		m_InputLayouts.push_back(pInputLayout);
//	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	// Ŭ���� ���� ID�� �ο��Ѵ�.
	//Allocate_RenderID();

	return S_OK;
}

HRESULT CShader::Link_Effect(const wstring& strEffectFileName, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const D3D_SHADER_MACRO* pShaderMacro)
{
	// ���̴� �Ŵ����� �������� ����, �׻� ���������� �ٷ�
	if (FAILED(Link_ToShaderManager()))
		RETURN_EFAIL;

	// �ִ��� ����
	Unlink_Effect();

	// �ִ��Ÿ� ���� �̸����� �˻��ؼ� ã�´�.
	shared_ptr<FEffectData> pEffectData = m_pShaderManager->Find_EffectData(strEffectFileName, pShaderMacro);

	if (!pEffectData)
	{
		// ������ �ε� ��Ű��, �׷��� �����ϸ� ���� �� ã�°���.
		if (FAILED(m_pShaderManager->Load_Effect(strEffectFileName, pElements, iNumElements, pShaderMacro)))
			RETURN_EFAIL;

		// �ε强���� �ٽ� ã�ƿ´�.
		pEffectData = m_pShaderManager->Find_EffectData(strEffectFileName, pShaderMacro);
	}
	m_pEffectData = pEffectData;

	return S_OK;
}

HRESULT CShader::Unlink_Effect()
{
	m_pEffectData.reset();

	return S_OK;
}

HRESULT CShader::IsRender_Ready()
{
	if (nullptr == m_pEffectData->pEffect)
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CShader::Link_ToShaderManager()
{
	if (m_pShaderManager)
		return S_OK;

	if (nullptr == m_pGameInstance
		|| nullptr == (m_pShaderManager = m_pGameInstance->Get_ShaderManager()))
		RETURN_EFAIL;

	Safe_AddRef(m_pShaderManager);

	return S_OK;
}

shared_ptr<CShader> CShader::Find_ShaderCompByID(_uint iID)
{
	return g_ShaderComps[iID].lock();
}

_uint CShader::Find_Unused_RenderID(_bool bAllocate)
{
	// �տ������� ���������� ����ִ� ������ ã�´�. O(n / 64)
	_bool bFound = false;
	_uint iFoundIndex = { 0 };
	for (_uint i = 0; i < Cast<_uint>(g_UsingRenderIDs.size()); i++)
	{
		if (g_UsingRenderIDs[i] != UINT64_MAX)
		{
			bFound = true;
			iFoundIndex = i;
			break;
		}
	}
	// ����ִ� ���� ������ ���� �Ҵ���.
	if (!bFound)
	{
		g_UsingRenderIDs.push_back(0ULL);
		iFoundIndex = Cast<_uint>(g_UsingRenderIDs.size() - 1);
	}

	// �ε��� * 64 + ����ִ� ��Ʈ = ���� �Ҵ��� ID O(n / 64 + m)
	_uint iBitNum = { 0 };
	for (_uint i = 0; i < 64; i++)
	{
		// �� ��Ʈ�� �˻��Ѵ�.
		if (!(g_UsingRenderIDs[iFoundIndex] & (1ULL << i)))
		{
			iBitNum = i;
			if (bAllocate)
				g_UsingRenderIDs[iFoundIndex] ^= (1ULL << i);
			break;
		}
	}

#ifdef _DEBUG
#if SHADER_COMP_DEBUG
	cout << "RenderID : " << iFoundIndex * 64 + iBitNum << endl;
#endif  
#endif // _DEBUG

	// �ϼ��� ID�� ��ȯ
	return iFoundIndex * 64 + iBitNum;
}

void CShader::Allocate_RenderID()
{
	m_iRenderID = Find_Unused_RenderID(true);
	// ���
	g_ShaderComps.resize(m_iRenderID + 1);
	g_ShaderComps[m_iRenderID] = DynPtrCast<CShader>(shared_from_this());
}

void CShader::Remove_RenderID()
{
	if (m_iRenderID == UINT_MAX)
		return;

	_uint iIndex = m_iRenderID / 64;
	_uint_64 iSubIndex = m_iRenderID - (iIndex * 64);

	g_UsingRenderIDs[iIndex] ^= (1ULL << iSubIndex);

	// �����͵� ����
	g_ShaderComps[m_iRenderID].reset();
}

HRESULT CShader::Begin(_uint iPassIndex, _bool bBindRenderID, const _char* pConstantName, const _char* pFlagConstantName)
{
	// �н� ���������� ���� ID ���ε��� �Ѵٸ� ���ε带 �õ��Ѵ�.
	if (bBindRenderID)
		Bind_RenderID(pConstantName, pFlagConstantName);

	// ������� �н� ���ε�
	if (iPassIndex >= m_pEffectData->tTechniqueDesc.Passes)
		RETURN_EFAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffectData->pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		RETURN_EFAIL;

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(iPassIndex);

	pPass->Apply(0, m_pContext.Get());

	m_pContext->IASetInputLayout(m_pEffectData->pInputLayouts[iPassIndex].Get());

	return S_OK;
}

HRESULT CShader::Bind_RenderID(const _char* pConstantName, const _char* pFlagConstantName)
{
	// ID ���ε�
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	if (FAILED(pVariable->SetRawValue(&m_iRenderID, 0, sizeof(_uint))))
		RETURN_EFAIL;

	// �÷��� ���ε�
	pVariable = m_pEffectData->pEffect->GetVariableByName(pFlagConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	if (FAILED(pVariable->SetRawValue(&m_iRenderFlag, 0, sizeof(_uint))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	/* �� ���̴��� ����Ǿ��ִ� ���������� �ڵ��� ���´�.*/
	//�������� �̸����� ��������
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	//�������� �ڷ���(Matrix)�� �°� ����
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		RETURN_EFAIL;

	//������ ���������� �� ����
	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{	
	/* �� ���̴��� ����Ǿ��ִ� ���������� �ڵ��� ���´�.*/
	//�������� �̸����� ��������
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	//�������� �ڷ���(Matrix)�� �°� ����
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		RETURN_EFAIL;

	//������ ���������� �� ����
	return pMatrixVariable->SetMatrixArray((_float*)pMatrix,0,iNumMatrices);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{	
	/* �� ���̴��� ����Ǿ��ִ� ���������� �ڵ��� ���´�.*/
	//�������� �̸����� ��������
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	//�������� �ڷ���(SRV)�� �°� ����
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		RETURN_EFAIL;

	//������ ���������� �� ����
	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures)
{

	/* �� ���̴��� ����Ǿ��ִ� ���������� �ڵ��� ���´�.*/
	//�������� �̸����� ��������
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	//�������� �ڷ���(SRV)�� �°� ����
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		RETURN_EFAIL;

	//������ ���������� �� ����
	return pSRVariable->SetResourceArray(ppSRV,0,iNumTextures);
}


HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iSize)
{
	ID3DX11EffectVariable* pVariable = m_pEffectData->pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		RETURN_EFAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}



void CShader::Free()
{
	__super::Free();

	Safe_Release(m_pShaderManager);
	//Remove_RenderID();
}
