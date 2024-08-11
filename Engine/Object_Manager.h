#pragma once

#include "Base.h"
#include "Layer.h"
/* ������ ���� ��ü�� ��Ƴ��´�. (������Ÿ��) */
/* ������ü�� �����Ͽ� ������ �纻��ü�� ���̾�� �����Ͽ� ��Ƴ��´�. */
/* �����ϰ� �ִ� �纻��ü���� Tick���� �Լ��� �ݺ������� ȣ�����ش�.  */



BEGIN(Engine)

class CComponent;

class ENGINE_DLL CObject_Manager final : public CBase
{
	INFO_CLASS(CObject_Manager, CBase)

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	void Priority_Tick(_cref_time fTimeDelta);
	void Tick(_cref_time fTimeDelta);
	void Late_Tick(_cref_time fTimeDelta);
	void Before_Render(_cref_time fTimeDelta);
	void Clear(_uint iLevelIndex);

	HRESULT Add_Prototype(const wstring& strPrototypeTag,  shared_ptr<class CGameObject> pPrototype);

	template<class T>
	HRESULT Add_CloneObject(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strPrototypeTag, void* pArg, shared_ptr<T>* ppOut = nullptr);

	HRESULT Add_Object(_uint iLevelIndex, const LAYERTYPE& strLayerTag, shared_ptr<CGameObject> pObject);
	shared_ptr<CGameObject> CloneObject(const wstring& strPrototypeTag, void* pArg);

	shared_ptr<CGameObject> Get_Object(_uint iLevelIndex, const LAYERTYPE& strLayerTag, _uint iIndex);
	shared_ptr<CComponent> Get_Component(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strComponentTag, _uint iIndex, const wstring& strPartTag);

private:
	_uint			m_iNumLevels = { 0 };

private:
	map<const wstring, class shared_ptr<CGameObject>>				m_Prototypes;
	map<const LAYERTYPE, class CLayer*>*				m_pLayers = { nullptr } ;
	typedef map<const LAYERTYPE, class CLayer*>		LAYERS;

public:
	map<const wstring,  shared_ptr<class CGameObject>>* Get_PrototypeList() { return &m_Prototypes; }
	list<shared_ptr<class CGameObject>>*				Get_ObjectList(_uint iLevelIndex, const LAYERTYPE& strLayerTag);

private:
	class shared_ptr<CGameObject> Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const LAYERTYPE& strLayerTag);
public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};


template<class T>
HRESULT CObject_Manager::Add_CloneObject(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strPrototypeTag, void* pArg, shared_ptr<T>* ppOut)
{
	/* ������ ã��. */
	shared_ptr<CGameObject>		pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		RETURN_EFAIL;

	/* ������ �����Ͽ� ���� ���ӳ��� ����� �纻 ��ü�� �����س���.  */
	shared_ptr<CGameObject>		pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		RETURN_EFAIL;

	pGameObject->Set_PrototypeTag(strPrototypeTag);

	/* ���� �纻��ü�� �߰��ؾ��� ���̾ ã��. */
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* ���� �ش� �̸��� ���� ���̾ ������. */
	/* �� �̸��� ���� ���̾ ���ʷ� �߰��ϰ� �ִ� ��Ȳ�̴�. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			RETURN_EFAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	/* �̹� �̸��� ���� ���̾ �־���. */
	else
		pLayer->Add_GameObject(pGameObject);

	if (ppOut != nullptr)
		*ppOut = static_pointer_cast<T>(pGameObject);

	return S_OK;
}

END