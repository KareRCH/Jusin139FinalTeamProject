#pragma once

#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CTexture_Data;

/// <summary>
/// �ؽ�ó�� �����ϴ� �Ŵ���.
/// �ؽ�ó�� ó�� ������ ���ҽ��� ��θ� ��������,
/// ��ΰ����� Ű��, ���� �̸��� Ȯ���ڸ� �� ������ �̸��� ����.
/// ������� ���� �߰������� ���� ������.
/// �׷��Ͽ��� ���͸��� ���ִ� �ؽ�ó ��θ� ���� �Ŵ����� �����Ͽ� �ؽ�ó�� ���� �� �ִ����Ѵ�.
/// </summary>
class CTexture_Manager final : public CBase
{
	DERIVED_CLASS(CBase, CTexture_Manager)

private:
	explicit CTexture_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTexture_Manager(const CTexture_Manager& rhs) = delete;
	virtual ~CTexture_Manager() = default;

public:
	HRESULT						Initialize(const wstring& strMainPath);

public:
	static CTexture_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strMainPath);

private:
	virtual void				Free();

private:
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

public:
	// �ؽ�ó�� �����ϴ��� Ȯ��
	HRESULT						IsExists_SRV(const wstring& strTextureKey);

	// ã�� �ؽ�ó�� ���� �� SRV�� �޾ƿ��� �Լ� (�� �Ⱦ�����)
	ID3D11ShaderResourceView*	Front_SRV(const wstring& strTextureKey) { return Find_SRVFromIndex(strTextureKey, 0); }

	// ã�� �ؽ�ó�� Ư�� �ε����� SRV�� �޾ƿ��� �Լ� (�� �Ⱦ�����)
	ID3D11ShaderResourceView*	Find_SRVFromIndex(const wstring& strTextureKey, const _uint iIndex);

	// ���̴� ���ҽ� �迭�� ����� �ؽ�ó �����κ��� �޾ƿ���
	HRESULT						Reference_SRVs(const wstring& strTextureKey, vector<ComPtr<ID3D11ShaderResourceView>>& RefSRVs);

	// �ؽ�ó�� �ε��ϱ�
	HRESULT						Ready_Texture(const wstring& strFilePath, const _uint iNumTextures, const _bool bPermanent, _bool bUseMainPath);

	// Ư�� ���� �ȿ� ��� �ؽ��ĸ� �ε� -> ��������Ʈ �ؽ��� �ȵ�
	HRESULT						Include_All_Textures(const wstring& strMainPath, _bool bSavePath, const _bool bPermanent);

	HRESULT Insert_TextureAsync(const wstring& strTextureTag, CTexture_Data* pTexture, 
		const wstring& strFilePath, _uint iNumTextures, _bool bIsPermanent, _bool bIsSavePath);

	// �ؽ�ó�� ���۷��� ī��Ʈ�� 0�� ��� �����ϴ� �Լ�, true�� ������ �������� �Ͱ� ������� �ؽ�ó�� �����Ѵ�. [���� �߰�, ���� �ʿ�]
	void						Clear_Textures(_bool bIsAllClear);

public:
	const wstring&				Get_MainPath() const { return m_strMainPath; }
	const map<wstring, shared_ptr<CTexture_Data>>* Get_TexturesMap() const { return &m_TexturesMap; }

private:
	wstring										m_strMainPath = { L"" };	// ���� �н� ����. �ε�Ǵ� �ؽ�ó�� Ű ���� ���� �н��� ������ ��.
	map<wstring, shared_ptr<CTexture_Data>>		m_TexturesMap;				// �ؽ�ó ���� ����, �н� �̸��� Ű��. ���� �ؽ�ó�� ���� �������� ����. ���⼭�� �з��� ���� ����.


public:
	// �񵿱� ���
	void WaitAsync();
	// �񵿱� ���ؽ� �۵�
	void GuardAsync();
	// �񵿱� ���ؽ� ����
	void UnGuardAsync();

private:
	atomic<_bool>			m_bIsUsingAsync = { false };
	mutex					m_AsyncMutex;
	list<future<HRESULT>>	m_Futures;

};

END