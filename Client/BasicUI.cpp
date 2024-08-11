#include "stdafx.h"
#include "BasicUI.h"

#include "GameInstance.h"

CBasicUI::CBasicUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{

}

CBasicUI::CBasicUI(const CBasicUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBasicUI::Initialize_Prototype()
{
	/* ������ü�� �ʱ�ȭ������ �����Ѵ�. */
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/
	//int*		pData_int = new int(3);

	//void**		pData = (void**)&pData_int;

	return S_OK;
}

HRESULT CBasicUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	UI_DESC pDesc = {};

	if(nullptr != pArg)
		pDesc = *(UI_DESC*)pArg;

	m_fX = pDesc.fX;
	m_fY = pDesc.fY;
	m_fSizeX = pDesc.fSizeX;
	m_fSizeY = pDesc.fSizeY;
	m_strTextureTag = pDesc.strTextureTag;
	m_iTextureIndex = pDesc.iTextureIndex;
	
	//���� ������ Scale,Position,ViewMatrix, ProjMatrix ����

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	//Transform ������ ������Ʈ �߰�
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBasicUI::Priority_Tick(_cref_time fTimeDelta)
{

}

void CBasicUI::Tick(_cref_time fTimeDelta)
{
}

void CBasicUI::Late_Tick(_cref_time fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return ;
}

HRESULT CBasicUI::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* �� ���̴��� 0��° �н��� �׸�����. */
	m_pShaderCom->Begin(0);

	/* ���� �׸������ϴ� ����, �ε������۸� ��ġ�� ���ε���. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* ���ε��� ����, �ε����� �׷�. */
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBasicUI::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), & (m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), & (m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), m_strTextureTag,
		TEXT("Com_Texture"), & (m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBasicUI::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioY", &m_fRatioY,sizeof(m_fRatioY))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

CBasicUI * CBasicUI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CBasicUI*		pInstance = new CBasicUI(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : BasicUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBasicUI::Clone(void* pArg)
{
	CBasicUI*		pInstance = new CBasicUI(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BasicUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBasicUI::Free()
{
	__super::Free();
}
