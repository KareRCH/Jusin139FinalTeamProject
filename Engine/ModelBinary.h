#pragma once

namespace ModelBinary {}
using namespace ModelBinary;
#include "Base.h"

#include <filesystem>

namespace fs = filesystem;

BEGIN(Engine)


//----------------���


struct TModelHeader
{
	_ushort		iID;			// �ĺ��� ���
	_ushort		iNumMeshes;
	_ushort		iNumMaterials;
	_ushort		iNumBones;
	_ushort		iNumAnims;
	_bool		bIsOnlyAnims;	// �ִϸ��̼� �����͸� �� ������
	_uint_64	iModelHash;		// �𵨰� �ִϸ��̼� ������ ���� �ؽð����� ���δ�.
};



//---------------�޽�

struct TMeshVertex
{
	_float3			vPosition;
	_float3			vTexCoord[8];
	_float3			vNormal;
	_float3			vTangent;
	_float3			vBiTangent;
	vector<_int>	vecBoneID;
	vector<_float>	vecWeights;
};

struct TMeshBone
{
	_int		iBoneID;	// �ش� �� ID
	_float4x4	matOffset;	// ���� ���� ������
};

struct TMesh
{
	_uint				iID;
	wstring				strName;
	vector<TMeshVertex>	vecVertices;
	vector<_uint>		vecIndices;
	vector<TMeshBone>	m_Bones;
	_uint				iMaterialIndex;
};



//-----------------�ִ�



struct TKeyFrame
{
	_float fTrackPosition;
	_float3 vPos;
	_float4 qtRot;
	_float3 vScale;
};

struct TChannel
{
	_uint				iBoneID;
	wstring				strName;
	vector<TKeyFrame>	vecKeyFrames;
};

struct TAnim
{
	_uint				iID;
	wstring				strName;
	_float				fDuration;
	_float				fTickPerSeconds;
	vector<TChannel>	vecChannels;
};



//-----------------��



struct TBone
{
	_uint			iID;
	wstring			strName;

	_int			iParentID;
	_float4x4		matTransform;
};


//-----------------���͸���


struct TMaterial
{
	wstring strName;
	wstring strTextures[TEXTURETYPE_MAX];
};


/// <summary>
/// �����ÿ� �� ������ ���� Ŭ����
/// </summary>
class ENGINE_DLL CModelBinary : public CBase
{
	INFO_CLASS(CModelBinary, CBase)

protected:
	explicit CModelBinary();
	explicit CModelBinary(const CModelBinary& rhs) = default;
	virtual ~CModelBinary() = default;

public:
	virtual HRESULT	Initialize();

public:
	static CModelBinary* Create();

private:
	virtual void	Free() override;

	
public:
	// ���� Ž�� fbx ����ŷ, �ִϸ��̼� ���� ����
	static HRESULT Bake_StaticModel(const wstring& strFolderPath);
private:
	static HRESULT Bake_StaticModel_Recursive(const fs::path& directory, list<wstring>& fbxFileList);

private:


#ifdef _DEBUG
public:
	// �Ϲ� ���� �� ����ŷ
	static HRESULT Bake_Model(const wstring& strFilePath, const _fmatrix PivotMatrix);
private:
	void Apply_PrePostRotation(aiNode* pAINode, const aiMatrix4x4& PreRotMatrix);
	// Assimp�� ����� �ִ� PreRotation�� ã���ִ� �Լ�.
	aiNode* Find_PreRotationNode(const aiNode* pAINode);
	// �޽� �ε����� �޽��� ���� ��带 ã���ִ� �Լ�.
	aiNode* Find_MeshNode(const aiNode* pAINode, _uint iMeshIndex);
	HRESULT Bake_Meshes(const aiScene* AIScene, const _fmatrix PreRotationMatrix
	, _bool bIsOnlyMesh, _bool IsAnimMesh);
	HRESULT Bake_Materials(const aiScene* AIScene, const string& strModelFilePath); //Material == Texture
	HRESULT Bake_Bones(aiNode* pAINode, _int iParentIndex);
	HRESULT Bake_BoneHash(aiNode* pAINode, wstring& strTotalName, _bool bIsRoot);
	HRESULT Bake_Animations(const aiScene* AIScene);
#endif

public:
	HRESULT Rebake_Materials(const wstring& strFilePath);

public:
	HRESULT Save_Binary(const wstring& strFilePath);
	HRESULT Load_Binary(const wstring& strFilePath);

	HRESULT Save_GroupBinary(const wstring& strFilePath, vector<CModelBinary*> pModels);

public:
	_bool				m_bIsOnlyAnims = { false };
	_uint_64			m_iBoneHash = { 0 };		// ���� ������ �ؽ��Ͽ� �𵨿��� �ִϸ��̼��� ã�� �뵵�� ����
	_uint				m_iNumBones_ForHash = { 0 };
	vector<TMesh>		m_Meshes;
	vector<TMaterial>	m_Materials;
	vector<TBone>		m_Bones;
	vector<TAnim>		m_Anims;
	vector<_float4x4>	m_MeshMatrices;
	_float4x4			m_GroupMatrix;

};

END