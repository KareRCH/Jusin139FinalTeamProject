#pragma once

#include "Base.h"

#include "MeshContainer.h"
#include "BoneContainer.h"
#include "BoneAnimContainer.h"
#include "MaterialContainer.h"

BEGIN(Engine)

/// <summary>
/// � �𵨿� ���� �׷�
/// </summary>
class ENGINE_DLL FModelData final : public CBase
{
	DERIVED_CLASS(CBase, FModelData)
public:
	enum TYPE { TYPE_MODEL, TYPE_ANIM_MODEL };

private:
	explicit FModelData() = default;
	explicit FModelData(const FModelData& rhs) = delete;
	virtual ~FModelData() = default;

public:
	HRESULT Initialize();

public:
	static FModelData* Create(const _bool bLoaded);

private:
	virtual void Free() override;

public:
	FMeshData*		Find_Mesh(const _uint iIndex);
	FMeshData*		Find_Mesh(const wstring& strMesh, const _uint iRangeIndex);

	FMaterialData*	Find_Material(const _uint iIndex);
	FMaterialData*	Find_Material(const wstring& strMaterial);

	CBoneGroup*		Find_BoneGroup();

	FBoneAnimData*	Find_BoneAnim(const _uint iIndex);
	FBoneAnimData*	Find_BoneAnim(const wstring& strBoneAnim);

public:
	_uint_64				iHashID = { 0 };					// �ؽ� ���̵�, ������ �Ŵ������� �ؽ�����ҿ� ����
	TYPE					eModelType = { TYPE_MODEL };
	class CMeshGroup*		pMeshGroup = { nullptr };			// �޽��� ��Ƴ��� �׷�
	class CMaterialGroup*	pMaterialGroup = { nullptr };		// ���� ���� �׷�
	class CBoneGroup*		pBoneGroup = { nullptr };			// �� ���� �׷�
	class CBoneAnimGroup*	pAnimGroup = { nullptr };			// �ִϸ��̼� �׷�


public:
	HRESULT Create_VTF_Texture();
	HRESULT Bind_ShaderResources(class CShader* pShader);
	HRESULT Bind_VTFBuffers();
	HRESULT Render_ToVTF(class CShader* pShader);

private:
	ComPtr<ID3D11Texture2D>				m_pVertexTexture = { nullptr };	// VTF�� �ؽ�ó
	ComPtr<ID3D11ShaderResourceView>	m_pVT_SRV = { nullptr };		// VTF ���̴� ���ҽ�
	ComPtr<ID3D11RenderTargetView>		m_pVT_RTV = { nullptr };		// VTF ����Ÿ��

	_uint								m_iStride = { 0 };
	D3D11_PRIMITIVE_TOPOLOGY			m_eVTF_Topology = { D3D11_PRIMITIVE_TOPOLOGY_POINTLIST };	// VTF�� �̰� ����
	
};

END