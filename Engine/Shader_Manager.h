#pragma once

#include "Base.h"
#include "ShaderMgr_Enum.h"

BEGIN(Engine)


/// <summary>
/// �Ϲ� ���̴����� ���� ������
/// </summary>
class FShaderData final
{
	THIS_CLASS(FShaderData)
private:
	FShaderData() = default;
public:
	~FShaderData()
	{
	}

public:
	HRESULT Initialize()
	{
		return S_OK;
	}

public:
	static FShaderData* Create()
	{
		ThisClass* pInstance = new ThisClass();

		if (FAILED(pInstance->Initialize()))
		{
			Engine::Safe_Delete(pInstance);
			MSG_BOX("RenderMgr Create Failed");
		}

		return pInstance;
	}

	void Free() 
	{
		delete this;
	}
	void Set_Shader(const wstring& _strFileName, ID3DBlob* _pShaderByte, ID3D11DeviceChild* _pShaderBuffer)
	{
		if (_pShaderByte == nullptr || _pShaderBuffer == nullptr)
			return;

		strFileName = _strFileName;
		bLoaded = true;
		pShaderByte = _pShaderByte;
		pShaderBuffer = _pShaderBuffer;
	}

public:
	const ComPtr<ID3DBlob>&				CRef_ShaderByte() const { return pShaderByte; }
	const ComPtr<ID3D11DeviceChild>&	CRef_ShaderBuffer() const { return pShaderBuffer; }
	const _bool							IsLoaded() const { return bLoaded; }

private:
	wstring							strFileName = L"";				// �ε��� ���̴� �ڵ�, ���� ��ũ��
	_bool							bLoaded = false;				// �ε��
	ComPtr<ID3DBlob>				pShaderByte = { nullptr };		// ���̴� ����Ʈ �ڵ�
	ComPtr<ID3D11DeviceChild>		pShaderBuffer = { nullptr };	// ���̴� ����
};


/// <summary>
/// FX11 ���� ������
/// </summary>
class FEffectData final : public CBase
{
	DERIVED_CLASS(CBase, FEffectData)

private:
	FEffectData() = default;
	~FEffectData() = default;

public:
	HRESULT Initialize() { return S_OK; }

public:
	static shared_ptr<FEffectData> Create()
	{
		ThisClass* pInstance = new ThisClass();

		if (FAILED(pInstance->Initialize()))
		{
			MSG_BOX("RenderMgr Create Failed");
			Safe_Release(pInstance);
		}

		return BaseMakeShared(pInstance);
	}

public:
	virtual void Free() {}

public:
	D3DX11_TECHNIQUE_DESC				tTechniqueDesc;		// ��ũ�ϼ� ���� ��ü
	ComPtr<ID3DX11Effect>				pEffect;			// FX ���̴� ����
	vector<ComPtr<ID3D11InputLayout>>	pInputLayouts;		// ���̾ƿ� ����

public:
	const D3D11_INPUT_ELEMENT_DESC*	pElements = { nullptr };	// �������� ����
	_uint							iNumElements = { 0 };		// �������� ����
	const D3D_SHADER_MACRO*			pShaderMacro = { nullptr };	// �������� ����
};


/// <summary>
/// ���̴� �ڵ带 �����ϰ� �������ִ� �Ŵ����Դϴ�.
/// ���� ������Ʈ ������� �ᵵ �ǰ�
/// �Ŵ����� ����Ͽ� �����ε����� ����Ͽ��� �����ϴ�.
/// </summary>
class CShader_Manager final : public CBase
{
	DERIVED_CLASS(CBase, CShader_Manager)
private:
	explicit CShader_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CShader_Manager(const CShader_Manager& rhs) = delete;
	virtual ~CShader_Manager() = default;

public:
	HRESULT					Initialize(const wstring& strMainPath);

public:
	static CShader_Manager*	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strMainPath);

private:
	virtual void			Free() override;

#pragma region ����̽�
private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pDeviceContext = { nullptr };
#pragma endregion



#pragma region �Ϲ� ���̴� (FX11 �ƴ�)
public:
	// �����̸��� ���� ���̴��� �ε�
	HRESULT	Load_Shader(const wstring& strFileName, const EShaderType eType, const _char* pEntryPoint);

	// �ε�� ���̴��� �ִٸ� �� ���� ��ȯ�Ѵ�.
	const ComPtr<ID3DBlob> Find_ShaderByte(const EShaderType eType, const wstring& strName) const;
	template <EShaderType Type>
	ComPtr<ShaderType<Type>> Find_ShaderBuffer(const wstring& strName) const;
	

private:
	ID3DBlob* Read_ShaderBinary(const wstring& strFile);
	ID3DBlob* Complie_Shader(const wstring& strFile, const _char* pEntryPoint);

private:
	wstring									m_strMainPath;
	unordered_map<wstring, FShaderData*>	m_mapShaderData[ECast(EShaderType::Size)];
#pragma endregion


#pragma region ����Ʈ ���̴� (�� FX ��� ���̴�, FX11)
public:
	// ����Ʈ �ε�
	HRESULT Load_Effect(const wstring& strFileName, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const D3D_SHADER_MACRO* pShaderMacro = nullptr);
	HRESULT Recompile_EffectsAll();
	HRESULT Recompile_Effect(const wstring& strFileName, shared_ptr<FEffectData> pEffectData);
	// ����Ʈ ã��
	ID3DX11Effect* Find_Effect(const wstring& strKey) const;
	// ����Ʈ �����͸� ã��
	shared_ptr<FEffectData> Find_EffectData(const wstring& strKey, const D3D_SHADER_MACRO* pShaderMacro) const;

private:
	// ���������� Wstring�� ���鶧 ���
	wstring Make_MacroToWstring(const D3D_SHADER_MACRO* pShaderMacro) const;


private:
	map<const wstring, shared_ptr<FEffectData>>	m_mapEffects;		// Effect11�� ������� ���̴� �����.
#pragma endregion

};


#pragma region Get Shader ���ø�

template <EShaderType Type>
ComPtr<ShaderType<Type>> CShader_Manager::Find_ShaderBuffer(const wstring& strName) const
{
	constexpr _uint iIndex = ECast(Type);

	auto iter = m_mapShaderData[iIndex].find(strName);
	if (iter == m_mapShaderData[iIndex].end() || !(*iter).second->IsLoaded())
		return nullptr;

	ID3D11DeviceChild* pShaderBuffer = (*iter).second->CRef_ShaderBuffer().Get();

	return ComPtr<ShaderType<Type>>(Cast<ShaderType<Type>*>(pShaderBuffer));
}

#pragma endregion



END