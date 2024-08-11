#include "ModelContainer.h"
#include "GameInstance.h"
#include "Shader.h"

HRESULT FModelData::Initialize()
{
	pMeshGroup = CMeshGroup::Create();
	pBoneGroup = CBoneGroup::Create();
	pAnimGroup = CBoneAnimGroup::Create();
	pMaterialGroup = CMaterialGroup::Create();

	return S_OK;
}

FModelData* FModelData::Create(const _bool bLoaded)
{
	ThisClass* pInstance = new ThisClass();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("FModelData Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void FModelData::Free()
{
	Safe_Release(pMeshGroup);
	Safe_Release(pMaterialGroup);
	Safe_Release(pBoneGroup);
	Safe_Release(pAnimGroup);
}

FMeshData* FModelData::Find_Mesh(const _uint iIndex)
{
	if (nullptr == pMeshGroup)
		return nullptr;

	return pMeshGroup->Find_Mesh(iIndex);
}

FMeshData* FModelData::Find_Mesh(const wstring& strMesh, const _uint iRangeIndex)
{
	if (nullptr == pMeshGroup)
		return nullptr;

	return pMeshGroup->Find_Mesh(strMesh, iRangeIndex);
}

FMaterialData* FModelData::Find_Material(const _uint iIndex)
{
	if (nullptr == pMaterialGroup)
		return nullptr;

	return pMaterialGroup->Find_Material(iIndex);
}

FMaterialData* FModelData::Find_Material(const wstring& strMaterial)
{
	if (nullptr == pMaterialGroup)
		return nullptr;

	return pMaterialGroup->Find_Material(strMaterial);
}

CBoneGroup* FModelData::Find_BoneGroup()
{
	if (nullptr == pBoneGroup)
		return nullptr;

	return pBoneGroup;
}


FBoneAnimData* FModelData::Find_BoneAnim(const _uint iIndex)
{
	if (nullptr == pAnimGroup)
		return nullptr;

	return pAnimGroup->Find_BoneAnim(iIndex);
}

FBoneAnimData* FModelData::Find_BoneAnim(const wstring& strBoneAnim)
{
	if (nullptr == pAnimGroup)
		return nullptr;

	return pAnimGroup->Find_BoneAnim(strBoneAnim);
}

HRESULT FModelData::Create_VTF_Texture()
{
	// �ؽ���
	// ��ġ + ��� + ź��Ʈ + �ؽ���� = 44 * ���� ����(Ŭ����� 110000��) = 4,840,000 BYTE��
	// ������ ��� 64 * ������ (560���ϱ� 64 * 560), ��� �Ѱ��� �� ������ ���� �������� �Ѱ�µ�
	// �� ����� �ε��� ã�µ� ������ ����Ű�� ������ ����� �� ���� �����. (������ ������ �����Ե�
	// ���� ������ �̻��ϰ� ��µǱ�� �߾���. ������ ��ü �� �ε����� ���ؼ� �Ѱ��־�� �Ѵ�.
	// �ִϸ��̼� ���� ���, �Ȱ��� ������ ���� (�Ȱ��� 560 * 64)...�̰� Validate ��
	// 

	// ���� -> �޽� ������ ��� ����, �ν��Ͻ�(����, )

	// ���� ���� sizeof(VTX_VTFANIMMODEL)(44) * �������� = 4840000
	// 4840000 / sizeof(FLOAT4) = �ʿ� �ȼ�����(302,500)
	// 4096�̸� ��� 73.8, �� 74���� �ʿ���. ������ �е����� ����ؾ���.
	// 4096 ����� �������� �ص���. ������ ���ٴ� 20����Ʈ �е��ؾ���. 1489���� ���� ��.
	// 11000 / 1489
	// �� ����� ���ٴ� 1,024���� ����.
	// �� �ټ� = ceil(������ / 4) = 560 / 1,024 = �ø��ϸ� 1�� * �޽����� = 1 * 12������ 12�� ��¾��.
	// ���� ������ �� ����
	// ������ ��ĵ��� ���ϸ� �ȵȴ�. ��, ���� �տ� �δ°� ���ڴ�.
	// �ִϸ��̼� �� ���, ���� ����. ��, �ν��Ͻ� ������ŭ ���̸� ���δ�. ��, 1�� * �ν��Ͻ� ����
	// (1�� + 74��) * �ν��Ͻ� ������ �ȴ�. �� �κ��� ����ȭ�� �Ǿ�� �ν��Ͻ��� �ǹ̰� ���� ��.
	// 4096 * 4096 �ؽ�ó�� 268�ް� ���� �Դ´�.
	// 4,084�� / 75�� = 54 �ν��Ͻ� ��ü�� �ִ°� ����...�ٵ� �̰� �׷��� ������ ��������?
	// Ű������ ����� CPU�� ����.

	//struct TVertexTexture
	//{
	//	_float3 vPos;
	//	_float3 vNormal;
	//	_float2 vTexcoord;
	//	_float3 vTangent;
	//};

	//// �ʱ�ȭ �ڵ� (����)
	//D3D11_SUBRESOURCE_DATA InitData;
	//InitData.pSysMem = { nullptr };
	//InitData.SysMemPitch = 10;
	//InitData.SysMemSlicePitch = 10;

	//// �޽��� 
	//size_t iInitDataIndex = 0;
	//_uint iRowStride = 4096 * sizeof(_float4);
	//_uint iNumMeshes = pMeshGroup->Get_NumMeshes();
	//for (_uint i = 0; i < iNumMeshes; i++)
	//{
	//	FMeshData* pMeshData = pMeshGroup->Find_Mesh(i);

	//	if (pMeshData->pVB != nullptr)
	//	{
	//		void* pVertexData = { nullptr };
	//		if (eModelType == TYPE_MODEL)
	//			pVertexData = new VTXMESH[pMeshData->iNumVertices];
	//		else
	//			pVertexData = new VTXMESHANIM[pMeshData->iNumVertices];

	//		if (pVertexData == nullptr)
	//			RETURN_EFAIL;
	//		D3D11_PRIMITIVE_
	//		// ���ۿ��� ���� �о�ͼ� �ؿ��� �۾��ϵ��� �Ѵ�.
	//		D3D11_MAPPED_SUBRESOURCE SubResource;
	//		if (SUCCEEDED(GI()->
	//			Get_D3D11Context()->Map(pMeshData->pVB.Get(), 0, D3D11_MAP_READ, 0, &SubResource)))
	//		{
	//			if (eModelType == TYPE_MODEL)
	//				memcpy(pVertexData, SubResource.pData, sizeof(VTXMESH) * pMeshData->iNumVertices);
	//			else
	//				memcpy(pVertexData, SubResource.pData, sizeof(VTXMESHANIM) * pMeshData->iNumVertices);

	//			GI()->Get_D3D11Context()->Unmap(pMeshData->pVB.Get(), 0);
	//		}
	//		
	//		// ���� �е���
	//		_uint iPadding = (4096 * (_uint)sizeof(_float4)) % sizeof(TVertexTexture);	// 20
	//		// �ؽ�ó�� ����־��ֱ� ���� memcpy, �е��� �־��ָ鼭 �ݺ��� �������Ѵ�.
	//		TVertexTexture* szVertices = new TVertexTexture[pMeshData->iNumVertices];
	//		_uint iCounting = 0;
	//		for (_uint i = 0; i < pMeshData->iNumVertices; i++)
	//		{
	//			for (_uint j = 0; j < pMeshData->iNumV; j++)
	//			{

	//			}
	//			if ((4096 * (_uint)sizeof(_float4)) / (_float)sizeof(TVertexTexture) < )
	//			{
	//				++iCounting;
	//				continue;
	//			}
	//			_uint iIndex = i * sizeof(TVertexTexture) + iCounting * iPadding;
	//			memcpy(&((_char*)szVertices)[i], &((VTXMESHANIM*)pVertexData)[i], sizeof(TVertexTexture));
	//		}
	//	}
	//}
	//

	//D3D11_TEXTURE2D_DESC Desc = {};
	//Desc.Width = 4096;
	//Desc.Height = 128;
	//Desc.MipLevels = 1;
	//Desc.ArraySize = 1;
	//Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//
	//Desc.SampleDesc.Quality = 0;
	//Desc.SampleDesc.Count = 1;

	//Desc.Usage = D3D11_USAGE_DYNAMIC;
	//Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	//Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//Desc.MiscFlags = 0;

	//if (FAILED(GI()->Get_D3D11Device()->CreateTexture2D(&Desc, &InitData, m_pVertexTexture.GetAddressOf())))
	//	RETURN_EFAIL;

	//// ���̴� ���ҽ�
	//D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	//SRVDesc.Format = Desc.Format;
	//SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//SRVDesc.Texture2D.MipLevels = 1;

	//if (FAILED(GI()->Get_D3D11Device()->CreateShaderResourceView(m_pVertexTexture.Get(), &SRVDesc, m_pVT_SRV.GetAddressOf())))
	//	RETURN_EFAIL;

	//// ���� Ÿ��
	//D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
	//RTVDesc.Format = Desc.Format;
	//RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//RTVDesc.Texture2D.MipSlice = 1;

	//if (FAILED(GI()->Get_D3D11Device()->CreateRenderTargetView(m_pVertexTexture.Get(), &RTVDesc, m_pVT_RTV.GetAddressOf())))
	//	RETURN_EFAIL;

	return S_OK;
}

HRESULT FModelData::Bind_ShaderResources(CShader* pShader)
{
	/*if (pShader == nullptr)
		RETURN_EFAIL;

	if (FAILED(pShader->Bind_SRV("g_VertexTexture", m_pVT_SRV.Get())))
		RETURN_EFAIL;*/

	return S_OK;
}

HRESULT FModelData::Bind_VTFBuffers()
{
	//if (m_pVT_SRV == nullptr || m_pVT_RTV == nullptr)
	//	RETURN_EFAIL;

	//auto pContext = GI()->Get_D3D11Context().Get();

	//for(_uint i = 0; i < pMeshGroup->Get_NumMeshes(); ++i)
	//{
	//	auto pMesh = pMeshGroup->Find_Mesh(i);
	//	ID3D11Buffer* pVBs[] = {
	//		pMesh->pVB.Get()
	//	};

	//	_uint		iStrides[] = {
	//		pMesh->iVertexStride
	//	};

	//	_uint		iOffsets[] = {
	//		0,
	//	};

	//	// ���ؽ� ���� �̿�
	//	pContext->IASetVertexBuffers(0, 1, pVBs, iStrides, iOffsets);

	//	// �� ��������
	//	pContext->IASetPrimitiveTopology(m_eVTF_Topology);

	//	// �ٷ� �׸���
	//	pContext->Draw(0, 0);
	//}

	return S_OK;
}

HRESULT FModelData::Render_ToVTF(CShader* pShader)
{
	/*if (FAILED(Bind_ShaderResources(pShader)))
		RETURN_EFAIL;

	if (FAILED(pShader->Begin(0)))
		RETURN_EFAIL;

	if (FAILED(Bind_VTFBuffers()))
		RETURN_EFAIL;*/

	return S_OK;
}

