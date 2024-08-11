#include "Texture_Manager.h"

#include "Texture_Data.h"

#include "DirectXTex/DirectXTex.h"

#include "Utility_File.h"
#include "Path_Mgr.h"

CTexture_Manager::CTexture_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
}

CTexture_Manager* CTexture_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strMainPath)
{
	ThisClass* pInstance = new ThisClass(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strMainPath)))
	{
		MSG_BOX("TextureMgr Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture_Manager::Free()
{
	CoUninitialize();
	m_TexturesMap.clear();
}

HRESULT CTexture_Manager::Initialize(const wstring& strMainPath)
{
	m_strMainPath = strMainPath;

	CoUninitialize();
	if (FAILED(CoInitializeEx(nullptr, 0)))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CTexture_Manager::IsExists_SRV(const wstring& strTextureKey)
{
	auto iter = m_TexturesMap.find(strTextureKey);
	if (iter == m_TexturesMap.end())
		RETURN_EFAIL;

	if ((*iter).second.get()->Get_TextureCount() == 0)
		return E_NOTIMPL;

	return S_OK;
}

ID3D11ShaderResourceView* CTexture_Manager::Find_SRVFromIndex(const wstring& strTextureKey, const _uint iIndex)
{
	auto iter = m_TexturesMap.find(strTextureKey);
	if (iter == m_TexturesMap.end())
		return nullptr;

	return (*iter).second->Get_SRV(iIndex);
}

HRESULT CTexture_Manager::Reference_SRVs(const wstring& strTextureKey, vector<ComPtr<ID3D11ShaderResourceView>>& RefSRVs)
{
	auto iter = m_TexturesMap.find(strTextureKey);
	if (iter == m_TexturesMap.end())
		return E_FAIL;

	if (FAILED((*iter).second->Reference_SRVs(RefSRVs)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTexture_Manager::Ready_Texture(const wstring& strFilePath, const _uint iNumTextures, const _bool bPermanent, _bool bUseMainPath)
{
	shared_ptr<CTexture_Data> pTextureData = { nullptr };
	HRESULT hr = S_OK;

	wstring strFileName = 
		CUtility_File::Get_FilePath(CPath_Mgr::TEXTURE_FILE, CUtility_File::ConvertToFileName(strFilePath));
 	auto iter = m_TexturesMap.find(strFilePath);
	// �ؽ�ó�� ������ ���θ����.
	if (iter == m_TexturesMap.end())
	{
		pTextureData = BaseMakeShared(CTexture_Data::Create(m_pDevice.Get(), m_pContext.Get()));
		
		// �����н��� ������ ��θ� Ű�� ����
		//if (bUseMainPath)
		//	hr = pTextureData->Insert_Texture(m_strMainPath + strFilePath, iNumTextures, bPermanent);
		//// ���� �н���� �����, Ȥ�� ���� ��η� Ű�� �����Ѵ�.
		//else
		//	hr = pTextureData->Insert_Texture(strFilePath, iNumTextures, bPermanent);
		if(iNumTextures > 1)
			hr = pTextureData->Insert_Texture(strFileName, iNumTextures, bPermanent);
		else
			hr = pTextureData->Insert_Texture(strFileName, iNumTextures, bPermanent);
		// �ؽ��İ� ���� ���̸� �ּҿ� %d�� �ٿ��ش�.
		
		if (SUCCEEDED(hr))
		{
			if (!m_TexturesMap.emplace(strFilePath, pTextureData).second)
				pTextureData.reset();
		}
		else
			cerr << "�ؽ�ó ������ �����߽��ϴ�. : " << ConvertToString(strFilePath) << endl;
	}

	return hr;
}

HRESULT CTexture_Manager::Include_All_Textures(const wstring& strMainPath, _bool bSavePath, const _bool bPermanent)
{
	HRESULT hr = S_OK;

	if (!filesystem::exists(m_strMainPath + strMainPath))
	{
		cout << "Texture Manager Include_All_Textures : ������ �����ϴ�..." << ConvertToString(m_strMainPath + strMainPath) << endl;
		RETURN_EFAIL;
	}
		
	m_bIsUsingAsync = true;
	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator(m_strMainPath + strMainPath))
	{
		string strFIlePath = WstrToStr(entry.path());

		_char	szDdrive[MAX_PATH] = "";
		_char	szDirectory[MAX_PATH] = "";
		_char	szFileName[MAX_PATH] = "";
		_char	szExc[MAX_PATH] = "";

		_splitpath_s(strFIlePath.c_str(), szDdrive, MAX_PATH, szDirectory, MAX_PATH,
			szFileName, MAX_PATH, szExc, MAX_PATH);

		if (!strcmp(szExc, ""))
			continue;

		CTexture_Data* pTextureData = { nullptr };

		wstring wstrFileName = StrToWstr(szFileName);

		auto iter = m_TexturesMap.find(wstrFileName);

		if (iter == m_TexturesMap.end())
		{
			pTextureData = CTexture_Data::Create(m_pDevice.Get(), m_pContext.Get());

			// �񵿱�ó��
			m_Futures.push_back(async(launch::async, &CTexture_Manager::Insert_TextureAsync, this,
				wstrFileName, pTextureData, entry.path(), 1, bPermanent, bSavePath));
		}
	}

	WaitAsync();

	m_bIsUsingAsync = false;

	return hr;
}

HRESULT CTexture_Manager::Insert_TextureAsync(const wstring& strTextureTag, CTexture_Data* pTexture, 
	const wstring& strFilePath, _uint iNumTextures, _bool bIsPermanent
	, _bool bIsSavePath)
{
	shared_ptr<CTexture_Data> pTextureData = BaseMakeShared(pTexture);
	HRESULT hr = pTexture->Insert_Texture(strFilePath, iNumTextures, bIsPermanent);

	if (SUCCEEDED(hr))
	{
		GuardAsync();
		m_TexturesMap.emplace(strTextureTag, pTextureData);
		UnGuardAsync();
		if (bIsSavePath)
			CPath_Mgr::GetInstance()->Add_FilePath(CPath_Mgr::FILE_TYPE::TEXTURE_FILE, strTextureTag, strFilePath);
	}
	else
	{
		cout << "�ؽ�ó �ε� ���� : " << ConvertToString(strFilePath) << endl;
		return hr;
	}

	return hr;
}

void CTexture_Manager::Clear_Textures(_bool bIsAllClear)
{
	for (auto iter = m_TexturesMap.begin(); iter != m_TexturesMap.end(); )
	{
		_bool bIsErased = { false };

		// ������ �������� ������� ����
		if (bIsAllClear)
		{
			// ���� �ؽ�ó �ƹ��ų� �����ϸ� 
			auto pTexture = (*iter).second->Get_SRV(0);
			if (pTexture)
			{
				_int iRefCount = static_cast<_int>(pTexture->AddRef() - 1);
				pTexture->Release();
				if (iRefCount <= 1)
				{
					iter = m_TexturesMap.erase(iter);
					bIsErased = true;
				}
			}
			// Ȥ���� �ؽ�ó �����Ͱ� ���ٶ�� �ϸ� ������ �ִ� �����̳��̴� �����Ѵ�.
			else
			{
				iter = m_TexturesMap.erase(iter);
				bIsErased = true;
			}
		}
		// ���������Ͱ� �ƴ� �� ����
		else
		{
			if (!(*iter).second->IsPermanent())
			{
				iter = m_TexturesMap.erase(iter);
				bIsErased = true;
			}
		}

		if (!bIsErased)
			++iter;
	}
}

void CTexture_Manager::WaitAsync()
{
	for (auto iter = m_Futures.begin(); iter != m_Futures.end();)
	{
		future_status status = (*iter).wait_for(chrono::duration(0.8ms));
		if (status == future_status::ready)
		{
			if (FAILED((*iter).get()))
			{
				cerr << "�ؽ�ó ����" << endl;
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
				cerr << "�ؽ�ó ����" << endl;
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

void CTexture_Manager::GuardAsync()
{
	if (m_bIsUsingAsync == true)
		m_AsyncMutex.lock();
}

void CTexture_Manager::UnGuardAsync()
{
	if (m_bIsUsingAsync == true)
		m_AsyncMutex.unlock();
}
