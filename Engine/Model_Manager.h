#pragma once

//#include <System/Define/ModelMgr_Define.h>
#include "ModelContainer.h"
#include "MeshContainer.h"
#include "BoneContainer.h"
#include "BoneAnimContainer.h"
#include <filesystem>

BEGIN(Engine)

namespace fs = filesystem;

/// <summary>
/// �� ������
/// �޽�(���� ����)
/// ���͸���
/// ��
/// �ִϸ��̼�
/// ������ �����ϰ� �����ϴ� Ŭ����
/// 
/// ���͸����� ��� �ؽ�ó �Ŵ����� ����Ǿ� �ٷ� �ε��ϵ��� �����ȴ�.
/// 
/// ������Ʈ�� Ȯ������ �𵨰� �ִϸ��̼��� ������ �ε��Ѵ�.
/// �ִϸ��̼��� �ε� �� ���� aanim�� ����� ���縦 ���� �ִϸ��̼� �߰������� Ȯ���Ͽ� �����ϸ�, Ȯ�εǸ� �ش� �ִϸ��̼��� �ε��Ͽ� ������ �ִϸ��̼� ������ �����Ѵ�.
/// 
/// </summary>
class ENGINE_DLL CModel_Manager final : public CBase
{
	DERIVED_CLASS(CBase, CModel_Manager)

private:
	explicit CModel_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CModel_Manager(const CModel_Manager& rhs) = delete;
	virtual ~CModel_Manager() = default;

public:
	HRESULT				Initialize(const wstring& strMainDir);

public:
	static CModel_Manager*	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strMainDir);
private:
	virtual void		Free() override;


private:
	wstring	m_strMainDir = { TEXT("") };			// ������ ���� ���͸�
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };

public:
	HRESULT Load_Binary(const wstring& strFileName, class CModelBinary* pModel);

	/// <summary>
	/// ���� ���̳ʸ� ������ �ε��ϴ� �Լ�
	/// </summary>
	/// <param name="strFileName">MainPath�� ������ ���� ���</param>
	/// <returns></returns>
	HRESULT Load_Model(const wstring& strFileName,_bool bSavePath = false);

	/// <summary>
	/// ������ ������ ���� ���ϵ��� ��� ã�� �ε����ִ� �Լ�
	/// </summary>
	/// <param name="strFolderPath"> MainPath�� ������ ���� ���</param>
	/// <returns></returns>
	HRESULT Load_DirectorySub_Models(const wstring & strFolderPath,_bool bSavePath = false);
	HRESULT Load_DirectorySub_Models_Recursive(const fs::path& directory, list<wstring>& listModels, list<wstring>& listAnims);

	HRESULT Rebake_Model(const wstring& strFileName);

	HRESULT Rebake_MaterialsAll(const wstring& strFolderPath);
	HRESULT Rebake_MaterialsAll_Recursive(const fs::path& directory, list<wstring>& listModels);


	// �� ������ ����
public:
	void Load_Meshes(FModelData* pModelData, class CModelBinary* pModel);
	void Load_Materials(FModelData* pModelData, class CModelBinary* pModel, const wstring& strPath);
	// FileName�� Ű��, �ؽ����� ���Ѵ�.
	void Load_Animations(FModelData* pModelData, class CModelBinary* pModel, const wstring& strParentFolderName);
	void Load_Bones(FModelData* pModelData, class CModelBinary* pModel);


	// �� ������ ����
public:
	const FModelData* const	Find_ModelData(const wstring & strModelKey);
	FModelData* Add_ModelData(const wstring & strModelKey, CModelBinary* pModelBinary);


	// �޽� ������ ����
public:
	const FMeshData* const	Find_MeshData(const wstring& strModelKey, const wstring& strMeshKey, const _uint iRangeIndex);
	const FMeshData* const	Find_MeshData(const wstring& strModelKey, const _uint iIndex);
	

	// �� ������ ����
public:
	CBoneGroup*				Clone_BoneGroup(const wstring& strModelKey);


	// ���͸��� ������ ����
public:
	const FMaterialData* const Find_MaterialData(const wstring& strModelKey, const _uint iIndex);


	// �� ������ �׷� ����
public:
	CMeshGroup*				Find_MeshGroup(const wstring& strModelKey);
	CBoneGroup*				Find_BoneGroup(const wstring& strModelKey);
	CBoneAnimGroup*			Find_AnimGroup(const wstring& strModelKey);
	CMaterialGroup*			Find_MaterialGroup(const wstring& strModelKey);

private:
	string				m_strLoadFilePath;				// ���ο�, �ε��� ���� ���.
	_int				m_iNodeID;						// ���ο�, ��� ��ȣ �ο���.
	_uint				m_iMaterialCount = 0U;
	_uint				m_iBoneCount = 0U;

public:
	const unordered_map<wstring, FModelData*>& Get_ModelDatas() const { return m_mapModelDatas; }

private:
	_unmap<wstring, FModelData*>& Lock_ModelDatas() 
	{ 
		GaurdAsync();
		return m_mapModelDatas; 
	}

private:
	// �� ����, �޽�, ��, ���͸���, �ִϸ��̼�
	unordered_map<wstring, FModelData*>	m_mapModelDatas;
	map<_uint_64, wstring> m_mapModelHashes;


public:
	// �񵿱� ���
	void WaitAsync(_bool bAllPushed);
	// �񵿱� ���ؽ� �۵�
	void GaurdAsync();
	// �񵿱� ���ؽ� ����
	void UnGaurdAsync();
	void WaitAtomic();
	void ExitAtomic();

private:
	using SLoadModelFuture = future<HRESULT>;

	atomic<_bool>			m_bWaitingAsync = { false };
	atomic<_bool>			m_bIsUsingAsync = { false };	// �׽�Ʈ�� ���� ����� ��� ����ȭ. �ߵ�.
	_uint					m_iThreadLimit = { 8 };
	mutex					m_AsyncMutex;	// �񵿱� ���ؽ�
	list<SLoadModelFuture>	m_Futures;		// �񵿱� ���

};

END