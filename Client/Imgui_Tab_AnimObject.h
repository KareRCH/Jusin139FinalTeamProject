#ifdef _DEBUG
#pragma once

#include "Imgui_Tab.h"


BEGIN(Client)

/// <summary>
/// �𵨰� �ִϸ��̼� ����Ʈ�� ����
/// ������ �𵨰� �ִϸ��̼��� �����Ѵ�.
/// </summary>
class CImgui_Tab_AnimObject : public CImgui_Tab
{
	INFO_CLASS(CImgui_Tab_AnimObject, CImgui_Tab)

private:
	CImgui_Tab_AnimObject(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_AnimObject() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	static CImgui_Tab_AnimObject* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

public:
	// ���� �����Ϳ� �ҷ�����
	void Load_ModelObjectToEditor();

	// �� ������Ʈ�� �����Ϳ��� ����
	void Delete_ModelObjectFromEditor();

	void TogglePlay_AnimObject();
	void Play_AnimObject();
	void Pause_AnimObject();
	void Reset_AnimObject();
	void Set_TrackPos_AnimObject(_int iIndex);
	_int Get_TrackPos_AnimObject();
	_int Get_Duration_AnimObject();

	shared_ptr<class CAnimObject>& Get_AnimObject() { return m_pAnimObject; }

private:
	shared_ptr<class CAnimObject>		m_pAnimObject = { nullptr };


public:
	struct TRecorder
	{
		_int Cur;
		_int Prev;
	};

	// �� ����Ʈ �ڽ�
private:
	TRecorder	m_iModelSelect_Index = { -1, -1 };
	wstring		m_ModelSelect_Name = TEXT("");

public:
	void Change_Animation();

private:
	TRecorder	m_iAnimSelect_Index = { -1, -1 };
	wstring		m_AnimSelect_Name = { TEXT("") };


private:
	_int	m_iBoneSelect_index = { -1 };
};

END
#endif // DEBUG
