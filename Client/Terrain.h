#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
	INFO_CLASS(CTerrain, CGameObject)

	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };


private:
	CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CTexture> m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* ������ü�� �����Ѵ�. */
	static shared_ptr<CTerrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

	/* �纻��ü�� �����Ѵ�. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END