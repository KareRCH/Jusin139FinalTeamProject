#pragma once

#include "SceneComponent.h"
#include "MeshComp.h"
#include "MaterialComponent.h"
#include "SkeletalComponent.h"
#include "AnimationComponent.h"
#include "Shader.h"

BEGIN(Engine)


/// <summary>
/// �п� �����ӿ�ũ�� ���缭 ���۵� ���� �� ������Ʈ
/// �̸� ������� ���� �𵨷ε� �Ŵ��� ������ �� ����
/// 
/// �� �Ŵ����� ��ŵ� ������ ���·� ���۵ȴ�.
/// �� ������Ʈ���� �� �ε� ����� ����.
/// �̴� ������Ÿ�����ε� ���� ���� �� �� �ְ� �ϱ� ������.
/// </summary>
class ENGINE_DLL CCommonModelComp final : public CSceneComponent
{
	DERIVED_CLASS(CSceneComponent, CCommonModelComp)

public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
	enum class ERootMotionType { Trans, Rotate, Scale };

public:
	struct COMMON_MODEL_DESC
	{
		TYPE	eModelType;
		wstring strModelFilePath;
		wstring strShaderFilePath;
		const D3D11_INPUT_ELEMENT_DESC* pElements;
		_uint iNumElements;
	};

protected:
	explicit CCommonModelComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CCommonModelComp(const CCommonModelComp& rhs);
	virtual ~CCommonModelComp() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize_Prototype(COMMON_MODEL_DESC& tDesc);
	virtual HRESULT Initialize(void* Arg = nullptr) override;
	virtual void	Priority_Tick(const _float& fTimeDelta);
	virtual void	Tick(const _float& fTimeDelta);
	virtual void	Late_Tick(const _float& fTimeDelta);
	virtual HRESULT	Render();

public:
	static shared_ptr<CCommonModelComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static shared_ptr<CCommonModelComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, COMMON_MODEL_DESC& tDesc);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr);

protected:
	virtual void	Free() override;



#pragma region �� ��ŷ
public:
	// �Ŵ����κ��� ���� ��ũ�ϴ� ����� ������.
	HRESULT Link_Model(TYPE eType, const wstring& strModelFilePath);
	HRESULT Link_Model(const wstring& strModelFilePath) { return Link_Model(m_eModelType, strModelFilePath); }
	// �޽���ũ ����, �� ��ũ ���� �Ŵ� ��� �� �Լ��� ������.
	HRESULT Link_Model(TYPE eType, const wstring& strMeshMatrialModel, const wstring& strBoneAnimModel);
	void	Unlink_Model();

	_bool	Intersect_MousePos(SPACETYPE eSpacetype, _float3* pOut, _matrix matWorld, _float* pLengthOut = nullptr);
#pragma endregion




#pragma region �ִϸ��̼� ����
	// �ִϸ��̼� ���� �Լ�
public:
	// �ִϸ��̼� ����
	void Set_Animation(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop, _bool bIsTransition = true, _float fTransitionSpeed = 1.0f, _bool bReverse = false, _bool bIsRootReset = true);

	// �ִϸ��̼� �̸����� ����
	void Set_Animation(const string& strAnimName, _float fSpeedMultiply, _bool bIsLoop, _bool bIsTransition = true, _float fTransitionSpeed = 1.0f, _bool bReverse = false, _bool bIsRootReset = true);

	// �ִϸ��̼� ���� Ű ������ �������
	void Set_AnimationMaintain(_uint iAnimIndex, _float fSpeedMultiply, _bool bIsLoop, _bool bReverse = false, _float fTransitionSpeed = 0.1f, _bool bIsResetRootMotion = false);

	// �ִϸ��̼� ���, �� ������Ʈ, ��� ������Ʈ
	void Tick_AnimTime(const _float& fTimeDelta);

	void Apply_RootMotion_To_GameObject();

	//���� �ִϸ��̼� �̸� �޾ƿ���
	string Get_CurrentAnimationName();
	string Get_PrevAnimationName();

	// ��Ʈ ��� �̵��� ���
	_float3 Get_RootTransDelta_Float3();
	_vector Get_RootTransDelta_Vector();

	// �ִϸ��̼� ������Ʈ ���
	shared_ptr<CAnimationComponent> Get_AnimationComponent() { return m_pAnimationComp; }

	_float3 Get_RootTrans();

	// �ִϸ��̼� ���� Ȯ��
public:
	_bool	IsAnimation_Finished();
	_bool	IsAnimation_Finished(string strAnimName);
	_bool	IsAnimation_UpTo(_float m_fTrackPos);
	_bool	IsAnimation_Frame_Once(_float m_fTrackPos);
	_bool	IsAnimation_Range(_float fTrackMin, _float fTrackMax);
	_bool	IsAnimation_Transition() const;
	_bool	IsCurrentAnimation(string strAnimName);

#pragma endregion



private:
	// �� �Ŵ����� ����
	HRESULT Link_ToModelManager();

public:
	const TYPE& Get_ModelType() const { return m_eModelType; }

private:
	// �����͵��� ��� ������Ʈ ���·� ������. ������ �ٸ� �κ�
	TYPE	m_eModelType = { TYPE_END };
	wstring m_strModelPath = TEXT("");		// ������Ÿ�Կ� ����
	wstring m_strEffectPath = TEXT("");		// ������Ÿ�Կ� ����
public:
	wstring Get_ModelPath() { return m_strModelPath; }


#pragma region �޽� ������Ʈ, ���
public:
	// �޽� ���� Ȱ��ȭ
	void Active_Mesh(_uint iIndex);
	// ��ü �޽� ���� Ȱ��ȭ
	void Active_AllMeshes();
	// �޽� ��Ȱ��ȭ
	void Deactive_Mesh(_uint iIndex);
	// ��ü �޽� ��Ȱ��ȭ
	void Deactive_AllMeshes();
	// �ܺ� ���� ���ε��
	HRESULT BindAndRender_Mesh(_uint iIndex);
	// Ȱ��ȭ�� �޽� ������ ���� �� �ִ�.
	const vector<_uint>& Get_ActiveMeshes() const { return m_ActiveMeshes; }
	const vector<shared_ptr<CMeshComp>>& Get_MeshComps() const { return m_pMeshComps; }

private:		// �޽� ����
	_uint								m_iNumMeshes = { 0 };
	vector<shared_ptr<CMeshComp>>		m_pMeshComps;
	_uint								m_iNumActiveMeshes = { 0 };
	vector<_uint>						m_ActiveMeshes;
#pragma endregion




#pragma region ���͸���
public:
	// ���ϴ� �޽��� ���͸��� ����� �ؽ�ó�� ���̴��� ���ε��Ѵ�.
	HRESULT Bind_MeshMaterialToShader(_uint iMeshIndex, TEXTURETYPE eType, const _char* pConstName);

private:		// ���͸��� ����
	_uint									m_iNumMaterials = { 0 };
	vector<shared_ptr<CMaterialComponent>>	m_pMaterialComps;
#pragma endregion



#pragma region ���̷�Ż ������Ʈ(��)
public:
	_float4x4	Get_BoneTransformFloat4x4(const wstring& strBoneName);
	_matrix		Get_BoneTransformMatrix(const wstring& strBoneName);
	_float4x4	Get_BoneTransformFloat4x4WithParents(_uint iIndex);
	_matrix		Get_BoneTransformMatrixWithParents(_uint iIndex);
	_matrix		Get_BoneTransformMatrixWithParents(const wstring& strBoneName);
	CBoneGroup* Get_BoneGroup() const;
	HRESULT		Bind_BoneToShader(_uint iMeshIndex, const _char* pBoneConstantName);
	void		Set_PreRotate(_matrix Matrix);
	// ���� 1���� ��κ� ��Ʈ���̾���
	void		Set_RootTransBone(_uint iIndex = 1);
	// ���ϴ� ���� Ʈ���� ���� ���� �ϵ��� �ٲ� �� �ִ�.
	void		Set_RootTransBone(const wstring& strBoneName);

private:		// �� ����
	// �� �ϳ��� ������ ����� ������Ʈ ������, �𸮾� �� ���Դ�.
	shared_ptr<CSkeletalComponent> m_pSkeletalComp = { nullptr };
#pragma endregion




#pragma region �ִϸ��̼� ������Ʈ
public:
	HRESULT Clone_NotifyFromPrototype(const wstring& strProtoTag);
	// ��Ƽ���� �̺�Ʈ ���
	HRESULT Regist_EventToNotify(const string& strAnimName, const string& strCollectionName,
		const string& strNotifyName, FastDelegate0<> Event);
	HRESULT Regist_ModelCompToNotify(weak_ptr<CCommonModelComp> pModelComp);

public:
	shared_ptr<CAnimationComponent> AnimationComp() { return m_pAnimationComp; }

private:
	shared_ptr<CAnimationComponent> m_pAnimationComp = { nullptr };
#pragma endregion




#pragma region ���̴� ������Ʈ
	// �ܺ� ������
public:
	// ����Ʈ�� ���ε� �Ѵ�.
	HRESULT Link_Shader(const wstring& strEffectKey, const D3D11_INPUT_ELEMENT_DESC* pElements = nullptr, _uint iNumElements = 0);
	// ����Ʈ�� ����ε� �Ѵ�. �Ⱦ���
	HRESULT Unlink_Shader();
	// ���̴� ���
	HRESULT Begin_Shader(_uint iPass);

	shared_ptr<CShader> ShaderComp() const { return m_pShaderComp; }

private:
	shared_ptr<CShader> m_pShaderComp = { nullptr };
#pragma endregion

};

END