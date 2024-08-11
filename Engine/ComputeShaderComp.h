#pragma once

#include "Component.h"
#include "Shader_Manager.h"


BEGIN(Engine)

/// <summary>
/// ��ǻƮ ���̴��� ����ϵ��� �����Ǵ� ������Ʈ ����� �������̽� �Դϴ�.
/// </summary>
class ENGINE_DLL CComputeShaderComp : public CComponent
{
	INFO_CLASS(CComputeShaderComp, CComponent)

private:
	CComputeShaderComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CComputeShaderComp(const CComputeShaderComp& rhs);
	virtual ~CComputeShaderComp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(const wstring& strShaderFilePath, const _char* pEntryPoint);
	virtual HRESULT Initialize(void* pArg) override;


public:	// �ܺο� �Լ�
	// ����Ʈ �ҷ�����
	HRESULT Link_Shader(const wstring& strEffectFileName, const _char* pEntryPoint, const D3D_SHADER_MACRO* pShaderMacro = nullptr);
	// ����Ʈ Ǯ��
	HRESULT Unlink_Shader();
	// �ܺο��� 
	HRESULT	IsShader_Ready();

private:
	// ���������� ���̴� ���̴� �Ŵ��� ��ũ �Լ�. �̸� ���� ���������� ���̴��� ����ϰ� �����ϴ� ������ �Ѵ�.
	HRESULT Link_ToShaderManager();

private:
	class CShader_Manager* m_pShaderManager = { nullptr };


public:
	// �����ε� ����� ������
	static shared_ptr<CComputeShaderComp>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	// ���� ����� ������
	static shared_ptr<CComputeShaderComp>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strShaderFilePath, const _char* pEntryPoint);
	virtual shared_ptr<CComponent>			Clone(void* pArg) override;
	virtual void Free() override;

public:
	const ComPtr<ID3D11ComputeShader>& CRef_Shader() const { return m_pShader; }

private:
	ComPtr<ID3D11ComputeShader>		m_pShader = { nullptr };

};

END