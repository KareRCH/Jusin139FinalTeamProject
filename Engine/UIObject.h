#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	enum UI_PROJ { UI_ORTHO, UI_PERS, UI_PERS_FULL, UI_PROJ_END }; //����, ����, 3D

	typedef struct tagUIMakeDesc : public CGameObject::GAMEOBJECT_DESC //UI ���� �� �ʿ��� Desc
	{
		_float3			fPos, fSize, fRot = { 0.f, 0.f, 0.f };
		_int			iProjType;
		_int			iRenderType;
		string			strFilePath;
		string			strFileName; //For Category
		enum DirType { DIR_0, DIR_90, DIR_180, DIR_270, DIR_END };
		DirType			eDirType;
		string			strNameLabel; //For Trigger

	}UI_Make_DESC;

	enum RENDERTYPE { EMERGENCE_RENDER = -1, PRIORITY_RENDER, MAIN_RENDER, LATE_RENDER, VERY_LATE_RENDER, END_RENDER };

	typedef struct	tagMotionUI_Data {  //��� ����Ʈ�� ������
		XMFLOAT3		fScale;
		XMFLOAT3		fRotation;
		XMFLOAT3		fPos;
		float			fAlpha;
		float			fKeyFrame;
	}UI_MOTION_DESC;

	typedef struct tagUIMotionData {
		string					strMotionName;
		vector<UI_MOTION_DESC>  vecMotionData;
	}UI_MOTIONS;


protected:
	CUIObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUIObject(const CGameObject& rhs);
	virtual ~CUIObject() = default;

public:
	_float					Get_CamDistance() const { return m_fCamDistance; }

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual void			Priority_Tick(_cref_time fTimeDelta);
	virtual void			Tick(_cref_time fTimeDelta);
	virtual void			Late_Tick(_cref_time fTimeDelta);
	virtual HRESULT			Render();

	/*-------------Motion-------------*/
	void					Set_Motion_ALL_Data(vector<UI_MOTION_DESC>& _MotionData, _int _MotionPointMax, _float _Duration, _bool _isLoop);
	void					Set_Motion_ALL_Data(vector<UI_MOTION_DESC>& _MotionData, MOTION_SETTING_DESC _MotionSettingData);
	MOTION_SETTING_DESC		Get_Motion_Set_Data();
	vector<UI_MOTION_DESC>&	Get_vecMotionData() { return m_vecMotionData; }
	vector<UI_MOTIONS>&		Get_ALLMotions() { return m_vecALLMotions; }

	void					Set_Motion_Loop(_bool _bLoop) { m_MotionSettingDesc.bisLoop = _bLoop; }
	void					Set_Current_Motion_Play(_int _MotionindexNum = 0);
	void					Set_Motion_Play()		{ m_bisPlay = true; }
	void					Set_Motion_Pause()		{ m_bisPlay = false; }
	void					Set_Motion_Stop()		{ m_bisPlay = false; m_MotionSettingDesc.fTrackPosition = 0.f; }
	void					Set_Motion_Clear();
	 
	void					Input_Motion_Data(string _MotionName, vector<UI_MOTION_DESC>& _MotionData, MOTION_SETTING_DESC& _SettingDesc);
	void					Update_Motion_Scale(_float3 _MotionScale);

	_uint					Get_Current_MotionPoint_Index() const;
	_uint					Get_Current_Motion_Index() const { return m_iCurrentPlayIdx; }

	_bool					Check_UIMotion_End();
	_bool					ALLMotionEndCheck(vector<shared_ptr<CGameObject>>& _vecUI);

protected:
	void					Play_UI_Motion(_cref_time fTimeDelta);
	void					Blend_UI_Motion(_cref_time fTimeDelta);

public:
	/*--------------Other---------------*/
	void					BillBoard(_float4* _CamPos = nullptr);

public:
	GETSET_EX2(float, m_fAlpha, fAlpha, GET, SET)
	GETSET_EX2(bool, m_bisWriteZDepth, UI_Write_ZDepth, GET, SET)
	_int					Get_RenderType() { return m_iRenderType; }
	_int					Get_ProjType() { return m_iProjType; }

	_float					Get_ZDepth() { return m_fPos.z; }
	void					Update_SRP_Data();

	void					Set_Pos_MotionObject(_float3 _NewPos, vector<shared_ptr<CGameObject>>& _vecUI);
	void					Set_Pos_Y_MotionObject(_float _MovePosY, vector<shared_ptr<CGameObject>>& _vecUI);

	void					Make_UV_Cutting(_float _NowNum, _float _MaxNum, _bool _bisWidthCut = true); //true ���� �ڸ��� fals ���� �ڸ���
	void					Make_UV_Cutting(_float fRatio, _bool _bisWidthCut = true);
	void					Make_UV_HPCutting(_float _NowNum, _float _MaxNum, _bool _bisWidthCut = true);
	void					Make_UV_HPCutting(_float _NowNum, _float _MoveNum, _float _MaxNum, _bool _bisWidthCut = true);

	/*--------------------------------*/


	template<typename T>
	void Set_State_(T _eState, T& state, bool& flag) {
		state = _eState;
		flag = true;
	}

protected:
	const bool Get_StateEnter(bool& flag) {
		if (flag) {
			flag = false;
			return true;
		}
		return false;
	}

protected:
	MOTION_SETTING_DESC		m_MotionSettingDesc;		//��� ���ÿ� �ʿ��� ������
	vector<UI_MOTION_DESC>	m_vecMotionData = {};		//��� ����Ʈ�� ������
	
	//���� ��ǿ�
	vector<MOTION_SETTING_DESC> m_vecALLMotionSettings = {};
	vector<UI_MOTIONS>			m_vecALLMotions = {};

	_int					m_iMotionCnt = { 0 };  //��� �� ��?
	_int					m_iCurrentPlayIdx = { -1 }; //���� �ε���
	_int					m_iPrePlayIdx = { -1 }; //���� �ε���
	
	//��� �÷��̿�
	_bool					m_bisWriteZDepth = { false };
	_bool					m_bisPlay = { true };		
	_bool					m_bisEnd = { false };

	_float3					m_fScale;
	_float3					m_fRotation;
	_float3					m_fPos;
	_float					m_fAlpha = { 1.f };

	//��ü�� ���� ����
	string				m_strNameLabel = "";  //For Trigger
	_int				m_iRenderType = { (_int)MAIN_RENDER };
	_int				m_iProjType = { 0 };
	_float4x4			m_WorldMatrix;
	_float4x4			m_ViewMatrix, m_ProjMatrix;

protected:
	//UI ������ Scale ������Ʈ�� (Transform�� ����X)
	_bool				m_bScaleSet = { false };
	_float3				m_fScaled = {};
	_float4x4			m_ScaleMatirx = {};

	//UV Cutting ������Ʈ��
	_bool				m_bisUVCutting = { false }; //�Ϲ� ����
	_bool				m_bisUVHPCutting = { false }; //HP Ŀ��
	_bool				m_bisWidthCutting = { true }; //true ���� �ڸ��� fals ���� �ڸ���
	_float				m_fUvCuttingRatio = { 1.f };
	_float				m_fUVCuttingRatio2 = { 100.f };

	_float				m_fCamDistance = { 0.0f };
		

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
