#pragma once

#include "Base.h"

BEGIN(Engine)

/// <summary>
/// �ַ� ���� ���� ���̴� Ŭ����
/// </summary>
class ENGINE_DLL FBoneData final : public CBase
{
	DERIVED_CLASS(CBase, FBoneData)

protected:
	explicit FBoneData() = default;
	explicit FBoneData(const FBoneData& rhs);
	virtual ~FBoneData() = default;

public:
	static FBoneData*	Create();
	virtual FBoneData*	Clone();
	virtual void		Free() override;

public:
	wstring			strName;				// ��� �̸�

	_short			iParentID = { -1 };		// �θ� ��� ID
	_short			iID = -1;				// �� ���п� ID
	
	vector<_short>	iChildrenID;			// �ڽ� ��� ID
	
};


/// <summary>
/// �� ��� ������ ����
/// </summary>
class ENGINE_DLL CBoneGroup final : public CBase
{
	DERIVED_CLASS(CBase, CBoneGroup)

private:
	explicit CBoneGroup();
	explicit CBoneGroup(const CBoneGroup& rhs);
	virtual ~CBoneGroup() = default;

public:
	static CBoneGroup* Create();
	virtual CBoneGroup* Clone();
	virtual void Free() override;

public:
	FBoneData* Find_BoneData(_int iID);
	FBoneData* Find_BoneData(const wstring& strBoneNodeKey);
	FBoneData* Find_RootTransBoneData();
	FBoneData* Find_RootRotateBoneData();
	FBoneData* Find_RootScaleBoneData();

public:
	// �� �߰��Լ�
	HRESULT		Add_BoneData(const wstring& strBoneNodeKey, FBoneData* pNode, _fmatrix& TransformMatrix);

public:
	// Ʈ������ ����� Ư�� ���� �����Ѵ�. �ִϸ��̼��� ������ �� ���δ�.
	HRESULT		Invalidate_BoneTransform(_uint iIndex, _fmatrix& matTransform);
	HRESULT		Invalidate_BoneTransforms(vector<_float4x4>& Transforms, vector<_bool>& Applies);
	// �� ������Ʈ �Լ�, �ִϸ��̼� ���� �Ŀ� �ҷ��´�.
	void		Invalidate_FinalTransforms();
	void		Provide_ChildrenList(vector<_short>& vecBones, const wstring& strBoneNodeKey, const vector<wstring>& strEndBones, _uint iRecursive) const;

private:
	void		Provide_ChildrenList_Recursive(vector<_short>& vecBones, const vector<_short>& vecEndBones, const _short& iIndex, _uint iRecursive) const;

public:
	// ���� ����
	const _uint					CRef_BoneDatas_Count() const { return m_iNumBones; }
	vector<FBoneData*>			Get_BoneDatasVector() const { return m_BonesVector; }
	const vector<_float4x4>&	CRef_BoneTransforms() const { return m_TransformMatrices; }
	const vector<_float4x4>&	CRef_BoneCombinedTransforms() const { return m_CombinedTransformMatrices; }
	//const vector<_float4x4>&	CRef_BoneOffsetTransforms() const { return m_OffsetTransformMatrices; }
	void Reserve_BoneSize(_uint iNumBoneSize);

public:
	const _uint			CRef_RootTransBoneInd() const { return m_iRootTransBoneInd; }
	void				Set_RootTransBoneInd(const _uint iIndex) { m_iRootTransBoneInd = iIndex; }
	const _uint			CRef_RootRotateBoneInd() const { return m_iRootRotateBoneInd; }
	void				Set_RootRotateBoneInd(const _uint iIndex) { m_iRootRotateBoneInd = iIndex; }
	const _uint			CRef_RootScaleBoneInd() const { return m_iRootScaleBoneInd; }
	void				Set_RootScaleBoneInd(const _uint iIndex) { m_iRootScaleBoneInd = iIndex; }

public:
	// ��� ���� ��Ʈ�� ����� ����ȸ�� ������ �Ѵ�.
	void				Set_PreRotate(_matrix Matrix);

	// �� �����̳�
private:
	_uint						m_iNumBones = { 0 };			// �� ����
	map<wstring, FBoneData*>	m_BonesMap;						// �� �̸� �˻�
	vector<FBoneData*>			m_BonesVector;					// �� �ε��� �˻�

	// Ʈ������
private:
	vector<_float4x4>			m_TransformMatrices;			// �ִϸ��̼� Ʈ������. ����ȭ��
	vector<_float4x4>			m_CombinedTransformMatrices;	// �������� ����. ����ȭ��
	//vector<_float4x4>			m_OffsetTransformMatrices;		// ���������. Ư���� ��쿡 ���δ�.
	//vector<_bool>				m_IsBoneCalculated;				// ���� ���Ǿ�����, �������� �ʱ�ȭ �ؾ���.


	// ��Ʈ��� ��
private:
	_uint						m_iRootTransBoneInd = { UINT_MAX };		// ��Ʈ��� �̵� �� �ε���
	_uint						m_iRootRotateBoneInd = { UINT_MAX };	// ��Ʈ��� ȸ�� �� �ε���
	_uint						m_iRootScaleBoneInd = { UINT_MAX };		// ��Ʈ��� ������ �� �ε���

	// VTF, [Deprecated]
/*public:
	HRESULT Create_VTF_Bones();
	HRESULT Bind_TransformsToVTF();

	const _bool	CRef_IsUse_VTF() const { return m_bUse_VTF; }


	// VTF
private:
	_bool								m_bUse_VTF = { false };				// ������۰� Ŀ���� VTF�� �����. 256���� �ϰ�, �� �̻��� �ؽ�ó ó��
	_uint								m_iNumVTFRows = { 0 };				// �� ��
	_uint								m_iNumVTFColums = { 0 };			// �ٴ� ���� (���)
	_uint								m_iNumVTF_LastRowColums = { 0 };	// ������ �� ��� ����
	ComPtr<ID3D11Texture2D>				m_pVertexTexture = { nullptr };		// VTF�� �ؽ�ó
	ComPtr<ID3D11ShaderResourceView>	m_pVT_SRV = { nullptr };			// VTF ���̴� ���ҽ�


public:
	HRESULT Create_TransformBuffers();
	*/


	// GPU ���� ��Ĺ���
/*private:
	ComPtr<ID3D11Buffer>				m_TransformBuffer = { nullptr };	// GPU�� Ʈ������
	ComPtr<ID3D11ShaderResourceView>	m_TransformSRV = { nullptr };		// SRV
	ComPtr<ID3D11UnorderedAccessView>	m_TransformUAV = { nullptr };		// UAV

	ComPtr<ID3D11Buffer>				m_CombinedTransformBuffer = { nullptr };	// GPU�� ���� ��� ����
	ComPtr<ID3D11ShaderResourceView>	m_CombinedTransformSRV = { nullptr };		// SRV
	ComPtr<ID3D11UnorderedAccessView>	m_CombinedTransformUAV = { nullptr };		// UAV
	*/
};

END