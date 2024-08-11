#pragma once

#include "Base.h"

BEGIN(Engine)

struct FKeyFrame
{
	_float fTrackPosition;
	_float3 vPos;
	_float4 qtRot;
	_float3 vScale;
};

// GPU��
struct FBoneKeyFrame
{
	_int iBoneID;
	_float fTrackPosition;
	_float3 vPos;
	_float4 qtRot;
	_float3 vScale;
};

/// <summary>
/// Ʈ�� ��� ���� ���� Ű�����Ӱ�
/// </summary>
struct FKeyFrameTR
{
	_int iBoneID;
	_float3 vPos;
	_float4 qtRot;
	_float3 vScale;
};

// �ܼ� Ű������ ���� �� �� ���δ�. �̋��� Ű �������� ����� ���� �־�� �Ѵ�.
struct FKeyFrameInterpolate
{
	_float				fWeight;
	vector<FKeyFrameTR>	KeyFrames;
};

// �ִϸ��̼� ������ ����, �ִϸ��̼ǿ� ���� ��ġ�� ���� �����ϱ� ������. ����ũ ������ Ʈ�������� �� ���δ�.
struct FAnimInterpolate
{
	_int			iAnimID;		// ���� �ĺ� �ִϸ��̼� ID
	vector<_int>	vecChannelIDs;	// ���� �ĺ� �ִϸ��̼��� ä�� ID��
	_float			fTrackPos;		// Ʈ�� ��ġ
	_float			fWeight;		// ���� ����ġ
};

/// <summary>
/// �ִϸ��̼� ä���� �����Ӻ� ���
/// </summary>
class ENGINE_DLL FBoneAnimChannelData final : public CBase
{
	DERIVED_CLASS(CBase, FBoneAnimChannelData)
private:
	explicit FBoneAnimChannelData() = default;
	explicit FBoneAnimChannelData(const FBoneAnimChannelData& rhs);
	virtual ~FBoneAnimChannelData() = default;

public:
	static FBoneAnimChannelData* Create();
	virtual void Free() override;

public:
	/// <summary>
	/// ������ �ð����� ������ ����� ��ȯ�ϴ� �Լ�
	/// </summary>
	/// <param name="fTime">������ ������ �־��ش�.</param>
	/// <returns></returns>
	_float4x4 Interpolated_Matrix(const _float& fCurTrackPos) const;
	FKeyFrameTR Interpolated_KeyFrame(const _float& fCurTrackPos) const;

	// ���� �迭�� �������� ã���ִ� �Լ�, �ش� �������� ���� �ִϸ��̼� ��ġ�� �˾Ƴ���.
	// ��ƿ���� ���ؼ� ���� ��ġ�� �����ϴ� ����� ������� �ʰ�, �������� ã���ش�.
	// �ݺ����� ���� �������� ã�Ƴ��µ�. �⺻���� �������� ������ ���ϸ�, �̴� Baking�� �����Ϳ� ���� ó����
	// �ظ��� ��Ȳ���� ����ð��� �˰����� ���´�.
	_uint Calculate_Pivot(const _float& fCurTrackPos) const;

public:
	_int					iBoneID = -1;			// �� ID
	_uint					iNumKeyFrames;			// Ű ������ ����
	vector<FKeyFrame>		vecKeyFrames;			// ��ġ �����͵�


};



/// <summary>
/// �ִϸ��̼� ��带 �����ϴ� ������
/// ��ü ��� �ð��� ����ӵ��� ����
/// </summary>
class ENGINE_DLL FBoneAnimData final : public CBase
{
	DERIVED_CLASS(CBase, FBoneAnimData)
private:
	explicit FBoneAnimData() = default;
	explicit FBoneAnimData(const FBoneAnimData& rhs) = delete;
	virtual ~FBoneAnimData() = default;

public:
	static FBoneAnimData* Create();
	virtual void Free() override;

public:
	const FBoneAnimChannelData* const Find_AnimChannelData(_uint iIndex) const;
	const FBoneAnimChannelData* const Find_AnimChannelData(const wstring& strNodeKey) const;
	void Add_AnimChannelData(const wstring& strNodeKey, FBoneAnimChannelData* pAnimNodeData);

public:
	// �ð� ��ȭ���� �ִϸ��̼� Ÿ�Ӷ����� ���� �ð��� �����ִ� �Լ�, Mod�� �Ѹ� �ݺ���
	_float Calculate_Time(_float fCurTime, _bool bMod = true) const;
	// ��� ä�� �߰��� ���Ŀ� ������ ��.
	HRESULT Create_ChannelBuffers();
	
	
public:
	_uint	iID = 0;				// �ִϸ��̼� ID
	wstring strName = L"";			// �ִϸ��̼� �̸�
	_float	fDuration = 0.0;		// ���� ����
	_float	fTickPerSecond = 0.0;	// �ð��� ������

public:
	map<const wstring, FBoneAnimChannelData*>	mapAnimChannels;	// ��� �̸����� �˻� �ý���
	vector<FBoneAnimChannelData*>				vecAnimChannels;	// �ִϸ��̼� �ε��� �˻� �ý���
	
public:
	vector<ComPtr<ID3D11Buffer>>				AnimChannelBuffers;	// �ִϸ��̼� ����ä��, ���� Ű���������� ����
	vector<ComPtr<ID3D11ShaderResourceView>>	AnimChannelSRVs;	// SRV. �б�����θ� �� ����.
	
};



/// <summary>
/// �� �ִϸ��̼ǵ��� �����ϴ� �׷�
/// </summary>
class ENGINE_DLL CBoneAnimGroup final : public CBase
{
	DERIVED_CLASS(CBase, CBoneAnimGroup)
private:
	explicit CBoneAnimGroup() = default;
	explicit CBoneAnimGroup(const CBoneAnimGroup& rhs) = delete;
	virtual ~CBoneAnimGroup() = default;

public:
	static CBoneAnimGroup* Create();
	virtual void Free() override;

public:
	FBoneAnimData* const	Find_BoneAnim(const _uint iIndex);
	FBoneAnimData* const	Find_BoneAnim(const wstring& strAnimKey);
	HRESULT Add_BoneAnim(const wstring& strAnimKey, FBoneAnimData* pAnimData);

	
	// �� �ִϸ��̼ǿ� ���� ��� �ִϸ��̼��� Ű�� ������ �� ����Ѵ�.
	// �̵� �����ڸ� ���� �����Ѵ�.
	vector<FKeyFrameTR> Provide_AnimKeyFrames(_uint iAnimIndex, const _float& fTrackPos, const class CBoneGroup& BoneGroup) const;

	// �� �ִϸ��̼ǿ� ���� �Ϻ� �ִϸ��̼��� Ű�� ������ �� ����Ѵ�. �θ���� ������ ����.
	vector<FKeyFrameTR> Provide_AnimKeyFrames_ParentedDown(
		_uint iAnimIndex, const _float& fTrackPos, const class CBoneGroup& BoneGroup
		, const wstring& strParentBoneName, const vector<wstring>& strEndBones, _uint iRecursive = UINT_MAX) const;

	// �� �ִϸ��̼ǿ� ���� �Ϻ� �ִϸ��̼��� Ű�� ������ �� ����Ѵ�. �θ���� ������ ����.
	vector<FKeyFrameTR> Provide_AnimKeyFrames_ParentedUp(
		_uint iAnimIndex, const _float& fTrackPos, const class CBoneGroup& BoneGroup
		, const wstring& strParentBoneName, _uint iRecursive = UINT_MAX) const;

	// �� �ִϸ��̼ǿ� ���� ���� �������� �޾Ƴ��� �� �� ����Ѵ�.
	FKeyFrameTR Interpolated_Anim(const _uint iAnimIndex, const _uint iChannelIndex, const _float& fCurTrackPos) const;

	// ������ �ʿ��� ������ �޾� ������ �ִϸ��̼��� �̿��� ������ Ű�������� �����ִ� �Լ�
	void Interpolated_Anims(FKeyFrameTR* pKeyFrames, size_t iNumKeyFrames, FAnimInterpolate* pArrInterpolateData, size_t iNumInterpolates);
	
	// Ű�����ӳ��� ����, �̹� ���� Ű������ ������� ���� �����Ѵ�.
	void Interpolated_KeyFrames(FKeyFrameTR* pKeyFrames, size_t iNumKeyFrames, FKeyFrameInterpolate* pArrInterpolate, size_t iNumInterpolates);
	
private:
	void CalcThread_Interpolated_KeyFrames(_uint iIndex, FBoneAnimData* pBoneAnim, FAnimInterpolate* pData, _float fWeightRatio,
		_vector* vPos, _vector* vRot, _vector* vScale);

public:
	const _uint& Get_NumAnims() const { return iNumAnims; }
	const vector<FBoneAnimData*>& Get_BoneAnimDatasVector() const { return vecAnimDatas; }

private:
	_uint									iNumAnims;
	unordered_map<wstring, FBoneAnimData*>	mapAnimDatas;	// �ִϸ��̼� ���� ��, Ű�� �ִϸ��̼� �̸�
	vector<FBoneAnimData*>					vecAnimDatas;	// �ִϸ��̼� �ε��� �˻�
	
};

END