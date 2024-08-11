#include "Model_Manager.h"

#include "ModelBinary.h"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

#include "Utility_File.h"

CModel_Manager::CModel_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
}

HRESULT CModel_Manager::Initialize(const wstring& strMainDir)
{
	m_strMainDir = strMainDir;

	m_iThreadLimit = thread::hardware_concurrency();

	return S_OK;
}

CModel_Manager* CModel_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strMainDir)
{
	ThisClass* pInstance = new ThisClass(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strMainDir)))
	{
		MSG_BOX("ModelMgr Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Manager::Free()
{
	for (auto& Pair : m_mapModelDatas)
		Safe_Release(Pair.second);
	m_mapModelDatas.clear();
}

HRESULT CModel_Manager::Load_Model(const wstring& strFileName, _bool bSavePath)
{
	GaurdAsync();
	FModelData* pModelData = ConCast<FModelData*>(Find_ModelData(strFileName));
	UnGaurdAsync();
	if (nullptr != pModelData)
		return S_OK;

	CModelBinary* pModel = CModelBinary::Create();

	//wstring strFilePath = m_strMainDir + strFileName;
	wstring strFilePath = TEXT(""), strParentFolder = TEXT("");
	strFilePath = CUtility_File::Get_FilePath(CPath_Mgr::MODEL_FILE, strFileName);
	if (strFilePath == TEXT("Not_Find"))
		strFilePath = CUtility_File::Get_FilePath(CPath_Mgr::ANIM_FILE, strFileName);

	// �ϰ��� ���� /�� �ٲٱ�, �θ� �������� �̾Ƴ���
	{
		replace(strFilePath.begin(), strFilePath.end(), L'\\', L'/');
		_bool bCanFoundParentPath = false;
		size_t iLast = strFilePath.find_last_of(L'/');
		if (iLast != wstring::npos)
		{
			strParentFolder = strFilePath.substr(0, iLast);
			bCanFoundParentPath = true;
		}
		size_t iFirst = strParentFolder.find_last_of(L'/');
		if (iFirst != wstring::npos)
		{
			strParentFolder = strParentFolder.substr(iFirst + 1);
			strParentFolder += TEXT("|");
			bCanFoundParentPath = true;
		}
		if (!bCanFoundParentPath)
			strParentFolder = TEXT("");
	}

	if (FAILED(pModel->Load_Binary(strFilePath)))
	{
		Safe_Release(pModel);
		cerr << "amodel ������ �����ϴ� : " << ConvertToString(strFilePath) << endl;
		RETURN_EFAIL;
	}


	// �� ������ ����
	GaurdAsync();
	pModelData = Add_ModelData(strFileName, pModel);
	UnGaurdAsync();
	if (pModelData == nullptr)
	{
		Safe_Release(pModel);
		return S_OK;
	}

	// �ִϸ��̼�, �Ϲ� �� ���Ͽ� ���� �ε� ����� �з�
	if (!pModel->m_bIsOnlyAnims)
	{
		Load_Bones(pModelData, pModel);

		Load_Animations(pModelData, pModel, strParentFolder);

		Load_Materials(pModelData, pModel, strFilePath);

		Load_Meshes(pModelData, pModel);
	}
	else
	{
		Load_Animations(pModelData, pModel, strParentFolder);
	}

	Safe_Release(pModel);
	cout << "�ε� �Ϸ��: " << ConvertToString(strFileName) << endl;

	return S_OK;
}

HRESULT CModel_Manager::Load_DirectorySub_Models(const wstring& strFolderPath, _bool bSavePath)
{
	// ��ȯ���� ���� ���� ��ȯ
#ifdef _DEBUG
	CModelBinary::Bake_StaticModel(m_strMainDir + strFolderPath);
	//// ���� �ý��� ��ε�
	//if (FAILED(CUtility_File::All_FilePath_Save(L"../Bin/Resources/Model/")))
	//	RETURN_EFAIL;
#endif
	// ���� ������ ��� amodel, aanim ���� ã�� ����Ʈ�� ��´�.
	list<wstring> listModels;
	list<wstring> listAnims;
	list<wstring> listTemp;
	fs::path directory(m_strMainDir + strFolderPath);
	if (fs::exists(directory) && fs::is_directory(directory))
	{
		Load_DirectorySub_Models_Recursive(directory, listModels, listAnims);
	}

	m_bIsUsingAsync = true;
	for (auto iter = listModels.begin(); iter != listModels.end(); ++iter)
	{
		size_t iCutIndex = m_strMainDir.length();
		wstring strNewPath = (*iter).substr(iCutIndex);
		fs::path filePath(strNewPath);
		wstring FileName = filePath.stem().wstring();
		listTemp.push_back(FileName);
		//m_Futures.push_back(async(launch::async, &CModel_Manager::Load_Model, this, FileName, bSavePath));
		//Load_Model(FileName, bSavePath);
	}
	for (auto iter = listTemp.begin(); iter != listTemp.end(); ++iter)
	{
		m_Futures.push_back(async(launch::async, &CModel_Manager::Load_Model, this, (*iter), bSavePath));
	}
	WaitAsync(true);
	listTemp.clear();


	for (auto iter = listAnims.begin(); iter != listAnims.end(); ++iter)
	{
		size_t iCutIndex = m_strMainDir.length();
		wstring strNewPath = (*iter).substr(iCutIndex);
		fs::path filePath(strNewPath);
		wstring FileName = filePath.stem().wstring();
		listTemp.push_back(FileName);
		//m_Futures.push_back(async(launch::async, &CModel_Manager::Load_Model, this, FileName, bSavePath));
		//Load_Model(FileName, bSavePath);
	}
	for (auto iter = listTemp.begin(); iter != listTemp.end(); ++iter)
	{
		m_Futures.push_back(async(launch::async, &CModel_Manager::Load_Model, this, (*iter), bSavePath));
	}
	WaitAsync(true);
	listTemp.clear();
	m_bIsUsingAsync = false;

	return S_OK;
}

HRESULT CModel_Manager::Load_DirectorySub_Models_Recursive(const fs::path& directory, list<wstring>& listModels, list<wstring>& listAnims)
{
	for (const auto& entry : fs::directory_iterator(directory))
	{
		if (fs::is_directory(entry)) {
			// ���丮�� ��� ��������� Ž��
			Load_DirectorySub_Models_Recursive(entry.path(), listModels, listAnims);
		}
		else if (fs::is_regular_file(entry))
		{
			// amodel�� �ִٸ� ����Ʈ�� �־� �� �ε��׸� �־��ش�.
			wstring strFileName = entry.path().wstring();
			if (strFileName.find(TEXT(".amodel")) != wstring::npos
			|| strFileName.find(TEXT(".groupmodel")) != wstring::npos)
			{
				for (_tchar& ch : strFileName)
				{
					if (ch == L'\\')
						ch = L'/';
				}
				listModels.push_back(strFileName);
			}
			else if (strFileName.find(TEXT(".aanim")) != wstring::npos)
			{
				for (_tchar& ch : strFileName)
				{
					if (ch == L'\\')
						ch = L'/';
				}
				listAnims.push_back(strFileName);
			}
		}
	}

	return S_OK;
}

HRESULT CModel_Manager::Rebake_Model(const wstring& strFileName)
{
	CModelBinary* pModel = CModelBinary::Create();

	//wstring strFilePath = m_strMainDir + strFileName;
	wstring strFilePath = TEXT(""), strParentFolder = TEXT("");
	strFilePath = CUtility_File::Get_FilePath(CPath_Mgr::MODEL_FILE, strFileName);
	if (strFilePath == TEXT("Not_Find"))
		strFilePath = CUtility_File::Get_FilePath(CPath_Mgr::ANIM_FILE, strFileName);

	// �ϰ��� ���� /�� �ٲٱ�, �θ� �������� �̾Ƴ���
	{
		replace(strFilePath.begin(), strFilePath.end(), L'\\', L'/');
		_bool bCanFoundParentPath = false;
		size_t iLast = strFilePath.find_last_of(L'/');
		if (iLast != wstring::npos)
		{
			strParentFolder = strFilePath.substr(0, iLast);
			bCanFoundParentPath = true;
		}
		size_t iFirst = strParentFolder.find_last_of(L'/');
		if (iFirst != wstring::npos)
		{
			strParentFolder = strParentFolder.substr(iFirst + 1);
			strParentFolder += TEXT("|");
			bCanFoundParentPath = true;
		}
		if (!bCanFoundParentPath)
			strParentFolder = TEXT("");
	}

	if (FAILED(pModel->Load_Binary(strFilePath)))
	{
		Safe_Release(pModel);
		cerr << "amodel ������ �����ϴ� : " << ConvertToString(strFilePath) << endl;
		RETURN_EFAIL;
	}

	// amodel�� �� ���͸����� �ٽ� �ε��Ų��. MRV, O, A, M �� ���� �߰������� ���� ������ �ؽ�ó�� �ε� ��Ų��.
	if (!pModel->m_bIsOnlyAnims)
	{
		if (SUCCEEDED(pModel->Rebake_Materials(strFilePath)))
		{
			if (FAILED(pModel->Save_Binary(strFilePath)))
			{

			}
		}
	}

	Safe_Release(pModel);

	return S_OK;
}

HRESULT CModel_Manager::Rebake_MaterialsAll(const wstring& strFolderPath)
{
	// ���� ������ ��� amodel, aanim ���� ã�� ����Ʈ�� ��´�.
	list<wstring> listModels;
	fs::path directory(m_strMainDir + strFolderPath);
	if (fs::exists(directory) && fs::is_directory(directory))
	{
		Rebake_MaterialsAll_Recursive(directory, listModels);
	}

	m_bIsUsingAsync = true;
	for (auto iter = listModels.begin(); iter != listModels.end(); ++iter)
	{
		size_t iCutIndex = m_strMainDir.length();
		wstring strNewPath = (*iter).substr(iCutIndex);
		fs::path filePath(strNewPath);
		wstring FileName = filePath.stem().wstring();
		//m_Futures.push_back(async(launch::async, &CModel_Manager::Rebake_Model, this, FileName));
		Rebake_Model(FileName);
	}
	//WaitAsync();
	m_bIsUsingAsync = false;

	return S_OK;
}

HRESULT CModel_Manager::Rebake_MaterialsAll_Recursive(const fs::path& directory, list<wstring>& listModels)
{
	for (const auto& entry : fs::directory_iterator(directory))
	{
		if (fs::is_directory(entry)) {
			// ���丮�� ��� ��������� Ž��
			Rebake_MaterialsAll_Recursive(entry.path(), listModels);
		}
		else if (fs::is_regular_file(entry))
		{
			// amodel�� �ִٸ� ����Ʈ�� �־� �� �ε��׸� �־��ش�.
			wstring strFileName = entry.path().wstring();
			if (strFileName.find(TEXT(".amodel")) != wstring::npos
				|| strFileName.find(TEXT(".groupmodel")) != wstring::npos)
			{
				for (_tchar& ch : strFileName)
				{
					if (ch == L'\\')
						ch = L'/';
				}
				listModels.push_back(strFileName);
			}
		}
	}

	return S_OK;
}


void CModel_Manager::Load_Meshes(FModelData* pModelData, CModelBinary* pModel)
{
	if (nullptr == pModel)
		return;

	auto pMeshGroup = pModelData->pMeshGroup;

	pMeshGroup->vecMeshDatas.resize(pModel->m_Meshes.size());
	for (_uint i = 0; i < pModel->m_Meshes.size(); i++)
	{
		TMesh& tMesh = pModel->m_Meshes[i];
		FMeshData* pMeshData = { nullptr };
		if (pModel->m_Anims.empty())
			pMeshData = FMeshData::Create(tMesh, FMeshData::TYPE_MODEL);
		else
			pMeshData = FMeshData::Create(tMesh, FMeshData::TYPE_ANIM_MODEL);
		if (pMeshData != nullptr)
			pMeshGroup->Add_Mesh(tMesh.strName, pMeshData);
	}
}

void CModel_Manager::Load_Materials(FModelData* pModelData, CModelBinary* pModel, const wstring& strPath)
{
	if (nullptr == pModel)
		return;

	auto pMaterialGroup = pModelData->pMaterialGroup;

	vector<wstring> vecTemp = SplitWstr(strPath, L'/');

	for (_uint i = 0; i < pModel->m_Materials.size(); i++)
	{
		TMaterial& Material = pModel->m_Materials[i];
		FMaterialData* pMaterialData = FMaterialData::Create();
		if (FAILED(pMaterialGroup->Add_Material(Material.strName, pMaterialData)))
			continue;

		for (_uint j = 0; j < Cast<_uint>(TEXTURETYPE_MAX); j++)
		{
			if (Material.strTextures[j].empty())
				continue;

			_tchar		szDrive[MAX_PATH] = L"";
			//_tchar		szDirectory[MAX_PATH] = L"";
			_tchar		szDirectory[MAX_PATH] = L"";
			_wsplitpath_s(strPath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
			// _wsplitpath_s(strPath.c_str(), szDrive, MAX_PATH, nullptr, 0, nullptr, 0, nullptr, 0);
			_tchar		szFileName[MAX_PATH] = L"";
			_tchar		szEXT[MAX_PATH] = L"";

			_wsplitpath_s(Material.strTextures[j].c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			_tchar		szTmp[MAX_PATH] = L"";
			wcscpy_s(szTmp, szDrive);
			wcscpy_s(szTmp, szDirectory);

			wcscpy_s(szTmp, szFileName);
			wcscpy_s(szTmp, szEXT);

			fs::path filePath(szDirectory);
			filePath = filePath.parent_path().parent_path() / "Texture";
			wstring newPath = filePath.generic_wstring();
			wcsncpy_s(szDirectory, MAX_PATH, newPath.c_str(), newPath.size() + 1);

			_tchar		szFullPath[MAX_PATH] = TEXT("");
			wcscpy_s(szFullPath, szDrive);
			wcscat_s(szFullPath, szDirectory);
			wcscat_s(szFullPath, szFileName);
			wcscat_s(szFullPath, szEXT);

			if (vecTemp[5] == L"VRStage" && Material.strTextures[1].empty())
			{
				pMaterialData->strTexture[1] = L"T_Flat_VRBattle_C.dds";
			}

			pMaterialData->strTexture[j] = szFileName;
		}
	}

	return;
}

void CModel_Manager::Load_Animations(FModelData* pModelData, CModelBinary* pModel, const wstring& strParentFolderName)
{
	if (nullptr == pModel)
		return;

	auto pAnimGroup = pModelData->pAnimGroup;

	for (_uint i = 0; i < pModel->m_Anims.size(); i++)
	{
		TAnim& Anim = pModel->m_Anims[i];
		FBoneAnimData* pBoneAnimData = FBoneAnimData::Create();

		GaurdAsync();
		HRESULT hr = pAnimGroup->Add_BoneAnim(strParentFolderName + Anim.strName, pBoneAnimData);
		UnGaurdAsync();
		if (FAILED(hr))
			return;

		pBoneAnimData->iID = pModelData->pAnimGroup->Get_NumAnims() - 1;
		pBoneAnimData->strName = strParentFolderName + Anim.strName;
		pBoneAnimData->fDuration = Anim.fDuration;
		pBoneAnimData->fTickPerSecond = Anim.fTickPerSeconds;

		for (_uint j = 0; j < Anim.vecChannels.size(); j++)
		{
			TChannel& Channel = Anim.vecChannels[j];
			FBoneAnimChannelData* pChannelData = FBoneAnimChannelData::Create();

			pChannelData->iBoneID = Channel.iBoneID;
			pChannelData->iNumKeyFrames = Cast<_uint>(Channel.vecKeyFrames.size());
			pBoneAnimData->Add_AnimChannelData(Channel.strName, pChannelData);

			for (_uint k = 0; k < Channel.vecKeyFrames.size(); k++)
			{
				TKeyFrame& KeyFrame = Channel.vecKeyFrames[k];
				FKeyFrame KeyData = {};

				memcpy(&KeyData, &KeyFrame, sizeof(TKeyFrame));

				pChannelData->vecKeyFrames.push_back(move(KeyData));
			}
		}
	}
}

void CModel_Manager::Load_Bones(FModelData* pModelData, CModelBinary* pModel)
{
	if (nullptr == pModel)
		return;

	auto pBoneGroup = pModelData->pBoneGroup;
	vector<_uint> RootBoneFinder;
	pBoneGroup->Reserve_BoneSize((_uint)pModel->m_Bones.size());

	for (_uint i = 0; i < pModel->m_Bones.size(); i++)
	{
		TBone& tBone = pModel->m_Bones[i];

        FBoneData* pBoneData = FBoneData::Create();
        pBoneData->iID = tBone.iID;
        pBoneData->strName = tBone.strName;
        pBoneData->iParentID = tBone.iParentID;

		// ���� ��Ʈ �ִϸ��̼� �κ��� ã�� �����Ѵ�.
		if (pBoneData->strName.find(TEXT("$_Translation")) != wstring::npos)
		{
			pBoneGroup->Set_RootTransBoneInd(pBoneData->iID);
			//// ���� ��Ʈ��
			//wstring strSub =
			//	pBoneData->strName.substr(0, pBoneData->strName.find(TEXT("_$Assimp")));
			//pBoneGroup->Set_RootMotionBoneInd(pBoneData->iID);
		}

		if (pBoneData->strName.find(TEXT("$_Rotation")) != wstring::npos)
		{
			pBoneGroup->Set_RootRotateBoneInd(pBoneData->iID);
		}

		if (pBoneData->strName.find(TEXT("$_Scaling")) != wstring::npos)
		{
			pBoneGroup->Set_RootScaleBoneInd(pBoneData->iID);
		}

		// �� �߰� + ��� �� �־���
		pBoneGroup->Add_BoneData(tBone.strName, pBoneData, tBone.matTransform);
	}
	pBoneGroup->Invalidate_FinalTransforms();

	// �Ⱦ�
	/*if (FAILED(pBoneGroup->Create_VTF_Bones()))
	{
		cout << "�� ������ ���� VT�� ������ �ʽ��ϴ� : " << pBoneGroup->CRef_BoneDatas_Count() << "��" << endl;
	}
	else
	{
		cout << "VT�� ��������ϴ� : " << pBoneGroup->CRef_BoneDatas_Count() << "��" << endl;
	}

	if (FAILED(pBoneGroup->Create_TransformBuffers()))
	{
		cout << "GPU�� ��Ĺ��۸� ����µ� �����߽��ϴ�." << endl;
		assert(true);
	}*/
}

const FMeshData* const CModel_Manager::Find_MeshData(const wstring& strModelKey, const wstring& strMeshKey, const _uint iRangeIndex)
{
	auto iter = m_mapModelDatas.find(strModelKey);
	if (iter == m_mapModelDatas.end())
		return nullptr;

	return (*iter).second->pMeshGroup->Find_Mesh(strMeshKey, iRangeIndex);
}

const FMeshData* const CModel_Manager::Find_MeshData(const wstring& strModelKey, const _uint iIndex)
{
	auto iter = m_mapModelDatas.find(strModelKey);
	if (iter == m_mapModelDatas.end())
		return nullptr;

	return (*iter).second->pMeshGroup->Find_Mesh(iIndex);
}

CBoneGroup* CModel_Manager::Clone_BoneGroup(const wstring& strModelKey)
{
	auto iter = m_mapModelDatas.find(strModelKey);
	if (iter == m_mapModelDatas.end())
		return nullptr;

	FModelData* pModelGroup = (*iter).second;
	return pModelGroup->pBoneGroup->Clone();
}

const FModelData* const CModel_Manager::Find_ModelData(const wstring& strModelKey)
{
	auto iter = m_mapModelDatas.find(strModelKey);
	if (iter == m_mapModelDatas.end())
		return nullptr;

	return (*iter).second;
}

FModelData* CModel_Manager::Add_ModelData(const wstring& strModelKey, CModelBinary* pModelBinary)
{
	FModelData* pModelData = { nullptr };

	// ���̳ʸ����� �ؽ��� ���� Ű�� ã��
	auto iter = m_mapModelHashes.find(pModelBinary->m_iBoneHash);
	// Ű�� ���� ��, ���� �����Ƿ� �� ������ �ʿ��� ���� ã�´�.
	// �� ���� ��ȯ�Ѵ�. 
	if (iter != m_mapModelHashes.end())
	{
		if (pModelBinary->m_bIsOnlyAnims)
		{
			auto iterModel = m_mapModelDatas.find((*iter).second);
			if (iterModel != m_mapModelDatas.end())
				return (*iterModel).second;
		}
		else
			return nullptr;
	}
	// Ű�� ���ٸ� �𵨵����Ͱ� ���� ������ ���� �����Ѵ�.
	// 
	else
	{
		pModelData = FModelData::Create(false);

		// ��Ű
		m_mapModelDatas.emplace(strModelKey, pModelData);
		// �ؽ�Ű
		m_mapModelHashes.emplace(pModelBinary->m_iBoneHash, strModelKey);
	}

	return pModelData;
}

const FMaterialData* const CModel_Manager::Find_MaterialData(const wstring& strModelKey, const _uint iIndex)
{
	auto iter = m_mapModelDatas.find(strModelKey);
	if (iter == m_mapModelDatas.end())
		return nullptr;

	return (*iter).second->pMaterialGroup->Find_Material(iIndex);
}

CMeshGroup* CModel_Manager::Find_MeshGroup(const wstring& strModelKey)
{
	auto iter = m_mapModelDatas.find(strModelKey);
	if (iter == m_mapModelDatas.end())
		return nullptr;

	return (*iter).second->pMeshGroup;
}

CBoneGroup* CModel_Manager::Find_BoneGroup(const wstring& strModelKey)
{
	auto iter = m_mapModelDatas.find(strModelKey);
	if (iter == m_mapModelDatas.end())
		return nullptr;

	return (*iter).second->pBoneGroup;
}

CBoneAnimGroup* CModel_Manager::Find_AnimGroup(const wstring& strModelKey)
{
	auto iter = m_mapModelDatas.find(strModelKey);
	if (iter == m_mapModelDatas.end())
		return nullptr;

	return (*iter).second->pAnimGroup;
}

CMaterialGroup* CModel_Manager::Find_MaterialGroup(const wstring& strModelKey)
{
	auto iter = m_mapModelDatas.find(strModelKey);
	if (iter == m_mapModelDatas.end())
		return nullptr;

	return (*iter).second->pMaterialGroup;
}

void CModel_Manager::WaitAsync(_bool bAllPushed)
{
	for (auto iter = m_Futures.begin(); iter != m_Futures.end();)
	{
		future_status status = (*iter).wait_for(chrono::duration(0.1ms));
		if (status == future_status::ready)
		{
			if (FAILED((*iter).get()))
			{
				cerr << "�𵨷ε� ����" << endl;
			}
			iter = m_Futures.erase(iter);
		}
		else if (status == future_status::timeout)
		{
			++iter;
		}
		else
		{
			if (FAILED((*iter).get()))
			{
				cerr << "�𵨷ε� ����" << endl;
			}
			iter = m_Futures.erase(iter);
		}

		if (iter == m_Futures.end())
		{
			if (m_Futures.size() > 0)
				iter = m_Futures.begin();
		}
	}
}

void CModel_Manager::GaurdAsync()
{
	if (m_bIsUsingAsync)
		m_AsyncMutex.lock();
}

void CModel_Manager::UnGaurdAsync()
{
	if (m_bIsUsingAsync)
	{
		m_AsyncMutex.unlock();
	}
}

void CModel_Manager::WaitAtomic()
{
	while (!m_bIsUsingAsync.load())
	{
		m_bIsUsingAsync = true;
	}
}

void CModel_Manager::ExitAtomic()
{
	m_bIsUsingAsync = false;
}
