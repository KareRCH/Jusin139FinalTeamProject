#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"
#include "Easing.h"

#include "Utility_File.h"

BEGIN(Engine)

//class CEasing;
class CBoneGroup;
END

BEGIN(Client)

class CEffect abstract  : public CBlendObject
{
public:
	enum DEADTYPE {TIME,SCALE,COLOR,RANGE,OWNER,TYPE_END};
	enum USE_TYPE {
		USE_NONE = 0,				// 단발성 이펙트
		USE_FOLLOW_NORMAL = 1,		// 상시 이펙트로 일반 적인 객체가 오너
		USE_FOLLOW_PARTS = 2,		// 상시 이펙트로 파트 오브젝트가 오너
		USE_FOLLOW_EFFECT = 3,		// 상시 이펙트로 이펙트가 오너
		USE_TYPE_END};

	enum COLORBLEND {COLOR_MULTIPLY,COLOR_SCREEN,COLOR_OVERLAY,COLOR_PLUS,COLOR_BURN};

	typedef struct tagEffect_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iShaderPassType = {0};

		_float fDiscard_Alpha = {0.f};

		wstring strDiffuse;
		wstring strMask;
		wstring strNoise;

		_float2 vDiffuseUV = {0.f,0.f};
		_float2 vMaskUV = { 0.f,0.f };
		_float2 vNoiseUV = { 0.f,0.f };

		_float2 vSize = { 1.f,1.f };

		_float3 vRotation = {0.f,0.f,0.f};

		_float4 vSolid_Color = {1.f,1.f,1.f,1.f};

		// ------ Sprite ---------

		_bool bSpriteAnim = { false };	// 스프라이트 유무

		_bool	bLoop = { true };		// 루프 유무
		_float	fSpeed = { 1.f };		// 프레임 속도 제어

		_float	fDuration = { 0.1f };	// 다음 프레임으로 넘어갈 시간 값

		_uint			iSpriteType = {0};	// 스프라이트를 사용하는 텍스쳐가 Diffuse 또는 Mask
		
		_float2			vSpriteColRow = {1.f,1.f}; // 크기 즉 행으로 몇개, 열로 몇개

		// ----- Distortion ---------
		_float3	vDistortionValue = {1.f,1.f,1.f}; // 노이즈 텍스쳐의 배율을 각각 늘리기 위함 총 3개
		_float	fDistortionScale = {0.01f};		// 왜곡 배율
		_float	fDistortionBias = {0.01f};		// 왜곡
		_float	fNoiseWeight = {1.f};	// 노이즈 텍스쳐에 들어가는 시간값의 가중치

		// ------- Easing --------
		_bool	bEasing = { false };
		_uint	eEasingType = { 0 };	// Easing함수를 사용하기 위한 변수들

		// -------- DeadType ------
		DEADTYPE eDeadType = { DEADTYPE::TYPE_END}; // 죽음 조건 타입

		// ------ Owner -------
		USE_TYPE eUseType = { USE_NONE }; // 오너의 타입에 따라 월드행렬 계산식이 달라진다.

		// ------ ColorBlendType --------
		COLORBLEND eColorBlendType = { COLORBLEND::COLOR_MULTIPLY};

		// ------ SolidColor -------- 
		_bool		bSolidColor = {false}; //쉐이더에서 단색 렌더타겟을 사용할지 유무

		// ------ ObjPool ----------
		//_uint		iObjPool_MaxNum = { 50.f }; // 오브젝트 풀에서 생성할 때, 클론 최대 갯수.

	}EFFECT_DESC;

protected:
	CEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(string strFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Begin_Play(_cref_time fTimeDelta) override;
	virtual void Priority_Tick(_cref_time fTimeDelta) override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual void Before_Render(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

	void Reset_Frame() {
		m_fFrame = 0; m_bIsFinished = false;
		m_vSpriteIndex = _float2(0.f, 0.f);
	} // 스프라이트 애니메이션 리셋

public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);
	virtual	void Effect_Write_Json(json& Out_Json, EFFECT_DESC eDesc);
	virtual	void Effect_Load_Json(const json& In_Json, EFFECT_DESC* eDesc);

public:
	virtual	void	Reset_Effect(_bool bActivate = false) {};
	// 리셋 기능을 묶어주려고

	virtual	void	Reset_Prototype_Data() {};

public:
	GETSET_EX2(_bool, m_bParticleDead, IsEffectDead, GET, SET)
	GETSET_EX2(_bool, m_bApplyTransform, ApplyTransform, GET, SET)
	GETSET_EX2(_float4x4, m_matOneTImeWorld, OneTImeWorld, GET, SET)
	GETSET_EX2(weak_ptr<class CTransform>, m_pOwnerTransformCom, OwnerTransformCom, GET, SET)

	GETSET_EX2(CBoneGroup*, m_pSocketBoneGroup, SocketBoneGroup, GET, SET)
	GETSET_EX2(_uint, m_iSocketBoneIndex, SocketBoneIndex, GET, SET)
	GETSET_EX2(_uint, m_iObjPool_MaxNum, ObjPool_MaxNum, GET, SET)
	

protected:
	_bool	m_bLoad = { false };
	_float	m_fTimeDelta = { 0.f };

protected:
	_float			m_fFrame = { 0.f };		// 프레임 계산을 위한 값
	_float2			m_vSpriteIndex = { 0.f,0.f }; // 현재 스프라이트(즉 UV), 프레임
	_bool			m_bIsFinished = { false };// 애니메이션을 모두 재생 후 루프가 아니면 끝남을 알림

protected:
	_bool			m_bParticleDead = { false }; // 이펙트가 죽음 조건을 충족했는지 판단.
	
protected:
		_float4x4	m_matJsonWorld;	// 로드용 월드행렬
		_float4x4	m_matOneTImeWorld; // 단발성 행렬
	
protected:
	weak_ptr<class CTransform>	m_pOwnerTransformCom; // 이펙트 오너를 받아오게 한다.

protected:
	_bool			m_bApplyTransform = { true }; // 툴 상에서 이펙트 전용 변수가 아닌. 트랜스폼 월드행렬 사용유무

protected:
	//wstring			m_strSocketBoneName; // 오너의 뼈 이름.
	CBoneGroup*			m_pSocketBoneGroup = { nullptr }; // 오너의 뼈 그룹.
	_uint				m_iSocketBoneIndex = { 0 }; // 선택한 뼈의 인덱스.

protected:
	_uint				m_iObjPool_MaxNum = { 200 }; // 오브젝트 풀 클론 생성 갯수.

#pragma region 오프셋 행렬

protected:
	shared_ptr<class CTransform>	m_pOffsetTransform = { nullptr }; // 오프셋 행렬 트랜스폼.

public:
	_bool		m_bUseOffsetTransform = { false };

	weak_ptr<class CTransform> Get_OffsetTransform() { return m_pOffsetTransform; }
	GETSET_EX2(_bool, m_bUseOffsetTransform, UseOffsetTransform, GET, SET)

#pragma endregion

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources(EFFECT_DESC EffectDesc);

protected:
	void	SpriteAnim(_cref_time fTimeDelta, EFFECT_DESC EffectDesc);
	virtual void	Judge_Dead(EFFECT_DESC EffectDesc); // 죽음을 판단
	void	UpdateWorldMatrix(EFFECT_DESC EffectDesc); // 월드행렬을 오너의 기준에 맞게 계산해주는 함수

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

/*
	
	- 이펙트는 기본적으로 m_matWorld라는 행렬 변수를 통한다.

	- Transform의 월드행렬은 로컬 행렬로 사용한다.
		- 로컬영역에서 각도, 위치등을 수정할 때만 사용.
	- m_matWorld 변수는 월드행렬로 사용한다.
		- 쉐이더로 바인딩시키는 월드행렬

*/