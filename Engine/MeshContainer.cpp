#include "MeshContainer.h"

#include "GameInstance.h"
#include "BoneContainer.h"
#include "ModelBinary.h"

HRESULT FMeshData::Initialize(const TMesh& MeshData, TYPE eTypeModel)
{
	iID = MeshData.iID;
	iMaterialIndex = MeshData.iMaterialIndex;

	// ���� ����
	if (eTypeModel == TYPE::TYPE_MODEL)
	{
		if (FAILED(Create_ModelBuffer(MeshData)))
			RETURN_EFAIL;
	}
	else
	{
		if (FAILED(Create_AnimModelBuffer(MeshData)))
			RETURN_EFAIL;
	}


	// �� ������ ����
	vecMeshBoneDatas.resize(MeshData.m_Bones.size());
	for (_uint i = 0; i < MeshData.m_Bones.size(); i++)
	{
		const TMeshBone& tMeshBone = MeshData.m_Bones[i];

		vecMeshBoneDatas[i].iBoneID = tMeshBone.iBoneID;
		vecMeshBoneDatas[i].matOffset = tMeshBone.matOffset;
	}

	

	return S_OK;
}

HRESULT FMeshData::Create_ModelBuffer(const TMesh& MeshData)
{
	iNumVertices = Cast<_uint>(MeshData.vecVertices.size());
	iVertexStride = Cast<_uint>(sizeof(VTXMESH));

	iNumIndices = Cast<_uint>(MeshData.vecIndices.size());
	iIndexStride = (65536 > iNumIndices) ? 2 : 4;
	eIndexFormat = (iIndexStride == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// ����, �ε��� ���� �����
	VTXMESH* vertices = new VTXMESH[iNumVertices];
	if (!vertices)
		RETURN_EFAIL;

	// ���� ���� ����
	for (_uint i = 0; i < iNumVertices; i++)
	{
		vecVertices.push_back(MeshData.vecVertices[i].vPosition);
		vertices[i].vPosition = MeshData.vecVertices[i].vPosition;
		vertices[i].vNormal = MeshData.vecVertices[i].vNormal;
		vertices[i].vTexcoord = { MeshData.vecVertices[i].vTexCoord[0].x, MeshData.vecVertices[i].vTexCoord[0].y };
		vertices[i].vTangent = MeshData.vecVertices[i].vTangent;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VTXMESH) * iNumVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	
	FAILED_CHECK_RETURN(GI()->Get_D3D11Device()->CreateBuffer(&vertexBufferDesc, &vertexData, pVB.GetAddressOf()), E_FAIL);


	// �ε��� ���� ����
	void* indices = nullptr;
	
	if (iIndexStride == 2)
		indices = new _ushort[iNumIndices];
	else
		indices = new _uint[iNumIndices];
		
	if (!indices)
		RETURN_EFAIL;

	if (iIndexStride == 2)
	{
		for (_uint i = 0; i < iNumIndices; i++)
		{
			Cast<_ushort*>(indices)[i] = Cast<_ushort>(MeshData.vecIndices[i]);
			vecIndices.push_back(MeshData.vecIndices[i]);
		}
	}
	else
	{
		vecIndices.reserve(iNumIndices);
		memcpy(indices, MeshData.vecIndices.data(), iIndexStride * iNumIndices);
		vecIndices.assign(Cast<_uint*>(indices), Cast<_uint*>(indices) + iNumIndices);
	}
		

	// ���� �ε��� ������ ����ü�� �����Ѵ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = iIndexStride * iNumIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ�
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	FAILED_CHECK_RETURN(GI()->Get_D3D11Device()->CreateBuffer(&indexBufferDesc, &indexData, pIB.GetAddressOf()), E_FAIL);

	Safe_Delete_Array(vertices);
	Safe_Delete_Array(indices);

	return S_OK;
}

HRESULT FMeshData::Create_AnimModelBuffer(const TMesh& MeshData)
{
	iNumVertices = Cast<_uint>(MeshData.vecVertices.size());
	iVertexStride = Cast<_uint>(sizeof(VTXMESHANIM));
	vecBoneIDs.resize(iNumVertices);
	vecWeights.resize(iNumVertices);

	iNumIndices = Cast<_uint>(MeshData.vecIndices.size());
	iIndexStride = (65536 > iNumIndices) ? 2 : 4;
	eIndexFormat = (iIndexStride == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// ����, �ε��� ���� �����
	VTXMESHANIM* vertices = new VTXMESHANIM[iNumVertices];
	if (!vertices)
		RETURN_EFAIL;

	// ���� ���� ����
	for (_uint i = 0; i < iNumVertices; i++)
	{
		vecVertices.push_back(MeshData.vecVertices[i].vPosition);
		vertices[i].vPosition = MeshData.vecVertices[i].vPosition;
		vertices[i].vNormal = MeshData.vecVertices[i].vNormal;
		vertices[i].vTexcoord = { MeshData.vecVertices[i].vTexCoord[0].x, MeshData.vecVertices[i].vTexCoord[0].y };
		vertices[i].vTangent = MeshData.vecVertices[i].vTangent;

		memcpy_s(&vertices[i].vBlendIndices, sizeof(_int4), MeshData.vecVertices[i].vecBoneID.data(), sizeof(_int4));
		memcpy_s(&vertices[i].vBlendWeights, sizeof(_float4), MeshData.vecVertices[i].vecWeights.data(), sizeof(_float4));

		memcpy_s(&vecBoneIDs[i], sizeof(_int4), MeshData.vecVertices[i].vecBoneID.data(), sizeof(_int4));
		memcpy_s(&vecWeights[i], sizeof(_float4), MeshData.vecVertices[i].vecWeights.data(), sizeof(_float4));
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VTXMESHANIM) * iNumVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	
	FAILED_CHECK_RETURN(GI()->Get_D3D11Device()->CreateBuffer(&vertexBufferDesc, &vertexData, pVB.GetAddressOf()), E_FAIL);





	// �ε��� ���� ����
	void* indices = nullptr;
	
	if (iIndexStride == 2)
		indices = new _ushort[iNumIndices];
	else
		indices = new _uint[iNumIndices];
		
	if (!indices)
		RETURN_EFAIL;

	if (iIndexStride == 2)
	{
		for (_uint i = 0; i < iNumIndices; i++)
		{
			Cast<_ushort*>(indices)[i] = Cast<_ushort>(MeshData.vecIndices[i]);
			vecIndices.push_back(MeshData.vecIndices[i]);
		}
	}
	else
	{
		vecIndices.reserve(iNumIndices);
		memcpy(indices, MeshData.vecIndices.data(), iIndexStride * iNumIndices);
		vecIndices.assign(Cast<_uint*>(indices), Cast<_uint*>(indices) + iNumIndices);
	}
		

	// ���� �ε��� ������ ����ü�� �����Ѵ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = iIndexStride * iNumIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ�
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	FAILED_CHECK_RETURN(GI()->Get_D3D11Device()->CreateBuffer(&indexBufferDesc, &indexData, pIB.GetAddressOf()), E_FAIL);

	Safe_Delete_Array(vertices);
	Safe_Delete_Array(indices);

	return S_OK;
}

FMeshData* FMeshData::Create(const TMesh& MeshData, TYPE eTypeModel)
{
	ThisClass* pInstance = new ThisClass();

	if (FAILED(pInstance->Initialize(MeshData, eTypeModel)))
	{
		MSG_BOX("FMeshData Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void FMeshData::Free()
{

}


//----------AnimGroup---------------------------------------------------------------------------------------------


CMeshGroup* CMeshGroup::Create()
{
	ThisClass* pInstance = new ThisClass();

	if (!pInstance)
	{
		MSG_BOX("CMeshGroup Create Failed");
	}

	return pInstance;
}

void CMeshGroup::Free()
{
	for (auto& Pair : mapMeshDatas)
		Safe_Release(Pair.second);
	mapMeshDatas.clear();
}

FMeshData* const CMeshGroup::Find_Mesh(const _uint iIndex) const
{
	if (iIndex < 0 || iIndex >= vecMeshDatas.size())
		return nullptr;

	return vecMeshDatas[iIndex];
}

FMeshData* const CMeshGroup::Find_Mesh(const wstring& strMeshKey, const _uint iRangeIndex) const
{
	auto range = mapMeshDatas.equal_range(strMeshKey);

	_uint i = 0;
	auto iter = range.first;
	for (; iter != range.second; ++iter)
	{
		if (iRangeIndex == i++)
			break;
	}
	if (iter == range.second)
		return nullptr;


	return (*iter).second;
}

HRESULT CMeshGroup::Add_Mesh(const wstring& strMeshKey, FMeshData* const pMeshData)
{
	if (pMeshData == nullptr)
		RETURN_EFAIL;

	// ��Ƽ�� �����̳�, �޽��� �ߺ��� �Ͼ �� ����
	mapMeshDatas.emplace(strMeshKey, pMeshData);

	// ���� �����̳�
	vecMeshDatas.resize(Cast<size_t>(pMeshData->iID + 1), nullptr);
	vecMeshDatas[pMeshData->iID] = pMeshData;

	++m_iNumMeshes;

	return S_OK;
}

HRESULT CMeshGroup::Create_VTF_OffsetBones(CBoneGroup* pBoneGroup)
{
	// ���� ������� �־�� ��.
	_uint iNumBones = pBoneGroup->CRef_BoneDatas_Count();
	if (iNumBones < 256)
		RETURN_EFAIL;

	

	_int iFactor = sqrt(iNumBones / 1024.f);		// ������ ���
	_int iBoneGroups = pow(2, iFactor);				// �ִ� �ؽ�ó �� �� ���
	m_iNumVTFColums = 4096 / 4;						// �ٴ� ��� ����(��� ����)
	m_iNumVTF_LastRowColums = iNumBones % 1024;		// ������ �࿡ ���� ����

	// ��ü �������� ���� �迭 �����
	vector<vector<_float4x4>> Bones;
	Bones.resize(m_iNumMeshes);
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		Bones[i].resize(iNumBones, XMMatrixIdentity());
	}

	// �޽� ������ŭ �ؽ�ó�� �Ҵ�
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		for (_uint j = 0; j < vecMeshDatas[i]->vecMeshBoneDatas.size(); j++)
		{
			auto& MeshBoneData = vecMeshDatas[i]->vecMeshBoneDatas[j];
			Bones[i][MeshBoneData.iBoneID] = MeshBoneData.matOffset;
		}
	}

	

	D3D11_TEXTURE2D_DESC Desc = {};
	Desc.Width = 4096;
	Desc.Height = m_iNumVTFRows = iBoneGroups;
	Desc.MipLevels = 1;
	Desc.ArraySize = m_iNumMeshes;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	Desc.SampleDesc.Quality = 0;
	Desc.SampleDesc.Count = 1;

	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;
	
	if (FAILED(GI()->Get_D3D11Device()->CreateTexture2D(&Desc, nullptr, m_pVertexTexture.GetAddressOf())))
		RETURN_EFAIL;

	// ���̴� ���ҽ�
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = Desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = 1;

	if (FAILED(GI()->Get_D3D11Device()->CreateShaderResourceView(m_pVertexTexture.Get(), &SRVDesc, m_pVT_SRV.GetAddressOf())))
		RETURN_EFAIL;


	if (SUCCEEDED(GI()->Get_D3D11Context()->Map(m_pVertexTexture.Get(), 0, D3D11_MAP_WRITE, 0, nullptr)))
	{


		GI()->Get_D3D11Context()->Unmap(m_pVertexTexture.Get(), 0);
	}
	else RETURN_EFAIL;


	m_bIsUseVTF = true;

	return S_OK;
}

