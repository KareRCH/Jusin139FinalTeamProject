#pragma once

#include "Component.h"

#include "BoneAnimContainer.h"
#include "SkeletalComponent.h"
#include "AnimNotify.h"
#include "AnimNotifyComp.h"

#include "Utility/LogicDeviceBasic.h"

BEGIN(Engine)

class CBoneAnimGroup;
class FBoneAnimData;
class FSkeletalData;
class FModelData;
class CBoneGroup;
class CComputeShaderComp;

enum ERootAnimType
{
	None, X, Y, Z, XY, YZ, ZX, XYZ
};

struct TAnimData
{
	_uint		iAnimID;
	string		strAnimName;
	_float		fDuration;
	_float		fTickPerSeconds;
	_float		fTrackPos;
	_float		fPrevTrackPos;
	_float		fSpeedMultiply;
	_bool		bIsLoop;
	_bool		bIsReverse;
	_bool		bIsTransition;
	ERootAnimType	eRootMotion_TransApplyType;	// ��Ʈ��� ���� Ÿ��, ���õ� �ɷ� ���� ������ �޶���
	wstring			strStartBoneName;		// �� ���� ���ۻ�
	vector<wstring>	strEndBoneNames;		// �� ���� ������
	_uint			iRecursive = { UINT_MAX };
};

/// <summary>
/// �ִϸ��̼ǿ� ���� ó���� ����ϴ� ������Ʈ
/// �� �ϳ��� �����ϴ� 
/// </summary>
class ENGINE_DLL CAnimationComponent final : public CComponent
{
	DERIVED_CLASS(CComponent, CAnimationComponent)
protected:
	explicit CAnimationComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CAnimationComponent(const CAnimationComponent& rhs);
	virtual ~CAnimationComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* Arg = nullptr) override;

public:
	static shared_ptr<CAnimationComponent> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr) override;

protected:
	virtual void	Free() override;

public:
	HRESULT Bind_BoneGroup(CBoneGroup* pBoneGroup);
	HRESULT Load_Animations(const wstring& strModelFilePath);

public:
	CBoneGroup* const		Get_BoneGroup() const { return m_pBoneGroup; }
	CBoneAnimGroup* const	Get_AnimGroup() const { return m_pAnimGroup; }

public:
	const FBoneAnimData* Find_BoneAnim(_uint iAnimID) const;

private:
	CBoneGroup* m_pBoneGroup = { nullptr };
	CBoneAnimGroup* m_pAnimGroup = { nullptr };		// �� �ϳ��� ���� ���� �ִϸ��̼� ������ ������.
	// �� �༮�� �����Ǿ� �־�� ����� �� �� �ִ�.
#pragma region ��ǻƮ ���̴� ������Ʈ + ����
	/* Deprecate, �ִϸ��̼� Ű������ ����� �ǳ� Staging ���� ���� ��� ������ �ذ� ������� ���� ���
public:
	struct TBoneBlenderConstants
	{
		_uint iSize;
		_float fBlend;
		_float fPadding[2];
	};
	// ������ ���� ����
	struct TBoneBlenderBuffers
	{
		// ��� ����
		ComPtr<ID3D11Buffer>				pConstantBuffer;

		// ������ SRV
		ComPtr<ID3D11Buffer>				pInputBuffers[2];	// �Է¿� ����
		ComPtr<ID3D11ShaderResourceView>	pInputSRVs[2];
		ComPtr<ID3D11Buffer>				pOutputBuffer;
		ComPtr<ID3D11UnorderedAccessView>	pOutputUAV;

		// ����� �޴� ������¡ ����
		ComPtr<ID3D11Buffer>				pBackBuffer;
	};

	struct TTransformBuffers
	{
		// ��� ����
		ComPtr<ID3D11Buffer>				pConstantBuffer;

		ComPtr<ID3D11Buffer>				pKeyframeBuffer;
		ComPtr<ID3D11ShaderResourceView>	pKeyframeSRV;
		ComPtr<ID3D11Buffer>				pTransformBuffer;
		ComPtr<ID3D11UnorderedAccessView>	pTransformUAV;

		ComPtr<ID3D11Buffer>				pStagingBuffer;
	};

	struct TMaskerBuffers
	{
		ComPtr<ID3D11Buffer>				pKeyframeBuffer;
		ComPtr<ID3D11ShaderResourceView>	pKeyframeSRV;
	};

private:
	// ��� ���� ��� ���̴� ����
	HRESULT Create_BoneBlenderShader();
	// ��� ���� ��� ���̴� ����
	HRESULT Compute_BoneBlenderShader(vector<FKeyFrameTR>& TargetTransforms, vector<FKeyFrameTR>& BlendTransforms, _float fBlend);
	//
	HRESULT Create_CreateTransformShader();
	// Ʈ������ ����
	HRESULT Compute_CreateTransformShader(vector<_float4x4>& Transforms, vector<FKeyFrameTR>& Keyframes);

private:
	// ���� ��ǻƮ ���̴�
	shared_ptr<CComputeShaderComp>		m_pBoneBlender = { nullptr };
	TBoneBlenderBuffers					m_BoneBlenderBuffers = {};

private:
	// Ʈ������ ��ǻƮ ���̴�
	shared_ptr<CComputeShaderComp>		m_pTransformCreator = { nullptr };
	TTransformBuffers					m_TransformCreateBuffers = {};

private:
	// ����ŷ ��ǻƮ ���̴�
	shared_ptr<CComputeShaderComp>		m_BoneMasker = { nullptr };
	*/
#pragma endregion Deprecated

#pragma region ���� �ִϸ��̼� üŷ�Լ�
public:
	_bool	IsAnimation_Finished();
	_bool	IsAnimation_Finished(wstring strAnimName);
	_bool	IsCurrentAnimation(wstring& strAnimName);

	_bool	IsAnimation_UpTo(_float m_fTrackPos);
	_bool	IsAnimation_Frame_Once(_float m_fTrackPos);
	_bool	IsAnimation_Range(_float fTrackMin, _float fTrackMax);
	_bool	IsAnimation_Transition() const { return m_MainAnim.Cur.bIsTransition && !m_MainAnim.fTransBlend.IsMax(); }
	_bool   IsCurrentAnimation(wstring strAnimName);
#pragma endregion


#pragma region ���� �ִϸ��̼� �����Լ�
public:
	void	Set_Animation(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop,
		_bool bIsTransition = true, _float fTransitionSpeed = 1.0f,
		_bool bReverse = false, _bool bIsRootReset = true,
		ERootAnimType eRootAnimType = ERootAnimType::XYZ);

	void	Set_Animation(const wstring& strAnimName, _float fSpeedMultiply, _bool bIsLoop,
		_bool bIsTransition = true, _float fTransitionSpeed = 1.0f,
		_bool bReverse = false, _bool bIsRootReset = true,
		ERootAnimType eRootAnimType = ERootAnimType::XYZ);

	void	Set_AnimationMaintain(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop,
		_bool bReverse = false, _float fTransitionSpeed = 1.0f,
		_bool bIsRootReset = true);

	void	Tick_AnimTime(_cref_time fTimeDelta);
	void	Set_AnimTrackPos(_int iIndex);
#pragma endregion


#pragma region ���� �ִϸ��̼� ���� ��� �Լ�
public:
	// �ִϸ��̼� ���� ��� �Լ�
	string	Get_PrevAnimName();
	string	Get_CurrentAnimName();
	_float	Get_AnimTrackPos();
	_float	Get_AnimDuration();
#pragma endregion


public:
	// ������Ʈ�� ��ĵ� ȸ���� ����ȴ�.
	void	Invalidate_Animation(_cref_time fTimeDelta);
	//void	Invalidate_AnimationCShader(_cref_time fTimeDelta);
	void	Invalidate_AnimationWithMask(class CAnimMaskComp* pAnimMaskComp);

public:
	struct TAnimPair
	{
		TAnimData	Cur = {};
		TAnimData	Prev = {};
		_float		fTransSpeed = { 0.1f };
		FGauge		fTransBlend = { 1.f };

	};

private:
	// ���� �ִϸ��̼ǿ� ���� 
	TAnimPair			m_MainAnim;


public:
	struct TMaskAnimPair
	{
		TAnimData	Cur = {};
		TAnimData	Prev = {};
		_bool		bUsing = { false };
		_float		fTransSpeed = { 0.1f };
		FGauge		fTransBlend = { 1.f };
		_bool		bEndMasking = { false };
		_float		fFadeOutSpeed = { 1.f };
		FGauge		fFadeOutBlend = { 1.f };
	};

public:
	void	Create_MaskAnim() { m_MaskAnim.emplace_back(TMaskAnimPair()); }
	void	Set_MaskAnim(_uint iIndex, const wstring& strAnimName
		, const wstring& strBasisBone, const vector<wstring>& strEndBoneNames
		, _uint iRecursive = UINT_MAX
		, _float fSpeedMultiply = 1.f, _bool bIsLoop = false
		, _bool bIsTransition = true, _float fTransitionSpeed = 1.0f
		, _bool bReverse = false);
	void	Exit_MaskAnim(_uint iIndex, _float fFadeOutSpeed = 1.f);
	void	Tick_MaskAnimTime(_cref_time fTimeDelta);


private:
	vector<TMaskAnimPair>	m_MaskAnim;


#pragma region ADD �ִϸ��̼�
public:
	struct TAddAnim
	{
		TAnimData	Add = {};
		_bool		bUsing = { false };
		_float		fWeight = { 1.f };
	};

public:
	void	Create_Add_Animation() { m_AddAnims.push_back(TAddAnim()); }
	_uint	Get_NumAdd_Animations() const { return (_uint)m_AddAnims.size(); }
	// �Ϲ� ADD �ִϸ��̼� �����Լ�
	void	Set_ADD_Animation(_uint iIndex, const wstring& strAnimName, const wstring strBasisBone
		, const vector<wstring>& strEndBoneNames, _uint iRecursive = UINT_MAX
		, _float fSpeedMultiply = 1.f, _float fWeight = 1.f, _bool bReverse = false);
	// ���� ����
	void	Set_ADD_AnimFinish(_uint iIndex);

	// ADDŸ�� �ִϸ��̼� ������Ʈ
	void	Tick_ADD_AnimTime(_cref_time fTimeDelta);
	// ADDŸ�� �ִϸ��̼� üŷ�Լ�
	_bool	IsADDAnim_Finished(_uint iIndex) const 
	{ return m_AddAnims[iIndex].Add.fTrackPos >= m_AddAnims[iIndex].Add.fDuration; }

private:
	_bool	IsADDAnim_Using(_uint iIndex) const { return m_AddAnims[iIndex].bUsing; }
#pragma endregion


private:
	// ADD �ִϸ��̼�
	vector<TAddAnim>	m_AddAnims = {};
	

	// ��Ʈ ��� ����
public:
	// Translate, Scale ����
	_vector			Apply_TransDeltaByRootAnimType(ERootAnimType eType, _fvector vTranslate);

	_float3			Get_Root_TransDelta() const { return m_vRootTransDelta; }
	_vector			Get_Root_TransDeltaVector() const { return m_vRootTransDelta; }
	_float4			Get_Root_RotateDelta() const { return m_vRootRotateDelta; }
	_vector			Get_Root_RotateDeltaVector() const { return m_vRootRotateDelta; }
	_float3			Get_Root_ScaleDelta() const { return m_vRootScaleDelta; }
	_vector			Get_Root_ScaleDeltaVector() const { return m_vRootScaleDelta; }
	
	_float3			Get_Root_Trans() const { return m_vRootTrans; }

private:
	void			RootReset() { m_vRootTransDelta = {}, m_vRootTrans = {}; m_vRootTrans_Prev = {}; }

private:
	_float3				m_vRootTransDelta = {};			// �̵���
	_float4				m_vRootRotateDelta = {};		// ȸ����
	_float3				m_vRootScaleDelta = {};			// ������ ��

	// ����
	_float3				m_vRootTrans = {};	// ����
	_float4				m_vRootRotate = {};
	_float3				m_vRootScale = {};

	_float3				m_vRootTrans_Prev = {};	// ����
	_float4				m_vRootRotate_Prev = {};
	_float3				m_vRootScale_Prev = {};

public:
	using SNotifyCollections = map<const string, shared_ptr<CAnimNotifyCollection>>;

public:
	HRESULT Clone_NotifyFromPrototype(const wstring& strProtoTag);
	// ��Ƽ���� �̺�Ʈ ���
	HRESULT Regist_EventToNotify(const string& strAnimName, const string& strCollectionName, 
										const string& strNotifyName, FastDelegate0<> Event);
	// ��Ƽ���̿� �� ������Ʈ ���
	HRESULT Regist_ModelCompToNotify(weak_ptr<CCommonModelComp> pModelComp);

private:
	// ��Ƽ���� �۵� �Լ�
	void Trigger_AnimNotifyByCurAnim(_cref_time fTrackPos);
	// ��Ƽ���� ���� �Լ�
	void ResetTrigger_AnimNotifyByCurAnim();

	void Set_AnimNameToNotify();

private:
	shared_ptr<CAnimNotifyComp>	m_pNotifyComp = { nullptr };	// ��Ƽ���� �����
};

END