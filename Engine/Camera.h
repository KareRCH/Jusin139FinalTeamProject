#pragma once

/* Ŭ���̾�Ʈ �����ڰ� ��������ϴ� ī�޶���� �������� �θ� �Ǳ����� Ŭ������. */
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	enum CAMERA_OFFSET_CATEGORY { //Pos�� ��� �������� Look�� ��� ��������
		OFFSET_POS, OFFSET_LOOK, OFFSET_ALL, OFFSET_NONE, OFFSET_END
	};										//�Ϲ� ����

	enum CAMERA_OFFSET_MAG { //���� �����ϴ� UI ���߿� �ֱ�
		OFFMAG_NEAR, OFFMAG_MIDDLE, OFFMAG_FAR, OFFMAG_END
	}; //   Ÿ���� �ִ� ��� Ȱ���ϴ� ������



	typedef struct tagCameraDesc : public tagGameObjectDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;
		_float		fMouseSensor = 0.0f;  // For Dynamic,Third
	}CAMERA_DESC;

	typedef struct tagCameraMotionData {
		enum INTERPOLATION_TYPE { INTER_LERP, INTER_CATROM, INTER_EASING, INTER_NONE, INTER_END };

		INTERPOLATION_TYPE	eType;
		int					eEasingType;		//Easing ��� ���� Ÿ��
		XMFLOAT4			vCamEyePos;
		XMFLOAT4			vCamLook;
		//XMFLOAT4			vCamRotate;
		float				fDuration;			//��� �� ����

		float				fKeyFrame;
		bool				bStarted = false;	//���� ������ üũ��
		bool				bFollow = false;    //ĳ���� ���󰡴��� �ƴ���

		//XMFLOAT4X4			WorldMatrix;
		shared_ptr<CTransform> pActorTransformCom;

	}CAMERA_MOTION_DESC;


protected:
	CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;

public:
	void		Apply_CameraView();
	void		Set_CameraMain(_bool bMainTrueOrFalse) { m_bisNowMain = bMainTrueOrFalse; }
	_float4		Get_CamLookPoint();

public:
	void		Chainging_Camera_Set(_float4 _PreCamPos, _float4 _PreCamLook, _float _Duration, _int eInterType);
	void		Chainging_Camera(_cref_time fTimeDelta);

	void		Offset_Setting(_cref_time fTimeDelta);
	void		Setting_Offset(CAMERA_OFFSET_CATEGORY eCategory, CAMERA_OFFSET_MAG eMag) {
		m_eOffSetCategory = eCategory;
		m_eOffSetMagnification = eMag;
		m_vOffset = m_vOffsetValue[m_eOffSetMagnification];
	}
	CAMERA_OFFSET_CATEGORY Get_OffsetCategory() { return m_eOffSetCategory; }

protected:
	_float				m_fFovy = { 0.f };
	_float				m_fAspect = { 0.0f };
	_float				m_fNear = { 0.0f };
	_float				m_fFar = { 0.0f };

	/*---------Base Setting--------*/
	_int				m_iINTERType = { 0 }; //���� ���
	_int				m_iEasingType = { 0 };
	_bool				m_bisNowMain = { false };	//������ ���� ī�޶�����
	_bool				m_bisChanging = { false };  //ī�޶� ��ȯ'
public:
	GETSET_EX2(_bool, m_bisChanging, IsChanging, GET, SET)
protected:
	/*--------Target Offset---------*/
	//����
	//_float4x4				m_WorldMatrix;
	shared_ptr<CTransform>	m_pActorTransformCom; //��ü

	CAMERA_OFFSET_CATEGORY  m_eOffSetCategory = { OFFSET_NONE };
	CAMERA_OFFSET_MAG		m_eOffSetMagnification = { OFFMAG_MIDDLE };

	//None�� �ƴ� ���
	_float3				m_vOffset = {};
	_float3				m_vOffsetValue[3] = { { 0.f, 1.5f, -2.0f } , { 0.f, 2.0f, -2.5f }, { 0.f, 3.0f, -3.5f } };

	/*--------Motion---------*/
	_float				m_fTrackPosition = { 0.f };
	_float				m_fDuration = { 5.f };

	/*--------Change ������---------*/
	_float4				m_vNowCamPos = { 0.f, 0.f, 0.f, 1.f };
	_float4				m_vNowLook = { 0.f, 0.f, 0.f, 0.f };

	_float4				m_vChangeCamPos = { 0.f, 0.f, 0.f, 1.f };
	_float4				m_vChangeLook = { 0.f, 0.f, 0.f, 0.f };


	_float				m_fMouseSensor = { 0.0f };
public:
	GETSET_EX1(_float, m_fFovy, Fovy, GET)
	GETSET_EX1(_float, m_fAspect, Aspect, GET)
	GETSET_EX1(_float, m_fNear, Near, GET)
	GETSET_EX1(_float, m_fFar, Far, GET)


public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END