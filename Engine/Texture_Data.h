#pragma once

#include "Texture_Manager.h"

BEGIN(Engine)

/// <summary>
/// �ؽ�ó ������ ����� Ŭ����
/// �߻�Ŭ����
/// </summary>
class CTexture_Data : public CBase
{
	DERIVED_CLASS(CBase, CTexture_Data)

public:
	explicit CTexture_Data(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CTexture_Data(const CTexture_Data& rhs);
	virtual ~CTexture_Data() = default;

public:
	virtual HRESULT		Initialize();

public:
	static	CTexture_Data* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	
protected:
	virtual void		Free();

protected:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };					// ��ġ
	ComPtr<ID3D11DeviceContext>			m_pDeviceContext = { nullptr };				// ��ġ ���ؽ�Ʈ

public:
	// �޸𸮸� ������ �� ���.
	void	Unload();
	HRESULT	Load(ID3D11ShaderResourceView* _pSRV);
	
public:
	virtual HRESULT		Insert_Texture(const wstring& strFilePath, const _uint iNumTextures, const _bool bPermanent);
	void				Load_Complete() { m_bIsLoaded = true; }

public:
	void								Set_Permanent(const _bool value) { m_bIsPermanent = value; }
	const _bool&						IsLoaded() const { return m_bIsLoaded; }
	const _bool&						IsPermanent() const { return m_bIsPermanent;}
	const _uint&						Get_TextureCount() const { return m_iNumTextures;}
	ID3D11ShaderResourceView* const		Get_SRV(const _uint iIndex);
	HRESULT								Reference_SRVs(vector<ComPtr<ID3D11ShaderResourceView>>& RefSRVs);

private:
	_bool										m_bIsLoaded = false;			// �ε�� �ؽ�ó üũ��
	_bool										m_bIsPermanent = false;			// ���� ���� �ؽ�ó ����, �������� ���Ǵ� �뵵
	_uint										m_iNumTextures = 0;
	vector<ComPtr<ID3D11ShaderResourceView>>	m_SRVs;							// ���̴� ���÷�

};

END